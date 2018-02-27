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

#include <iostream>
#include <ctime>
#include <cmath>

/* -------------------------------------------------------- */

#include <LibSL/LibSL.h>
#include <LibSL/LibSL_linalg.h>

using namespace std;

LIBSL_WIN32_FIX;

/* -------------------------------------------------------- */

typedef LOKI_TYPELIST_5(ImageRGB,
                        ImageRGBA,
                        ImageFloat1,
                        ImageFloat3,
                        ImageFloat4) AllImageTypes;

/* -------------------------------------------------------- */

uint do_grid(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <N> <M> <spacing> <image0> ... <imageNxM>
  uint  n = atoi(argv[firstarg  ]);
  uint  m = atoi(argv[firstarg+1]);
  uint  s = atoi(argv[firstarg+2]);
  cerr << sprint(" - expecting %dx%d images\n",n,m);
  cerr << sprint(" - spacing set to %d\n",s);
  uint arg = firstarg+3;
  if (argc < arg+n*m) {
    cerr << sprint("ERROR: not enough images on command line\n",n,m);
    return (-1);
  }
  ImageRGB_Ptr result = ImageRGB_Ptr(new ImageRGB(image->w()*n + s*(n - 1), image->h()*m + s*(n - 1)));
  result->pixels().fill(255);
  ForIndex(j,m) {
    ForIndex(i,n) {
      cerr << sprint(" - adding image %s\n",argv[arg]);
      ImageRGB_Ptr img;
      try {
        img = ImageRGB_Ptr(loadImage<ImageRGB>(argv[arg]));
      } catch (...) {
        img = ImageRGB_Ptr(loadImage<ImageRGBA>(argv[arg])->cast<ImageRGB>());
      }
      result->copy(i*img->w()+s*i,j*img->h()+s*j,img);
      if (i+j*n < n*m-1) {
        sl_assert(arg < argc);
      }
      arg ++;
    }
  }
  saveImage(outname,result.raw());
  return (arg);
}

/* -------------------------------------------------------- */

class CropData
{
public:
  LibSL::Image::Image *image;
  uint          x,y,w,h;
  const char   *outname;
};

template <class T> class Crop
{
public:
  void operator()(CropData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      saveImage(d.outname,image->extract(d.x,d.y,d.w,d.h));
    }
  }
};

uint do_crop(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <x> <y> <width> <height>
  sl_assert(firstarg+4 <= argc);
  CropData nfo;
  nfo.image = image.raw();
  nfo.x = atoi(argv[firstarg+0]);
  nfo.y = atoi(argv[firstarg+1]);
  nfo.w = atoi(argv[firstarg+2]);
  nfo.h = atoi(argv[firstarg+3]);
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,Crop,CropData> crop(nfo);
  return (firstarg+4);
}

/* -------------------------------------------------------- */

uint do_alphamult(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  ImageRGBA *rgba = dynamic_cast<ImageRGBA*>( image.raw() );
  sl_assert (rgba != NULL);
  ImageRGBA_Ptr img = ImageRGBA_Ptr(rgba->clone());
  ForImage(rgba,i,j) {
	  img->pixel(i,j)[0] =  ((int)rgba->pixel(i,j)[0] * (int)rgba->pixel(i,j)[3]) / (255);
	  img->pixel(i,j)[1] =  ((int)rgba->pixel(i,j)[1] * (int)rgba->pixel(i,j)[3]) / (255);
	  img->pixel(i,j)[2] =  ((int)rgba->pixel(i,j)[2] * (int)rgba->pixel(i,j)[3]) / (255);
	  img->pixel(i,j)[3] =  rgba->pixel(i,j)[3];
  }
  saveImage(outname,img.raw());
  return (firstarg);
}

/* -------------------------------------------------------- */

uint do_rand(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  sl_assert(firstarg+1 <= argc);
  int n = atoi(argv[firstarg+0]);
  ImageRGBA *rgba = dynamic_cast<ImageRGBA*>( image.raw() );
  sl_assert (rgba != NULL);
  ImageRGBA_Ptr img = ImageRGBA_Ptr(rgba->clone());
  img->pixels().fill(0);
  set<v2i> pixs;
  while ((int)pixs.size() < n) {
    int i = rand()%img->w();
    int j = rand()%img->h();
    pixs.insert( V2I(i,j) );
	  img->pixel(i,j) = 255;
  }
  saveImage(outname, img.raw());
  return (firstarg+1);
}

