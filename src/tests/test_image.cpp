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

#include <iostream>
using namespace std;

#include <LibSL/LibSL_linalg.h>
#include <LibSL/Image/GradientPaste.h>

// -----------

/// data and functor classes to illustrate ExecuteTypeDependent

class data 
{ 
public: 
  LibSL::Image::Image *image;
};

template <class T> class functor 
{
public: 
  void operator()(data& d) 
  {
    if (dynamic_cast<T *>(d.image) != NULL) {
      std::cerr << "Image has type " << typeid(T).name() << std::endl;
    }
  }
};

// -----------

void test_image()
{

  try {
    Image_Ptr img0=loadImage("doesnotexist.png");
  } catch (LibSL::Errors::Fatal& e) {
    cerr << e.message() << endl;
  }

  {
    Image_Ptr img1;

    img1=loadImage("media/image_test_rgb.png");
    saveImage("test_save_rgb.png",img1);

    img1=loadImage("media/image_test_rgba.png");
    saveImage("test_save_rgba.png",img1);

    img1=loadImage("media/image_test_l8.png");
    saveImage("test_save_l8.png",img1);
  }

  {
    ImageRGB_Ptr img_rgb=loadImage<ImageRGB>("media/image_test_rgb.png");
    ForImage(img_rgb,x,y) {
      img_rgb->pixel(x,y)[0]=((x+y)&1)*255;
    }
    ImageRGB_Ptr sub=img_rgb->extract(16,16,64,64);
    img_rgb->copy(32,32,sub);
    saveImage("test0.png",img_rgb);

    try {
      ImageL8_Ptr img_l8=loadImage<ImageL8>("media/image_test_rgb.png");
      img_l8->pixel(0,0)[0]=255;
    } catch (LibSL::Errors::Fatal& e) {
      cerr << e.message() << endl;
    }

    data d;
    d.image=img_rgb;
    ExecuteOnTypeList<LOKI_TYPELIST_2(ImageRGB,ImageRGBA),functor,data> test(d);

    ImageL8_Ptr img_l8_cast=img_rgb->cast<ImageL8>();
    saveImage("test_cast_l8.png",img_l8_cast);
  }

  // interpolate an image
  {
    ImageRGB_Ptr imgs    = loadImage<ImageRGB>("media/image_test_rgb.png");
    ImageRGB_Ptr imgi    = new ImageRGB(imgs->w()*2,imgs->h()*2);
    ForImage(imgi,x,y) {
      float u = float(x+0.5f)/float(imgi->w());
      float v = float(y+0.5f)/float(imgi->h());
      imgi->pixels().at(x,y) = imgs->bilinear<Clamp>(u,v);
    }
    saveImage("interpolated.png",imgi);
    // test clamping
    ForImage(imgi,x,y) {
      float u = -0.5f+float(x+0.5f)/float(imgs->w());
      float v = -0.5f+float(y+0.5f)/float(imgs->h());
      imgi->pixels().at(x,y) = imgs->bilinear(u,v);
    }
    saveImage("clamp.png",imgi);
    // test wrap
    ForImage(imgi,x,y) {
      float u = -0.5f+float(x+0.5f)/float(imgs->w());
      float v = -0.5f+float(y+0.5f)/float(imgs->h());
      imgi->pixels().at(x,y) = imgs->bilinear<Wrap>(u,v);
    }
    saveImage("wrap.png",imgi);
  }

  // push-pull on an image
  {
    ImageFloat3_Ptr  img = loadImage<ImageRGB>("media/lena_hole.png")->cast<ImageFloat3>();
    ImageBool1_Ptr usage = new ImageBool1(img->w(),img->h());
    usage->pixels().fill(true);
    ForImage(img,x,y) {
      if (img->pixel(x,y) == 0) {
        usage->pixel(x,y) = false;
      }
    }

    ImageFloat3_Ptr result = pushPull<ImageFloat3>(img,usage);
    saveImage("pushpull.png",result->cast<ImageRGB>());
  }

  // image pyramid
  {
    ImageFloat3_Ptr  img = new ImageFloat3(256,256);
    ForImage(img,i,j) {
      img->pixel(i,j)[0] = (i+j)&1 ? 255.0f : 0.0f;
      img->pixel(i,j)[1] = ((i>>1)+(j>>1))&1 ? 255.0f : 0.0f;
      img->pixel(i,j)[2] = ((i>>2)+(j>>2))&1 ? 255.0f : 0.0f;
    }{
      // box
      ImagePyramidFactory<ImageFloat3,BoxFilter> factory;
      ImagePyramid<ImageFloat3>::t_AutoPtr pyr = factory.newPyramid(img->pixels());
      ForIndex(l,pyr->numLevels()) {
        saveImage(sprint("box_pyr%02d.png",l),pyr->level(l)->cast<ImageRGB>());     
      }
    }{
      // gauss
      ImagePyramidFactory<ImageFloat3,GaussianFilter> factory;
      ImagePyramid<ImageFloat3>::t_AutoPtr pyr = factory.newPyramid(img->pixels());
      ForIndex(l,pyr->numLevels()) {
        saveImage(sprint("gauss_pyr%02d.png",l),pyr->level(l)->cast<ImageRGB>());     
      }
    }
  }

  // filter image
  {
    ImageFloat3_Ptr  img = new ImageFloat3(256,256);
    uint sz = 16;
    ForIndex(i,sz) { ForIndex(j,sz) {
      img->pixel(127+i-sz/2,127+j-sz/2) = 255.0f;
    } }
    {
      // box
      ImageFloat3_Ptr filtered = new ImageFloat3();
      SeparableFilter2D<ImageFloat3::t_PixelArray,BoxFilter,11,1> filter;
      filter.filter(img->pixels(),filtered->pixels());
      saveImage("box_filter.png",filtered->cast<ImageRGB>());     
    }{
      // gauss
      ImageFloat3_Ptr filtered = new ImageFloat3();
      SeparableFilter2D<ImageFloat3::t_PixelArray,GaussianFilter,11,1> filter;
      filter.filter(img->pixels(),filtered->pixels());
      saveImage("gauss_filter.png",filtered->cast<ImageRGB>());     
    }
  }
  
}

// -----------
