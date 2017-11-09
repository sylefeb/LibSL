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

#include "ImageFormat_float.h"
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
  NAMESPACE::ImageFormat_float s_Float;  /// FIXME: this mechanism does not work with VC++
}                                          ///      see also ImageFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::ImageFormat_float::ImageFormat_float()
{
  try {
    // register plugin
    IMAGE_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

// class to write all possible instantiation of float images from 1 to T_Max channels

template<int,int> class NewImageGeneric_aux;

template <int T_Max> class NewImageGeneric_aux<T_Max,T_Max>
{
protected:
  NAMESPACE::Image *m_Instance;
public:
  NewImageGeneric_aux(int,uint,uint) { m_Instance = NULL; }
  NAMESPACE::Image* getInstance()    { return m_Instance; }
};

template <int T,int T_Max> class NewImageGeneric_aux
  : public NewImageGeneric_aux<T+1,T_Max>
{
    typedef NewImageGeneric_aux<T+1,T_Max> Base;
protected:
    using Base::m_Instance;
public:
  NewImageGeneric_aux(int n,uint w,uint h) : NewImageGeneric_aux<T+1,T_Max>(n,w,h)
  {
    if (n == T) {
      m_Instance = new Image_generic<float,T>(w,h);
    }
  }
};

template <int T_Max> class NewImageGeneric : public NewImageGeneric_aux<1,T_Max>
{
public:
  NewImageGeneric(int n,uint w,uint h) : NewImageGeneric_aux<1,T_Max>(n,w,h) { }
};

//---------------------------------------------------------------------------

NAMESPACE::Image *NAMESPACE::ImageFormat_float::load(const char *name) const
{
  FILE *f = NULL;
	fopen_s(&f, name, "rb");
  if (f == NULL) {
    throw Fatal("ImageFormat_float::load - Sorry, cannot open file '%s'",name);
  }
  // read header
  uint  w    = 0;
  uint  h    = 0;
  uchar numc = 0;
  size_t sz;
  sz = fread(&w   ,sizeof(uint) ,1,f);
  sz = fread(&h   ,sizeof(uint) ,1,f);
  sz = fread(&numc,sizeof(uchar),1,f);
  sl_assert(w > 0 && h > 0 && numc > 0);
  Image *img = NULL;
  /*
  switch (numc) {
   case 1: img = new Image_generic<float,1>(w,h); break;
   case 2: img = new Image_generic<float,2>(w,h); break;
   case 3: img = new Image_generic<float,3>(w,h); break;
   case 4: img = new Image_generic<float,4>(w,h); break;
  }*/
  sl_assert(numc < 64);
  NewImageGeneric<64> gen(numc,w,h);
  img = gen.getInstance();
  sl_assert( img != NULL );
  sl_assert( img->w() == w );
  sl_assert( img->h() == h );
  // read data
  sz = fread(img->raw(),w*h*numc*sizeof(float),1,f);
  // done
  fclose(f);
  return (img);
}

//---------------------------------------------------------------------------

void NAMESPACE::ImageFormat_float::save(const char *name,const NAMESPACE::Image *img) const
{
  FILE *f = NULL;
	fopen_s(&f, name, "wb");
  if (f == NULL) {
    throw Fatal("ImageFormat_float::save - Sorry, cannot open file '%s'",name);
  }
  // save header
  uint  w    = img->w();
  uint  h    = img->h();
  uchar numc = img->numComp();
  fwrite(&w   ,sizeof(uint) ,1,f);
  fwrite(&h   ,sizeof(uint) ,1,f);
  fwrite(&numc,sizeof(uchar),1,f);
  // save data
  fwrite(img->raw(),w*h*numc*sizeof(float),1,f);
  // done
  fclose(f);
}

//---------------------------------------------------------------------------