/* -------------------------------------------------------- */

/*
class ResizeData
{
public:
  LibSL::Image::Image *image;
  uint          w,h;
  const char   *outname;
};

template <class T> class Resize
{
public:
  void operator()(ResizeData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      AutoPtr<T> filtered = new T(*image);
      if (d.w < d.image->w() || d.h < d.image->h()) {
        GaussianFilter<T> filter;
        filter.setSamplingFrequency( float(Math::min(d.w,d.h)) / float(Math::max(d.image->w(),d.image->h())) );
        filtered = filter.filter(filtered);
      }
      AutoPtr<T> resized = new T(d.w,d.h);
      ForImage(resized,i,j) {
        resized->pixel(i,j) = filtered->bilinear((i+0.5f)/float(d.w),(j+0.5f)/float(d.h));
      }
      saveImage(d.outname,resized);
    }
  }
};

uint do_resize(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <width> <height>
  sl_assert(firstarg+2 <= argc);
  ResizeData nfo;
  nfo.image = image;
  nfo.w = atoi(argv[firstarg+0]);
  nfo.h = atoi(argv[firstarg+1]);
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,Resize,ResizeData> rsz(nfo);
  return (firstarg+2);
}
*/

/* -------------------------------------------------------- */

class ClampData
{
public:
  LibSL::Image::Image *image;
  float                vmin;
  float                vmax;
  const char          *outname;
};

template <class T> class Clamp
{
public:
  void operator()(ClampData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      AutoPtr<T> tmp = AutoPtr<T>(new T(*image));
      tmp->clamp(typename T::t_Component(d.vmin),typename T::t_Component(d.vmax));
      saveImage(d.outname, tmp.raw());
    }
  }
};

uint do_clamp(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <min> <max>
  sl_assert(firstarg+2 <= argc);
  ClampData nfo;
  nfo.image = image.raw();
  nfo.vmin    = float(atof(argv[firstarg+0]));
  nfo.vmax    = float(atof(argv[firstarg+1]));
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,::Clamp,ClampData> clp(nfo);
  return (firstarg+2);
}

/* -------------------------------------------------------- */

class GradData
{
public:
  LibSL::Image::Image *image;
  uint                 comp;
  const char          *outname;
};

template <class T> class Grad
{
public:
  void operator()(GradData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      ImageRGB_Ptr tmp = ImageRGB_Ptr(new ImageRGB(image->w(), image->h()));
      tmp->pixels().fill(0);
      ForIndex(j,tmp->h()-1) {
        ForIndex(i,tmp->w()-1) {
          tmp->pixel(i, j)[0] = (uchar)(LibSL::Math::clamp(128.0f + float(image->pixel(i + 1, j + 1)[d.comp]) - float(image->pixel(i, j + 1)[d.comp]), 0.0f, 255.0f));
          tmp->pixel(i, j)[1] = (uchar)(LibSL::Math::clamp(128.0f + float(image->pixel(i + 1, j + 1)[d.comp]) - float(image->pixel(i + 1, j)[d.comp]), 0.0f, 255.0f));
        }
      }
      saveImage(d.outname, tmp.raw());
    }
  }
};

uint do_grad(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <comp>
  sl_assert(firstarg+1 <= argc);
  GradData nfo;
  nfo.image = image.raw();
  nfo.comp    = atoi(argv[firstarg+0]);
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,::Grad,GradData> div(nfo);
  return (firstarg+1);
}

/* -------------------------------------------------------- */

Image_generic<float,3>::t_AutoPtr
	computeDistanceField(const Image_generic<uchar,3> *image,const Tuple<uchar,3>& empty_color)
{
	try {
		Array2D<v2i> mvec(image->w(),image->h());

		ForArray2D(mvec,x,y) {
			bool isundef = false;
			if (image->pixel(x,y) == empty_color) {
				isundef = true;
			}
			int vecy     = isundef ? (1<<15): 0;
			mvec.at(x,y) = V2I(0,vecy);
		}

		computeEuclidianDistanceField(mvec);

    Image_generic<float, 3>::t_AutoPtr res = Image_generic<float, 3>::t_AutoPtr(new Image_generic<float, 3>(image->w(), image->h()));
		ForImage(image,x,y) {
			//sl_assert(mvec.at(x,y)[0]<image->w());
			//sl_assert(mvec.at(x,y)[1]<image->h());
			float l         = length(v2f(mvec.at(x,y)));
			res->pixel(x,y) = l;
		}
		return (res);

	} catch (Fatal& f) {
		LIBSL_BOUNCE_ERROR(f);
	}
}

