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
#ifdef HAS_TAUCS

// --------------------------------------------------------------

#include <LibSL/LibSL.h>
#include "shlwapi.h"

#include "TaucsHelpers.h"
#include "SparseMatrix.h"

// --------------------------------------------------------------

namespace LibSL {

class LeastSquareSolver
{
protected:

  SparseMatrix                m_tA;     // system matrix
  Array  <double>             m_b;      // b vector

  taucs_ccs_matrix           *m_TaucsA; // system matrix as taucs matrix
  taucs_ccs_matrix           *m_F;      // factored system matrix

  Array<double>               m_Atb;    // Atb vector, hold in memory to avoid realloc

  void computeAtA(SparseMatrix& _AtA)
  {
    LIBSL_BEGIN;

    // transfer system matrix into a taucs matrix
    taucs_ccs_matrix *tA = TaucsHelpers::buildTaucsMatrix(m_tA);
    // transpose
    if (m_TaucsA != NULL) {
      taucs_dccs_free(m_TaucsA);
    }
    m_TaucsA             = TaucsHelpers::transposeMatrix(tA);
    // free temporary taucs matrix
    taucs_dccs_free(tA);
    
    {
      // compute AtA
      // Timer tm(" [AtA           ]");

      _AtA.allocate(m_tA.numR(),m_tA.numR());

      ForIndex(i,m_tA.numR()) {
        ForIndex(j,m_tA.numR()) {
          double v = 0.0;
          uint n_ci = m_TaucsA->colptr[i+1] - m_TaucsA->colptr[i];
          uint n_cj = m_TaucsA->colptr[j+1] - m_TaucsA->colptr[j];
          uint ci   = 0;
          uint cj   = 0;
          bool nnz  = false;
          while (ci < n_ci && cj < n_cj) {
            int ri = m_TaucsA->rowind[m_TaucsA->colptr[i] + ci];
            int rj = m_TaucsA->rowind[m_TaucsA->colptr[j] + cj];
            if (ri < rj) {
              ci ++;
            } else if (rj < ri) {
              cj ++;
            } else {
              sl_assert(ri == rj);
              v += m_TaucsA->values.d[m_TaucsA->colptr[i] + ci] * m_TaucsA->values.d[m_TaucsA->colptr[j] + cj];
              nnz = true;
              ci ++;
              cj ++;
            }
          }
          if (nnz) {
            sl_assert(i < (int)_AtA  .m().size());
            _AtA  .m()[i].insert(std::make_pair(j,v));
          }
        }
      }
    }
    LIBSL_END;
  }

  void computeAtB(Array<double>& _Atb)
  {
    LIBSL_BEGIN;
    // TODO: check matrix exists
    // Timer tm(" [Atb           ]");
    if (_Atb.size() != m_TaucsA->n ) {
      _Atb.erase();
      _Atb.allocate(m_TaucsA->n);
    }
    ForIndex(j,m_TaucsA->n) {
      int     cj = m_TaucsA->colptr[j];
      int     cn = m_TaucsA->colptr[j+1] - m_TaucsA->colptr[j];
      double  v  = 0.0;
      ForIndex(i,cn) {
        int    r   = m_TaucsA->rowind  [cj + i];
        double aij = m_TaucsA->values.d[cj + i];
        v         += aij * m_b[r];
      }
      _Atb[j] = v;
    }
    LIBSL_END;
  }

public:

  bool load(const char *fname)
  {
    if (!PathFileExistsA(sprint("%s.mF",fname))) {
      return false;
    }
    if (!PathFileExistsA(sprint("%s.mA",fname))) {
      return false;
    }
    m_F      = TaucsHelpers::loadMatrix(sprint("%s.mF",fname));
    m_TaucsA = TaucsHelpers::loadMatrix(sprint("%s.mA",fname));
    return true;
  }

  void save(const char *fname)
  {
    TaucsHelpers::saveMatrix(sprint("%s.mF",fname),m_F);
    TaucsHelpers::saveMatrix(sprint("%s.mA",fname),m_TaucsA);
  }

