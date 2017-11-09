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
//---------------------------------------------------------------------------
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#include "ImageFormat_pfm.h"
using namespace LibSL::Image;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Tuple.h>
using namespace LibSL::Math;

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
using namespace std;

//---------------------------------------------------------------------------

#define NAMESPACE LibSL::Image

//---------------------------------------------------------------------------

/// Declaring a global will automatically register the plugin
namespace {
  NAMESPACE::ImageFormat_pfm s_Pfm;  /// FIXME: this mechanism does not work with VC++
}                                    ///        see also ImageFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::ImageFormat_pfm::ImageFormat_pfm()
{
  try {
    // register plugin
    IMAGE_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

void NAMESPACE::ImageFormat_pfm::readLine(FILE *f,char *str,uint len) const
{
  uint n = 0;
  str[0] = '\0';
  while (1) {
    char   c  = '\0';
    size_t nr = fread(&c,1,1,f);
    if (!(isalnum(c) || (c == '-' && n == 0) || (c == '.'))) {
      break;
    }
    str[n++] = c;
    if (n >= len) {
      throw Fatal("ImageFormat_pfm - error reading file header");
    }
  }
  str[n] = '\0';
}

//---------------------------------------------------------------------------

NAMESPACE::Image *NAMESPACE::ImageFormat_pfm::load(const char *name) const
{
  size_t nr = 0;
  FILE  *f  = NULL;
	fopen_s(&f, name, "rb");
  if (f == NULL) {
    throw Fatal("ImageFormat_pfm::load - Sorry, cannot open file '%s'",name);
  }
  // read header
  uint  w    = 0;
  uint  h    = 0;
  uchar numc = 0;
  static char str[1024];
  // identifier
  nr = fread(str,3,1,f);
  if (str[0] != 'P') {
    throw Fatal("ImageFormat_pfm::load - '%s' does not appear to be in PFM format",name);
  }
  if (str[1] == 'f') {
    numc = 1;
  } else if (str[1] == 'F') {
    numc = 3;
  } else {
    throw Fatal("ImageFormat_pfm::load - '%s' does not appear to be in PFM format",name);
  }
  // width
  readLine(f,str,1024);
  w = atoi(str);  
  // height
  readLine(f,str,1024);
  h = atoi(str);
  // scale factor / endianness
  readLine(f,str,1024);
  float scale = float(atof(str)); // ignored
  // assert
  sl_assert(w > 0 && h > 0 && numc > 0);
  // create image
  Image *img = NULL;
  switch (numc) {
   case 1: img = new Image_generic<float,1>(w,h); break;
   case 3: img = new Image_generic<float,3>(w,h); break;
  default: 
    throw Fatal("ImageFormat_pfm::load - '%s' does not appear to be in PFM format",name);
    break;
  }
  // read data
  nr = fread(img->raw(),w*h*numc*sizeof(float),1,f);
  // done
  fclose(f);
  // flip ?
  if (scale == -1) {
    switch (numc) {
    case 1: dynamic_cast<Image_generic<float,1>*>(img)->flipH(); break;
    case 3: dynamic_cast<Image_generic<float,3>*>(img)->flipH(); break;
    }
  }
  return (img);
}

//---------------------------------------------------------------------------

void NAMESPACE::ImageFormat_pfm::save(const char *name,const NAMESPACE::Image *img) const
{
  sl_assert(dynamic_cast<const ImageFloat1*>(img) != NULL
    ||      dynamic_cast<const ImageFloat3*>(img) != NULL);
  const ImageFloat1 *imgf1 = dynamic_cast<const ImageFloat1*>(img);
  const ImageFloat3 *imgf3 = dynamic_cast<const ImageFloat3*>(img);
  ImageFloat1_Ptr f1;
  if (imgf1) f1 = ImageFloat1_Ptr(new ImageFloat1(*dynamic_cast<const ImageFloat1*>(imgf1)));
  ImageFloat3_Ptr f3;
  if (imgf3) f3 = ImageFloat3_Ptr(new ImageFloat3(*dynamic_cast<const ImageFloat3*>(imgf3)));

  const char *id = img->numComp() == 1 ? "Pf\n" : "PF\n";
  FILE *f = NULL;
	fopen_s(&f, name, "wb");
  if (f == NULL) {
    throw Fatal("ImageFormat_pfm::save - Sorry, cannot open file '%s'",name);
  }
  fwrite(id,3,1,f);
  const char *str = LibSL::CppHelpers::sprint("%d %d\n-1.0000000\n",img->w(),img->h());
  fwrite(str,strlen(str),1,f);
//  fwrite(img->raw(),img->w()*img->h()*img->numComp()*sizeof(float),1,f);
  // All this mess is for compatibility with HDRShop which does not understand a scaling != -1.0
  if (!f1.isNull()) {
    f1->flipH();
    fwrite(f1->raw(),f1->w()*f1->h()*f1->numComp()*sizeof(float),1,f);
  }
  if (!f3.isNull()) {
    f3->flipH();
    fwrite(f3->raw(),f3->w()*f3->h()*f3->numComp()*sizeof(float),1,f);
  }
  fclose(f);
}

//---------------------------------------------------------------------------