// ------

Image_generic<float,1>::t_AutoPtr computeSignedDistanceField(const Image_generic<uchar,3> *image)
{
	// compute distance field within white color
	Image_generic<float,3>::t_AutoPtr in_white = computeDistanceField(image,255);
	// compute distance field within black color
	Image_generic<float,3>::t_AutoPtr in_black = computeDistanceField(image,  0);
	// merge
  Image_generic<float, 1>::t_AutoPtr result = Image_generic<float, 1>::t_AutoPtr(new Image_generic<float, 1>(image->w(), image->h()));
	ForImage(result,i,j) {
		float d = 0;
		if (image->pixel(i,j)[0] == 255) {
			d =   in_white->pixel(i,j)[0] - 0.5f;
		} else {
			d = - in_black->pixel(i,j)[0] + 0.5f;
		}
		result->pixel(i,j) = d;
	}
	return (result);
}

// -------

class DistFieldData
{
public:
  LibSL::Image::Image *image;
  const char          *outname;
};

template <class T> class DistField
{
public:
  void operator()(DistFieldData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      ImageFloat1_Ptr dfield = computeSignedDistanceField(image->template cast<ImageRGB>());
      Tuple<float,1> dmin,dmax;
      dfield->findMinMax(dmin,dmax);
      float m = LibSL::Math::max(abs(dmin[0]),abs(dmax[0]));
      ImageFloat3_Ptr field = ImageFloat3_Ptr(new ImageFloat3(image->w(), image->h()));
      ForImage(field,i,j) {
        float d = dfield->pixel(i,j)[0];
        if (d >= 0) {
          field->pixel(i,j) = V3F(0,d * 255.0f / m,0);
        } else {
          field->pixel(i,j) = V3F(0,0,-d * 255.0f / m);
        }
      }
      saveImage(d.outname,field->cast<ImageRGB>());
    }
  }
};

uint do_distfield(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  DistFieldData nfo;
  nfo.image = image.raw();
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,::DistField,DistFieldData> dfield(nfo);
  return (firstarg);
}

/* -------------------------------------------------------- */

class ResizeCanvasData
{
public:
  AutoPtr<LibSL::Image::Image>  image;
  uint                          w,h;
  float                         r,g,b;
  const char                   *outname;
};

template <class T> class ResizeCanvas
{
public:
  void operator()(ResizeCanvasData& d)
  {
    T *image = dynamic_cast<T *>(d.image.raw());
    if (image != NULL) {
      AutoPtr<T> tmp = AutoPtr<T>(new T(d.w, d.h));
      typename T::t_Pixel pix;
      pix[0] = typename T::t_Component(d.r); pix[1] = typename T::t_Component(d.g); pix[2] = typename T::t_Component(d.b);
      tmp->pixels().fill(pix);
      tmp->copy((d.w - image->w()) / 2, (d.h - image->h()) / 2, AutoPtr<T>(d.image));
      saveImage(d.outname, tmp.raw());
    }
  }
};

uint do_resizecanvas(AutoPtr<LibSL::Image::Image> image,
                     uint  firstarg,
                     uint  argc,
                     const char **argv,
                     const char *outname)
{
  // <width> <height> <r> <g> <b>
  sl_assert(firstarg+5 <= argc);
  ResizeCanvasData nfo;
  nfo.image   = image;
  nfo.w       = atoi(argv[firstarg+0]);
  nfo.h       = atoi(argv[firstarg+1]);
  nfo.r       = float(atof(argv[firstarg+2]));
  nfo.g       = float(atof(argv[firstarg+3]));
  nfo.b       = float(atof(argv[firstarg+4]));
  nfo.outname = outname;
  sl_assert(image->w() <= nfo.w && image->h() <= nfo.h);
  ExecuteOnTypeList<AllImageTypes,ResizeCanvas,ResizeCanvasData> rszc(nfo);
  return (firstarg+2);
}

/* -------------------------------------------------------- */

#define NEAREST  0
#define BILINEAR 1

class CloseUpData
{
public:
  LibSL::Image::Image *image;
  uint                 l,t;
  uint                 wz,hz;
  float                zoomfactor;
  uint                 filter;
  const char          *outname;
};

