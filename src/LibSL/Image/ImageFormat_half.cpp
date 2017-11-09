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

#include "ImageFormat_half.h"
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
  NAMESPACE::ImageFormat_half s_Half;  /// FIXME: this mechanism does not work with VC++
}                                          ///        see also ImageFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::ImageFormat_half::ImageFormat_half()
{
  try {
    // register plugin
    IMAGE_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

NAMESPACE::Image *NAMESPACE::ImageFormat_half::load(const char *name) const
{
  FILE *f = NULL;
  fopen_s(&f,name,"rb");
  if (f == NULL) {
    throw Fatal("ImageFormat_half::load - Sorry, cannot open file '%s'",name);
  }
  // read header
  uint  w    = 0;
  uint  h    = 0;
  uchar numc = 0;
  fread(&w   ,sizeof(uint) ,1,f);
  fread(&h   ,sizeof(uint) ,1,f);
  fread(&numc,sizeof(uchar),1,f);
  sl_assert(w > 0 && h > 0 && numc > 0);
  Image *img = NULL;
  switch (numc) {
   case 1: img = new Image_generic<half,1>(w,h); break;
   case 2: img = new Image_generic<half,2>(w,h); break;
   case 3: img = new Image_generic<half,3>(w,h); break;
   case 4: img = new Image_generic<half,4>(w,h); break;
  }
  // read data
  fread(img->raw(),w*h*numc*sizeof(half),1,f);
  // done
  fclose(f);
  return (img);
}

//---------------------------------------------------------------------------

void NAMESPACE::ImageFormat_half::save(const char *name,const NAMESPACE::Image *img) const
{
  FILE *f = NULL;
  fopen_s(&f,name,"wb");
  if (f == NULL) {
    throw Fatal("ImageFormat_half::save - Sorry, cannot open file '%s'",name);
  }
  // save header
  uint  w    = img->w();
  uint  h    = img->h();
  uchar numc = img->numComp();
  fwrite(&w   ,sizeof(uint) ,1,f);
  fwrite(&h   ,sizeof(uint) ,1,f);
  fwrite(&numc,sizeof(uchar),1,f);
  // save data
  fwrite(img->raw(),w*h*numc*sizeof(half),1,f);
  // done
  fclose(f);
}

//---------------------------------------------------------------------------
