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
#include "precompiled.h"

#include "test_memory_perf.h"

#include <cstring>
#include <iostream>
#include <ctime>

using namespace std;

#define N1 4096
#define STEPS1 10000
#define N2 256
#define STEPS2 1000
#define IX(i,j) ((i)+(N2+2)*(j))

void test_std_array()
{
  srand(42);
  int data[N1+2];
  for (int i = 0; i < N1+2; i++) {
    data[i] = rand();
  }
  for (int s = 0; s < STEPS1; s++) {
    for (int i = 1; i <= N1; i++) {
      data[i] = (data[i-1]+data[i+1]) / 2;
    }
  }
  int s = 0;
  for (int i = 0; i < N1+2; i++) {
    s += data[i];
    s %= 1024;
  }
  cout << s << endl;
}

typedef Array<int,
	      LibSL::Memory::Array::InitNop,
	      LibSL::Memory::Array::CheckAll> Array_check;
typedef Array<int,
	      LibSL::Memory::Array::InitNop,
	      LibSL::Memory::Array::CheckNop> Array_nocheck;


void test_Array()
{
  srand(42);
  Array_nocheck data;
  data.allocate(N1+2);
  ForArray(data,i) {
    data[i] = rand();
  }
  for (int s = 0; s < STEPS1; s++) {
    for (int i = 1; i <= N1; i++) {
      data[i] = (data[i-1]+data[i+1]) / 2;
    }
  }
  int s = 0;
  ForArray(data,i) {
    s += data[i];
    s %= 1024;
  }
  cout << s << endl;
}

void test_Array_check()
{
  srand(42);
  Array_check data;
  data.allocate(N1+2);
  ForArray(data,i) {
    data[i] = rand();
  }
  for (int s = 0; s < STEPS1; s++) {
    for (int i = 1; i <= N1; i++) {
      data[i] = (data[i-1]+data[i+1]) / 2;
    }
  }
  int s = 0;
  ForArray(data,i) {
    s += data[i];
    s %= 1024;
  }
  cout << s << endl;
}

void test_std_array_2D_1D()
{
  srand(42);
  int data[(N2+2)*(N2+2)];
  for (int i = 0; i < (N2+2)*(N2+2); i++) {
    data[i] = rand();
  }
  for (int s = 0; s < STEPS2; s++) {
    for (int j = 1; j <= N2; j++) {
      for (int i = 1; i <= N2; i++) {
	data[IX(i,j)] = (data[IX(i-1,j)]+data[IX(i+1,j)]+data[IX(i,j-1)]+data[IX(i,j+1)]) / 4;
      }
    }
  }
  int s = 0;
  for (int j = 0; j < N2+2; j++) {
    for (int i = 0; i < N2+2; i++) {
      s += data[IX(i,j)];
      s %= 1024;
    }
  }
  cout << s << endl;
}

void test_std_array_2D()
{
  srand(42);
  int data[N2+2][N2+2];
  for (int i = 0; i < N2+2; i++) {
    for (int j = 0; j < N2+2; j++) {
      data[i][j] = rand();
    }
  }
  for (int s = 0; s < STEPS2; s++) {
    for (int i = 1; i <= N2; i++) {
      for (int j = 1; j <= N2; j++) {
	data[i][j] = (data[i-1][j]+data[i+1][j]+data[i][j-1]+data[i][j+1]) / 4;
      }
    }
  }
  int s = 0;
  for (int i = 0; i < N2+2; i++) {
    for (int j = 0; j < N2+2; j++) {
      s += data[i][j];
      s %= 1024;
    }
  }
  cout << s << endl;
}


typedef Array2D<int,
		LibSL::Memory::Array::InitNop,
		LibSL::Memory::Array::CheckAll> Array2D_check;
typedef Array2D<int,
		LibSL::Memory::Array::InitNop,
		LibSL::Memory::Array::CheckNop> Array2D_nocheck;

void test_Array2D()
{
  srand(42);
  Array2D_nocheck data;
  data.allocate(N2+2,N2+2);
  ForArray2D(data,i,j) {
    data.set(i,j) = rand();
  }
  for (int s = 0; s < STEPS2; s++) {
    for (int j = 1; j <= N2; j++) {
      for (int i = 1; i <= N2; i++) {
      data.set(i,j) = (data.get(i-1,j)+data.get(i+1,j)+data.get(i,j-1)+data.get(i,j+1)) / 4;
      }
    }
  }
  int s = 0;
  ForArray2D(data,i,j) {
    s += data.get(i,j);
    s %= 1024;
  }
  cout << s << endl;
}

void test_Array2D_check()
{
  srand(42);
  Array2D_check data;
  data.allocate(N2+2,N2+2);
  ForArray2D(data,i,j) {
    data.set(i,j) = rand();
  }
  for (int s = 0; s < STEPS2; s++) {
    for (int j = 1; j <= N2; j++) {
      for (int i = 1; i <= N2; i++) {
      data.set(i,j) = (data.get(i-1,j)+data.get(i+1,j)+data.get(i,j-1)+data.get(i,j+1)) / 4;
      }
    }
  }
  int s = 0;
  ForArray2D(data,i,j) {
    s += data.get(i,j);
    s %= 1024;
  }
  cout << s << endl;
}

void test_memory_perf()
{
  clock_t t, dt;

  cout << "    ####  1D tests  ####" << endl;
  cout << endl;

  cout << "Running test on a standard array" << endl;
  t = clock();
  test_std_array();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;

  cout << "Running test on a LibSL Array with no checks" << endl;
  t = clock();
  test_Array();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;

  cout << "Running test on a LibSL Array with all checks" << endl;
  t = clock();
  test_Array_check();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;
  
  cout << "    ####  2D tests  ####" << endl;
  cout << endl;

  cout << "Running test on a 2D array represented as 1D array" << endl;
  t = clock();
  test_std_array_2D_1D();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;

  cout << "Running test on a 2D standard array" << endl;
  t = clock();
  test_std_array_2D();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;

  cout << "Running test on a LibSL Array2D with no checks" << endl;
  t = clock();
  test_Array2D();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;

  cout << "Running test on a LibSL Array2D with all checks" << endl;
  t = clock();
  test_Array2D_check();
  dt = clock() - t;
  cout << "Test took " << ((double)dt/(double)CLOCKS_PER_SEC) << " seconds." << endl;
  cout << endl;
}
