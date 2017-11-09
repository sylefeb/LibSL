/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@sophia.inria.fr

Simple Library for Graphics (LibSL)

This software is a computer program whose purpose is to offer a set of
tools to simplify programming real-time computer graphics applications
under OpenGL and DirectX.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-C
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------- */
// --------------------------------------------------------------

#pragma once

// --------------------------------------------------------------

#include <LibSL/LibSL.h>
#include "SparseMatrix.h"

#include <NL/nl.h>

// --------------------------------------------------------------

namespace LibSL 
{
  class LinearSolver
  {
  protected:

    SparseMatrix                          m_tA;     // system matrix
    Array  <double>                       m_b;      // b vector
    std::vector<std::pair<int,double> >   m_Locked;
    std::map<int,double>                  m_Init;   // initial values

  public:

    void prepareSolver()
    {

    }

    void solve(Array<double>& x,int solver = NL_CG)
    {
      LIBSL_BEGIN;

      // prepare x vector
      x.erase();
      x.allocate(m_b.size());
      x.fill(0);

      nlNewContext();
      nlSolverParameteri(NL_NB_VARIABLES, m_b.size() );
      if ( m_tA.numC() > m_tA.numR()-(int)m_Locked.size() ) {
        // cerr << "[LinearSolver] Switching to least square" << endl;
        nlSolverParameteri(NL_LEAST_SQUARES, NL_TRUE );
      } else {
        nlSolverParameteri(NL_LEAST_SQUARES, NL_FALSE );
      }
      nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI);
      nlSolverParameteri(NL_SOLVER, solver);
      nlSolverParameterd(NL_THRESHOLD,1e-6);
      nlSolverParameterd(NL_MAX_ITERATIONS,10000);
      nlBegin(NL_SYSTEM);
      ForIndex(i,m_b.size()) {
        nlSetVariable(i,0.0f);
      }
      typedef std::map<int,double> t_map;
      ForConstIterator(t_map,m_Init,V) {
        nlSetVariable(V->first,V->second);
      }
      ForIndex(l,m_Locked.size()) {
        nlSetVariable(m_Locked[l].first,m_Locked[l].second);
        nlLockVariable(m_Locked[l].first);
      }

      nlBegin(NL_MATRIX);
      typedef std::map<int,double> t_row;
      ForIndex(r,m_tA.m().size()) {
        nlRowParameterd(NL_RIGHT_HAND_SIDE, m_b[r] );
        nlBegin(NL_ROW);
        ForIterator(t_row,m_tA.m()[r],C) {
//          cerr << sprint("[%d,%f] ",C->first, C->second);
          nlCoefficient( C->first, C->second );
        }
        nlEnd(NL_ROW);
//        cerr << endl;
      }    
      nlEnd(NL_MATRIX);
      nlEnd(NL_SYSTEM);

      nlSolve();

	  ForIndex(i,x.size()) {
        x[i] = nlGetVariable( i );
      }

	  nlDeleteContext(nlGetCurrent());

      LIBSL_END;
    }

  public:

    LinearSolver()
    {
    }

    ~LinearSolver()
    {
    }

    void allocate(int numEqns,int numVars)
    {
      m_tA.allocate( numEqns , numVars );
      b() .allocate( numEqns );
      b() .fill(0.0);
    }

    double&  coeff(int eqn,int var)       
    {  
      sl_assert(eqn < m_tA.numC());
      sl_assert(var < m_tA.numR());
      return m_tA.m()[ eqn ][ var ]; 
    }

    double   coeff(int eqn,int var) const 
    { 
      sl_assert(eqn < m_tA.numC());
      sl_assert(var < m_tA.numR());
      const std::map<int,double>& _map       = m_tA.m()[ eqn ];
      std::map<int,double>::const_iterator I = _map.find(var);
      if (I == _map.end()) {
        return 0.0;
      } else {
        return (*I).second;
      }
    }

    void lock(int var,double value)
    {
      m_Locked.push_back( std::make_pair(var,value) );
    }

    void init(int var,double value)
    {
      m_Init[ var ] = value;
    }

    Array<double>&               b()  { return m_b;  }

  };
} //namespace LibSL

// --------------------------------------------------------------