template <class T> class CloseUp
{
public:
  void operator()(CloseUpData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      AutoPtr<T> tmp = AutoPtr<T>(new T(d.wz, d.hz));
      ForImage(tmp,i,j) {
        typename T::t_Pixel clr;
        if (d.filter == BILINEAR) {
          float x = (i+0.5f)/ d.zoomfactor + d.l;
          float y = (j+0.5f)/ d.zoomfactor + d.t;
          v2f p   = V2F(x/float(image->w()),y/float(image->h()));
          clr     = image->bilinear(p[0],p[1]);
        } else {
          int x   = int(i / d.zoomfactor) + d.l;
          int y   = int(j / d.zoomfactor) + d.t;
          clr     = image->pixel(x,y);
        }
        tmp->pixel(i,j) = clr;
      }
      saveImage(d.outname, tmp.raw());
    }
  }
};

uint do_closeup(AutoPtr<LibSL::Image::Image> image,
                uint  firstarg,
                uint  argc,
                const char **argv,
                const char *outname)
{
  // <left> <top> <width> <height> <zoom> <filter>
  sl_assert(firstarg+6 <= argc);
  CloseUpData nfo;
  nfo.image = image.raw();
  nfo.l          = atoi(argv[firstarg+0]);
  nfo.t          = atoi(argv[firstarg+1]);
  uint w         = atoi(argv[firstarg+2]);
  uint h         = atoi(argv[firstarg+3]);
  nfo.zoomfactor = float(atof(argv[firstarg+4]));
  nfo.wz         = uint(w*nfo.zoomfactor);
  nfo.hz         = uint(h*nfo.zoomfactor);
  nfo.outname    = outname;
  if (!strcmp(argv[firstarg+5],"nearest")) {
    nfo.filter     = NEAREST;
  } else if (!strcmp(argv[firstarg+5],"bilinear")) {
    nfo.filter     = BILINEAR;
  } else {
    nfo.filter     = NEAREST;
  }
  ExecuteOnTypeList<AllImageTypes,CloseUp,CloseUpData> rszc(nfo);
  return (firstarg+6);
}

/* -------------------------------------------------------- */

class CompareData
{
public:
  LibSL::Image::Image     *imageA;
  LibSL::Image::Image_Ptr  imageB;
  const char              *outname;
  bool                     done;
};

template <class T> class Compare
{
public:
  void operator()(CompareData& d)
  {
    T *imageA = dynamic_cast<T *>(d.imageA);
    T *imageB = dynamic_cast<T *>(d.imageB.raw());
    if (imageA != NULL && imageB != NULL) {
      d.done = true;
      uint numdiff       = 0;
      AutoPtr<T> diff = AutoPtr<T>(new T(imageA->w(), imageA->h()));
      float mse          = 0.0f;
      ForImage(diff,i,j) {
        typename T::t_Pixel a     = imageA->pixel(i,j);
	typename T::t_Pixel b     = imageB->pixel(i,j);
        Tuple<float,T::e_NumComp> delta;
        //if (d.log2RGB) {
          //delta = tupleLog2(Tuple<float,T::e_NumComp>(a))-tupleLog2(Tuple<float,T::e_NumComp>(b));
        //} else {
        delta = Tuple<float,T::e_NumComp>(a)-Tuple<float,T::e_NumComp>(b);
        //}
        if (length(delta) > 1e-9f) {
          numdiff ++;
        }
        mse             += sqLength(delta);
        diff->pixel(i,j) = typename T::t_Component(length(delta));
      }
      mse        = mse / float(imageA->w()*imageA->h()*imageA->numComp());
      float psnr = float(10.0 * log10(255.0*255.0/mse));
      cout << sprint("MSE = %f, PSNR = %f dB, numdiff %d\n",mse,psnr,numdiff);
      saveImage(d.outname,diff);
    }
  }
};

uint do_compare(AutoPtr<LibSL::Image::Image> image,
                uint  firstarg,
                uint  argc,
                const char **argv,
                const char *outname)
{
  // <imageB>
  sl_assert(firstarg+1 <= argc);
  cerr << sprint("-compare: comparing with '%s'\n",argv[firstarg+0]);
  CompareData nfo;
  nfo.imageA = image.raw();
  nfo.imageB = Image_Ptr(loadImage(argv[firstarg + 0]));
  sl_assert(!nfo.imageB.isNull());
  nfo.outname    = outname;
  nfo.done       = false;
  sl_assert(nfo.imageA->w() == nfo.imageB->w() && nfo.imageA->h() == nfo.imageB->h());
  ExecuteOnTypeList<AllImageTypes,::Compare,CompareData> cmp(nfo);
  if (!nfo.done) {
    throw Fatal("-compare: images are of different types!");
  }
  return (firstarg+2);
}

