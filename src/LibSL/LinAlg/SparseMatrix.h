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

#include <map>
#include <vector>

namespace LibSL 
{
  class SparseMatrix
  {
  private:

    int                                m_NumR;
    int                                m_NumC;
    std::vector<std::map<int,double> > m_M;

  public:

    SparseMatrix() :  m_NumR(0), m_NumC(0) {}
    ~SparseMatrix() { erase(); }

    void allocate(int c,int r) 
    {
      m_NumC = c;
      m_NumR = r;
      {
        //Timer tm("   erase    ");
        m_M.clear();
      }
      {
        //Timer tm("   allocate ");
        m_M.resize(c);
      }
    }

    void erase()
    {
      //Timer tm("   erase    ");
      m_M.clear();
    }

    bool isZeroAt(int c,int r) const { return m_M[c].find(r) == m_M[c].end(); }

    std::vector<std::map<int,double> >&       m()          { return m_M; }
    const std::vector<std::map<int,double> >& m()    const { return m_M; }

    int                                       numR() const { return m_NumR; }
    int                                       numC() const { return m_NumC; }

  };

  inline void transposeMatrix(const SparseMatrix& M , SparseMatrix& _T)
  {
    // allocate transposed matrix
    _T.allocate(M.numR(),M.numC());
    ForIndex(c,M.numC()) {
      for (std::map<int,double>::const_iterator R = M.m()[c].begin() ; R != M.m()[c].end() ; R++) {
        _T.m()[(*R).first].insert(std::make_pair(c,(*R).second));
      }
    }
  }

} //namespace LibSL