  void prepareSolver()
  {
    LIBSL_BEGIN;

    // compute AtA in a sparse matrix
    SparseMatrix AtA;
    computeAtA(AtA);

    // transfer AtA in a taucs matrix
    taucs_ccs_matrix *taucsAtA = TaucsHelpers::buildTaucsSymmetricMatrix(AtA);

    // factor system
    {
      Timer tm(" [factorization]");
      if (m_F != NULL) {
        taucs_dccs_free((taucs_ccs_matrix*)m_F);
      }
      m_F        = taucs_ccs_factor_llt(taucsAtA,0,0);
      sl_assert(m_F != NULL);
    }

    // free temporary AtA matrix
    taucs_dccs_free(taucsAtA);

    LIBSL_END;
  }

  void solve(Array<double>& x)
  {
    LIBSL_BEGIN;

    sl_assert (m_F      != NULL);
    sl_assert (m_TaucsA != NULL);

    // compute AtB
    computeAtB(m_Atb);

    // prepare x vector
    x.erase();
    x.allocate(m_Atb.size());
    x.fill(0);

    {
      // Timer tm(" [solve         ]");
      // call solver
      switch (taucs_ccs_solve_llt(m_F,x.raw(),m_Atb.raw()))
      {
      case TAUCS_SUCCESS:             break;
      case TAUCS_ERROR:               std::cerr << "TAUCS_ERROR" << std::endl; break;
      case TAUCS_ERROR_NOMEM:         std::cerr << "TAUCS_ERROR_NOMEM" << std::endl; break;
      case TAUCS_ERROR_BADARGS:       std::cerr << "TAUCS_ERROR_BADARGS" << std::endl; break;
      case TAUCS_ERROR_MAXDEPTH:      std::cerr << "TAUCS_ERROR_MAXDEPTH" << std::endl; break;
      case TAUCS_ERROR_INDEFINITE:    std::cerr << "TAUCS_ERROR_INDEFINITE" << std::endl; break;
      default:                        std::cerr << "TAUCS_ERROR (unknown)" << std::endl; break;
      }
    }
/*
    // prepare x vector
    x.erase();
    x.allocate(m_Atb.size());
    x.fill(0);

    static const char* options[] = 
    {
      "taucs.factor.LLT=true",
      "taucs.factor.droptol=1e-2",
      "taucs.solve.cg=true",
      NULL
    };

    switch (taucs_linsolve(
      m_TaucsA,
      NULL,
      1,
      x.raw(),
      m_b.raw(),
      (char**) options,
      (void**) NULL)) 
    {
      case TAUCS_SUCCESS:             break;
      case TAUCS_ERROR:               std::cerr << "TAUCS_ERROR" << std::endl; break;
      case TAUCS_ERROR_NOMEM:         std::cerr << "TAUCS_ERROR_NOMEM" << std::endl; break;
      case TAUCS_ERROR_BADARGS:       std::cerr << "TAUCS_ERROR_BADARGS" << std::endl; break;
      case TAUCS_ERROR_MAXDEPTH:      std::cerr << "TAUCS_ERROR_MAXDEPTH" << std::endl; break;
      case TAUCS_ERROR_INDEFINITE:    std::cerr << "TAUCS_ERROR_INDEFINITE" << std::endl; break;
      default:                        std::cerr << "TAUCS_ERROR (unknown)" << std::endl; break;
    }
*/
    LIBSL_END;
  }

public:

  LeastSquareSolver()
  {
    m_TaucsA   = NULL;
    m_F        = NULL;
  }

  ~LeastSquareSolver()
  {
    if (m_TaucsA != NULL) {
      taucs_dccs_free(m_TaucsA);
    }
    if (m_F != NULL) {
      taucs_dccs_free(m_F);
    }
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

  Array<double>&               b()  { return m_b;  }

};

} // namespace LibSL

#endif

// --------------------------------------------------------------