/* -------------------------------------------------------- */

class PushPullData
{
public:
  LibSL::Image::Image *image;
  const char          *outname;
  float                r,g,b;
};

template <class T> class PushPull
{
public:
  void operator()(PushPullData& d)
  {
    T *image = dynamic_cast<T *>(d.image);
    if (image != NULL) {
      typename T::t_Pixel pix;
      pix[0] = typename T::t_Component(d.r); pix[1] = typename T::t_Component(d.g); pix[2] = typename T::t_Component(d.b);
      ImageBool1_Ptr usage = ImageBool1_Ptr(new ImageBool1(image->w(), image->h()));
      usage->pixels().fill(true);
      ForImage(image,x,y) {
        if (image->pixel(x,y) == pix) {
          usage->pixel(x,y) = false;
        }
      }
      typename Image_generic<float, T::e_NumComp>::t_AutoPtr tmp = typename Image_generic<float, T::e_NumComp>::t_AutoPtr(
        pushPull<Image_generic<float, T::e_NumComp> >(typename Image_generic<float, T::e_NumComp>::t_AutoPtr(image->template cast<Image_generic<float, T::e_NumComp> >()), usage)
        );
      saveImage(d.outname,tmp->template cast<T>());
    }
  }
};

uint do_pushpull(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <empty_r> <empty_g> <empty_b>
  sl_assert(firstarg+3 <= argc);
  PushPullData nfo;
  nfo.image = image.raw();
  nfo.outname = outname;
  nfo.r       = float(atof(argv[firstarg++]));
  nfo.g       = float(atof(argv[firstarg++]));
  nfo.b       = float(atof(argv[firstarg++]));
  ExecuteOnTypeList<AllImageTypes,::PushPull,PushPullData> pushpull(nfo);
  return (firstarg);
}

/* -------------------------------------------------------- */

class HistoMatchData
{
public:
  LibSL::Image::Image *imageA;
  LibSL::Image::Image *imageB;
  const char          *outname;
};

template <class T> class HistoMatch
{
public:
  void operator()(HistoMatchData& d)
  {
    sl_assert(d.imageA != NULL);
    sl_assert(d.imageB != NULL);
    T *imageA = dynamic_cast<T*>(d.imageA);
    T *imageB = dynamic_cast<T*>(d.imageB);
    if (imageB != NULL) {
      sl_assert(imageA != NULL);
      sl_assert(imageB != NULL);
      ImageRGB *rgbA = imageA->template cast<ImageRGB>();
      ImageRGB *rgbB = imageB->template cast<ImageRGB>();
      //ForImage(rgbA,i,j) {
      //  rgbA->pixel(i,j) = v3b(clamp(rgb2xyz(v3f(rgbA->pixel(i,j))),v3f(0.0f),v3f(255.0f)));
      //}
      //ForImage(rgbB,i,j) {
      //  rgbB->pixel(i,j) = v3b(clamp(rgb2xyz(v3f(rgbB->pixel(i,j))),v3f(0.0f),v3f(255.0f)));
      //}
      // compute histograms
      Histogram h_ref[3],h_mod[3];
      ForImage(rgbB,i,j) {
        ForIndex(c,3) {
          h_ref[c] << rgbB->pixel(i,j)[c];
          h_mod[c] << rgbA->pixel(i,j)[c];
        }
      }
      // compute cumulative distributions
      int c_ref[3][256],c_mod[3][256];
      ForIndex(n,3) {
        ForIndex(c,256) {
          c_ref[n][c] = 0;
          c_mod[n][c] = 0;
        }
        c_ref[n][0] = h_ref[n][0];
        c_mod[n][0] = h_mod[n][0];
        ForRange(c,1,255) {
          c_ref[n][c] = c_ref[n][c-1] + h_ref[n][c];
          c_mod[n][c] = c_mod[n][c-1] + h_mod[n][c];
        }
      }
      // match
      ForIndex(n,3) {
        Array  <int>     counts(256);
        map    <int,v2i> ids;
        counts.fill(0);
        ForIndex(v,256) {
          counts[v] = h_mod[n][v];
        }
        LIBSL_TRACE;
        ForImage(rgbA,i,j) {
          int v  = rgbA->pixel(i,j)[n];
          sl_assert(counts[ v ] > 0);
          int id = c_mod[n][ v ] - counts[ v ];
          counts[ v ] --;
          sl_assert(id >= 0 && id < (int)(rgbA->w()*rgbA->h()));
          sl_assert(ids.find(id) == ids.end());
          ids.insert( make_pair(id,V2I(i,j)) );
        }
        ForIndex(c,256) {
          sl_assert(counts[c] == 0);
        }
        int pid  = 0;
        int cur  = 0;
        LIBSL_TRACE;
        ForImage(rgbA,i,j) {
          while (pid >= c_ref[n][cur]) {
            cur ++;
          }
          sl_assert(cur < 256);
          sl_assert(pid < (int)(rgbA->w()*rgbA->h()));
          v2i ij = ids[pid];
          sl_assert(ij[0] < (int)rgbA->w() && ij[1] < (int)rgbA->h());
          rgbA->pixel(ij[0],ij[1])[n] = cur;
          pid ++;
        }
      }
      // verify histogram
      Histogram h_new[3];
      ForImage(rgbB,i,j) {
        ForIndex(c,3) {
          h_new[c] << rgbA->pixel(i,j)[c];
        }
      }
      ForIndex(v,256) {
        cerr << sprint("[%03d]: (R) %3d =?= %3d (G) %3d =?= %3d (B) %3d =?= %3d\n",v,h_new[0][v],h_ref[0][v],h_new[1][v],h_ref[1][v],h_new[2][v],h_ref[2][v]);
        ForIndex(c,3) {
          sl_assert(h_new[c][v] == h_ref[c][v]);
        }
      }
      // save
      //ForImage(rgbA,i,j) {
      //  rgbA->pixel(i,j) = v3b(clamp(xyz2rgb(v3f(rgbA->pixel(i,j))),v3f(0.0f),v3f(255.0f)));
      //}
      saveImage(d.outname,rgbA);
    }
  }
};

