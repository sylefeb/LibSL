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
// ------------------------------------------------------
#include "LibSL.precompiled.h"

#ifdef HAS_TAUCS

// ------------------------------------------------------
#include <LibSL/LibSL.h>
#include "TaucsHelpers.h"

using namespace std;

#define NAMESPACE LibSL::TaucsHelpers

// -----------

taucs_ccs_matrix *NAMESPACE::buildTaucsSymmetricMatrix( const SparseMatrix& M )
{
  //count number of non-zero elements
  int nnz = 0;

  ForIndex(I, M.numC()) {
    nnz += M.m()[I].size();
  }
 
  // allocate taucs matrix
  taucs_ccs_matrix* A = taucs_ccs_create( M.numR()/*m: rows*/, M.numC()/*n: cols*/, nnz ,TAUCS_DOUBLE);
  
  // FIXME: place flags somewhere else
  A->flags |= TAUCS_SYMMETRIC | TAUCS_LOWER;

  // fill taucs matrix
  double *vals   = A->values.d;
  int    *colptr = A->colptr; /* n+1 entries */
  int    *rowind = A->rowind;
  int     next   = 0;

  ForIndex(c,M.numC()) {
    colptr[c] = next;
    for (map<int,double>::const_iterator R = M.m()[c].begin() ; R != M.m()[c].end() ; R++) {
      if ((*R).first >= c) {
        rowind[next] = (*R).first;
        vals  [next] = (*R).second;
        next ++;
      }
    }
  }
  colptr[M.numC()] = next; // 'close' colptr

  return A;
}

// -----------

taucs_ccs_matrix *NAMESPACE::buildTaucsMatrix( const SparseMatrix& M )
{
//count number of non-zero elements
  int nnz = 0;

  ForIndex(I, M.numC()) {
    nnz += M.m()[I].size();
  }

  // allocate taucs matrix
  taucs_ccs_matrix* A = taucs_ccs_create( M.numR()/*m: rows*/, M.numC()/*n: cols*/, nnz ,TAUCS_DOUBLE);
  
  // fill taucs matrix
  double *vals   = A->values.d;
  int    *colptr = A->colptr; /* n+1 entries */
  int    *rowind = A->rowind;
  int     next   = 0;

  ForIndex(c,M.numC()) {
    colptr[c] = next;
    for (map<int,double>::const_iterator R = M.m()[c].begin() ; R != M.m()[c].end() ; R++) {
        rowind[next] = (*R).first;
        vals  [next] = (*R).second;
        next ++;
    }
  }
  colptr[M.numC()] = next; // 'close' colptr
  
  return A;
}

// -----------

taucs_ccs_matrix *NAMESPACE::transposeMatrix( taucs_ccs_matrix* A )
{
  // allocate taucs matrix
  taucs_ccs_matrix* T = taucs_ccs_create( A->n /*m: rows*/, A->m /*n: cols*/, A->colptr[A->n] ,TAUCS_DOUBLE);
  // count values per column in T
  Array<int> counts(T->n);
  counts.fill(0);
  ForIndex(c,A->n) {
    int cs  = A->colptr[c];
    int cn  = A->colptr[c+1] - cs;
    ForIndex(j,cn) {
      int r = A->rowind[cs+j];
      counts[r] ++;
    }
  }
  // at this point counts contains the number of nnz values per column in T
  // -> compute colind in T
  T->colptr[0] = 0;
  ForRange(c,1,T->n) {
    T->colptr[c] = T->colptr[c-1] + counts[c-1];
  }
  // -> distribute values
  counts.fill(0);
  ForIndex(c,A->n) {
    int cs   = A->colptr[c];
    int cn   = A->colptr[c+1] - cs;
    ForIndex(j,cn) {
      int r     = A->rowind  [cs+j];
      double v  = A->values.d[cs+j];
      // store in T
      int t  = T->colptr[r];
      T->rowind  [t+counts[r]] = c;
      T->values.d[t+counts[r]] = v;
      counts[r] ++;
    }
  }
  return (T);
}

// -----------

void              NAMESPACE::saveMatrix( const char *fname,taucs_ccs_matrix* A )
{
  // taucs_ccs_matrix* taucs_ccs_create(int m, int n, int nnz, flags);
  // void taucs_ccs_free (taucs_ccs_matrix* A);
  FILE *f = NULL;
  fopen_s(&f,fname,"wb");
  if (f == NULL) {
    throw Fatal("[NAMESPACE::saveMatrix] Cannot save matrix to file '%s'",fname);
  }
  int nnz = A->colptr[A->n] - A->colptr[0];
  fwrite(&(A->flags),sizeof(A->flags),1,f);
  fwrite(&(A->n)    ,sizeof(A->n),1,f);
  fwrite(&(A->m)    ,sizeof(A->m),1,f);
  fwrite(&nnz       ,sizeof(nnz),1,f);
  fwrite(A->colptr  ,sizeof(int),A->n+1,f);
  fwrite(A->rowind  ,sizeof(int)   ,nnz,f);
  fwrite(A->values.d,sizeof(double),nnz,f);
  fclose(f);
}

// -----------

taucs_ccs_matrix *NAMESPACE::loadMatrix( const char *fname )
{
  FILE *f = NULL;
  fopen_s(&f,fname,"rb");
  if (f == NULL) {
    throw Fatal("[NAMESPACE::loadMatrix] Cannot read matrix from file '%s'",fname);
  }
  int flags=0,n=0,m=0,nnz=0;
  fread(&flags,sizeof(flags),1,f);
  fread(&n    ,sizeof(n),1,f);
  fread(&m    ,sizeof(m),1,f);
  fread(&nnz  ,sizeof(nnz),1,f);
  taucs_ccs_matrix *A = taucs_ccs_create(m,n,nnz,flags);
  fread(A->colptr  ,sizeof(int),A->n+1,f);
  sl_assert(A->colptr[A->n] - A->colptr[0] == nnz);
  fread(A->rowind  ,sizeof(int)   ,nnz,f);
  fread(A->values.d,sizeof(double),nnz,f);
  fclose(f);
  return (A);
}

// -----------

#endif
