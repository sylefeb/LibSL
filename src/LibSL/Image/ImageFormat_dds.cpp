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

#include "ImageFormat_dds.h"
using namespace LibSL::Image;

#include <LibSL/Errors/Errors.h>
using namespace LibSL::Errors;
#include <LibSL/Memory/Array.h>
using namespace LibSL::Memory::Array;
#include <LibSL/Memory/Pointer.h>
using namespace LibSL::Memory::Pointer;
#include <LibSL/Math/Tuple.h>
using namespace LibSL::Math;
#include <LibSL/D3DHelpers/D3DHelpers.h>

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
  NAMESPACE::ImageFormat_dds s_DDS;  /// FIXME: this mechanism does not work with VC++
}                                    ///        see also ImageFormatManager constructor

//---------------------------------------------------------------------------

NAMESPACE::ImageFormat_dds::ImageFormat_dds()
{
  try {
    // register plugin
    IMAGE_FORMAT_MANAGER.registerPlugin(this);
  } catch (LibSL::Errors::Fatal& e) {
    std::cerr << e.message() << std::endl;
  }
}

//---------------------------------------------------------------------------

NAMESPACE::Image *NAMESPACE::ImageFormat_dds::load(const char *name) const
{
#ifdef DIRECT3D
  // create sysmem surface
  LPDIRECT3DDEVICE9  d3d    = LIBSL_D3D_DEVICE;
  LPDIRECT3DTEXTURE9 tmp    = NULL;
  LPDIRECT3DSURFACE9 tmpsrf = NULL;
  
  D3DXIMAGE_INFO info;
  LIBSL_D3D_CHECK_ERROR( D3DXGetImageInfoFromFileA(name,&info) );

  LIBSL_D3D_CHECK_ERROR( d3d->CreateTexture(
    info.Width,
    info.Height,
    1,0,
    D3DFMT_A8R8G8B8,
    D3DPOOL_SYSTEMMEM,
    &tmp,NULL));
  LIBSL_D3D_CHECK_ERROR( tmp->GetSurfaceLevel(0,&tmpsrf) );

  LIBSL_D3D_CHECK_ERROR( D3DXLoadSurfaceFromFileA(
    tmpsrf,
    NULL,
    NULL,
    name,
    NULL,
    D3DX_DEFAULT,
    0,
    NULL) );

  // read image data
  ImageRGBA *img = new ImageRGBA(info.Width,info.Height);
  D3DLOCKED_RECT rect;
  tmp->LockRect(0,&rect,NULL,D3DLOCK_READONLY);
  uint   stride     = rect.Pitch;
  const uchar *data = (const uchar *)rect.pBits;
  ForImage(img,i,j) {
    ForIndex(c,3) {
      img->pixel(i,j)[c] = data[i*4 + j*stride + 2 - c];
    }
    img->pixel(i,j)[3] = data[i*4 + j*stride + 3];
  }
  tmp->UnlockRect(0);
  LIBSL_D3D_SAFE_RELEASE(tmpsrf);
  LIBSL_D3D_SAFE_RELEASE(tmp);

  return (img);
#else
  sl_assert(false);
  return NULL;
#endif
}

//---------------------------------------------------------------------------

void NAMESPACE::ImageFormat_dds::save(const char *name,const NAMESPACE::Image *img) const
{
  throw Fatal("ImageFormat_dds::save - Sorry, not yet implemented");
}

//---------------------------------------------------------------------------