uint do_histomatch(AutoPtr<LibSL::Image::Image> image,
             uint  firstarg,
             uint  argc,
             const char **argv,
             const char *outname)
{
  // <imageB>
  sl_assert(firstarg+1 <= argc);
  HistoMatchData nfo;
  nfo.imageA = image.raw();
  nfo.imageB  = loadImage(argv[firstarg+0]);
  sl_assert(nfo.imageB != NULL);
  nfo.outname = outname;
  ExecuteOnTypeList<AllImageTypes,::HistoMatch,HistoMatchData> histomatch(nfo);
  return (firstarg+1);
}

/* -------------------------------------------------------- */

uint do_convert(AutoPtr<LibSL::Image::Image> image,
                uint  firstarg,
                uint  argc,
                const char **argv,
                const char *outname)
{
  saveImage(outname,image);
  return (firstarg);
}

/* -------------------------------------------------------- */

int main(int argc, const char **argv)
{
  try {

    if (argc < 3) {
      cerr << sprint("imageedit <in> <out> <args>\n");
      exit (-1);
    }

    Image_Ptr image = Image_Ptr(loadImage(argv[1]));
    cerr << sprint("Input: %dx%d %d components\n",image->w(),image->h(),image->numComp());

    int arg = 3;
    while (arg<argc) {
      if (!strcmp(argv[arg],"-grid")) {
        arg = do_grid      (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-crop")) {
        arg = do_crop      (image,arg+1,argc,argv,argv[2]);
      //} else if (!strcmp(argv[arg],"-resize")) {
      //  arg = do_resize(image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-convert")) {
        arg = do_convert   (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-resizecanvas")) {
        arg = do_resizecanvas(image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-closeup")) {
        arg = do_closeup   (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-compare")) {
        arg = do_compare   (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-clamp")) {
        arg = do_clamp     (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-grad")) {
        arg = do_grad      (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-pushpull")) {
        arg = do_pushpull  (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-distfield")) {
        arg = do_distfield (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-histomatch")) {
        arg = do_histomatch(image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-alphamult")) {
        arg = do_alphamult (image,arg+1,argc,argv,argv[2]);
      } else if (!strcmp(argv[arg],"-rand")) {
        arg = do_rand (image,arg+1,argc,argv,argv[2]);
      } else {
        arg ++;
      }
    }

  } catch (Fatal& e) {
    cerr << Console::red << e.message() << Console::gray << endl;
    return (-1);
  }

  return (0);
}

/* -------------------------------------------------------- */
