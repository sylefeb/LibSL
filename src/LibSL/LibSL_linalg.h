// ------------------------------------------------------
// LibSL_linalg - Linear Algebra
// ------------------------------------------------------
// Christian Eisenacher - 2006-11-20 newmat and PCA
// Sylvain Lefebvre     - 2009-04-28 linear algebra package, 
//                        adding TAUCS and linear solver
// ------------------------------------------------------

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


// Output of matrices (e.g. the vectors obtained from PCA)
#define WANT_STREAM
// Include Matrix IO to streams (newmat.h included from within)
#include <newmat/newmatio.h>

// PCA algorithms based on newmat
#include <LibSL/LinAlg/PCA.h>
// Linear solver (based on OpenNL)
#include <LibSL/LinAlg/LinearSolver.h>

#include <LibSL/Image/PushPull.h>

using namespace NEWMAT;
