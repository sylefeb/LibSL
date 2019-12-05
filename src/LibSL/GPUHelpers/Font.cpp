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
/*
*
*
* ******************** IN DEVELOPMENT !!! ***************
* TODO:
*  - DX10 support
*
*/

// #include "LibSL.precompiled.h"

// ------------------------------------------------------

#include "Font.h"
#include <LibSL/LibSL.h>

#include <cstring>

using namespace LibSL::GPUTex;

#if defined(EMSCRIPTEN) | defined(ANDROID)
#define OPENGLES
#endif

// ------------------------------------------------------

#define NAMESPACE LibSL::GPUHelpers::Font

// ------------------------------------------------------

#define HAS_A_HOLE(c) ((c) == '"')

// ------------------------------------------------------

Array<char> NAMESPACE::s_Buf;

// ------------------------------------------------------

#include "basefont.h"

// ------------------------------------------------------

NAMESPACE::Font(bool alpha)
{
  ImageRGBA_Ptr img(new ImageRGBA(width, height));
  const char *data = header_data;
  ForImage(img, i, j) {
    v3b pixel;
    HEADER_PIXEL(data,pixel);
    img->pixel(i, j)[0] = pixel[0];
    img->pixel(i, j)[1] = pixel[1];
    img->pixel(i, j)[2] = pixel[2];
    img->pixel(i, j)[3] = pixel.oneGt(0) ? 255 : 0;
  }
  // init
  m_CharMinY = 0;
  m_CharMaxY = 0;
  m_CharMinY = 0;
  m_CharMaxY = 0;
  m_MaxCharW = 0;
  ForIndex(i, 256) {
    m_CharSizeX[i] = 0;
    m_CharBeginX[i] = 0;
    m_CharEndX[i] = 0;
  }
  // find characters
  computeHeight(img);
  computeWidth(img);
  // create a square texture
  ImageRGBA_Ptr tbl = createFontTable(img);
  // space character is a special case
  m_CharSizeX[(uchar)' '] = 0.5f*m_MaxCharW;
  // create texture
  m_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(tbl->pixels(), GPUTEX_AUTOGEN_MIPMAP));
  // create square
  m_Square = AutoPtr<Shapes::Square>(new Shapes::Square());
  glBindTexture(GL_TEXTURE_2D, m_Tex->handle());
}

// ------------------------------------------------------

NAMESPACE::Font(const char *n,bool alpha)
{
  ImageRGBA_Ptr img = ImageRGBA_Ptr(loadImage<ImageRGB>(n)->cast<ImageRGBA>());
  ForImage(img,i,j) {
    img->pixel(i,j)[3] = v3f(img->pixel(i,j)).oneGt(0) ? 255 : 0;
  }
  // init
  m_CharMinY        = 0;
  m_CharMaxY        = 0;
  m_CharMinY        = 0;
  m_CharMaxY        = 0;
  m_MaxCharW        = 0;
  ForIndex(i,256) {
    m_CharSizeX [i] = 0;
    m_CharBeginX[i] = 0;
    m_CharEndX  [i] = 0;
  }
  // find characters
  computeHeight(img);
  computeWidth (img);
  // create a square texture
  ImageRGBA_Ptr tbl = createFontTable(img);
  // space character is a special case
  m_CharSizeX[(uchar)' ']  = 0.5f*m_MaxCharW;
  // create texture
  m_Tex = Tex2DRGBA_Ptr(new Tex2DRGBA(tbl->pixels(), GPUTEX_AUTOGEN_MIPMAP));
  // create square
  m_Square = AutoPtr<Shapes::Square>(new Shapes::Square());
}

// ------------------------------------------------------

NAMESPACE::~Font()
{
  m_Tex = Tex2DRGBA_Ptr();
  m_Square = AutoPtr<Shapes::Square>();
}

// ------------------------------------------------------

void NAMESPACE::computeWidth(ImageRGBA_Ptr img)
{
  LIBSL_BEGIN;
  m_MaxCharW       = 0;
  uchar chr        = LIBSL_FONT_FIRST_CHAR;
  bool  hole       = HAS_A_HOLE(chr);
  bool  hole_ok    = false;
  bool  in         = false;
  bool  nothing    = false;
  ForIndex(x,img->w()) {
    nothing = true;
    ForIndex(y,img->h()) {
      if ( img->pixel(x,y).oneGt(0) ) {
        if (!in) {
          in = true;
          if (!(hole && hole_ok)) {
            m_CharBeginX[chr] = float(x-1);
          }
        }
        nothing = false;
        break;
      }
    }
    if (in && nothing) {
      in = false;
      if (hole && !hole_ok) {
        hole_ok = true;
      } else {
        m_CharEndX [chr] = float(x);
        m_CharSizeX[chr] = m_CharEndX[chr] - m_CharBeginX[chr] + 1;
        if (m_CharSizeX[chr] > m_MaxCharW) {
          m_MaxCharW = floor(m_CharSizeX[chr]);
        }
        chr ++;
        if (HAS_A_HOLE(chr)) {
          hole    = true;
          hole_ok = false;
        } else {
          hole = false;
        }
        if (chr > 255) {
          throw Fatal("Font: Too many characters in font !");
        }
      }
    }
  }
  if (chr - LIBSL_FONT_FIRST_CHAR < LIBSL_FONT_NUM_CHAR) {
    throw Fatal("Not enough characters in font (%d/%d) !",chr - LIBSL_FONT_FIRST_CHAR,LIBSL_FONT_NUM_CHAR);
  }
  if (chr - LIBSL_FONT_FIRST_CHAR > LIBSL_FONT_NUM_CHAR) {
    throw Fatal("Too many characters in font (%d/%d) !",chr - LIBSL_FONT_FIRST_CHAR,LIBSL_FONT_NUM_CHAR);
  }
  LIBSL_END;
}

// ------------------------------------------------------

void NAMESPACE::computeHeight(ImageRGBA_Ptr img)
{
  LIBSL_BEGIN;
  // compute min and max height of characters
  m_CharMinY = float(img->h())-1;
  m_CharMaxY = 0;
  ForIndex (x,img->w()) {
    ForIndex (y,img->h()) {
      if ( img->pixel(x,y).oneGt(0)) {
        m_CharMinY = min(m_CharMinY,float(y));
        m_CharMaxY = max(m_CharMaxY,float(y));
      }
    }
  }
  LIBSL_END;
}

// ------------------------------------------------------

ImageRGBA_Ptr NAMESPACE::createFontTable(ImageRGBA_Ptr img)
{
  LIBSL_BEGIN;
  uint          n;
  ImageRGBA_Ptr tbl;

  // grid of LIBSL_FONT_TABLE_SIZE^2 characters
  uint r = uint(max(m_MaxCharW, (m_CharMaxY - m_CharMinY + 1)) * LIBSL_FONT_TABLE_SIZE);
  // round to superior power of two (SL: for platforms that do not handle non pow. of 2 textures)
  n = 1;
  while (n < r) { n = n << 1; }
  tbl   = ImageRGBA_Ptr(new ImageRGBA(n,n));
  // init at 0
  tbl->pixels().fill(0);
  // copy characters
  ForRange(chr,LIBSL_FONT_FIRST_CHAR,LIBSL_FONT_LAST_CHAR) {
    copyChar(chr,n,img,tbl);
  }
//  saveImage("fonttbl.png",tbl->cast<ImageRGB>());
#if 0
  // outline chars for debugging
  int szchar = (n / LIBSL_FONT_TABLE_SIZE);
  ForImage(tbl, i, j) {
    int p = i / szchar;
    int q = j / szchar;
    tbl->pixel(i, j) = ((p + q) & 1) ? (v4b(255) - tbl->pixel(i, j)) : tbl->pixel(i, j);
  }
#endif
  // convert sizes in texture space
  ForRange(chr,LIBSL_FONT_FIRST_CHAR,LIBSL_FONT_LAST_CHAR) {
    m_CharSizeX[chr] = m_CharSizeX[chr] / float(n);
  }  
  m_CharMinY = float(m_CharMinY) / float(n);
  m_CharMaxY = float(m_CharMaxY) / float(n);
  m_MaxCharW = float(m_MaxCharW) / float(n);
  return (tbl);
  LIBSL_END;
}

// ------------------------------------------------------

void NAMESPACE::copyChar(
  int           chr,
  uint          n,
  ImageRGBA_Ptr src,
  ImageRGBA_Ptr dst)
{
  int c = chr - LIBSL_FONT_FIRST_CHAR;
  sl_assert(c >= 0);
  int szchar = (n / LIBSL_FONT_TABLE_SIZE);
  int u = (c % LIBSL_FONT_TABLE_SIZE) * szchar;
  int v = (c / LIBSL_FONT_TABLE_SIZE) * szchar;
  ForRange(j,m_CharMinY,m_CharMaxY) {
    int kj = j - int(m_CharMinY);
    ForRange(i,m_CharBeginX[chr],m_CharEndX[chr]) {
      int ki = i - int(m_CharBeginX[chr]);
      dst->pixel(u+ki,v+kj) = src->pixel(i,j);
    }
  }
}

// ------------------------------------------------------

void NAMESPACE::renderChar(uchar chr,float sz,m4x4f& m)
{
  if (chr == ' ') {
    m = translationMatrix(V3F(0.5f*sz,0,0)) * m;
  } else {   
    float wdth = m_CharSizeX[chr] / m_MaxCharW;
    uchar c = chr - LIBSL_FONT_FIRST_CHAR;
    float u = (float(c % LIBSL_FONT_TABLE_SIZE)) / float(LIBSL_FONT_TABLE_SIZE);
    float v = (float(c / LIBSL_FONT_TABLE_SIZE)) / float(LIBSL_FONT_TABLE_SIZE);
    // position character
    m4x4f mtmp = m * scaleMatrix(V3F(wdth*sz*0.5f,sz*0.5f,1)) * translationMatrix(V3F(1,1,0));
    // texture transform
    m4x4f mtex = translationMatrix(V3F(u, v + m_CharMinY, 0)) * scaleMatrix(V3F(m_CharSizeX[chr], (m_CharMaxY - m_CharMinY), 1));
    // render
    GPUHelpers::Transform::set(LIBSL_TEXTURE_MATRIX  ,mtex);
    GPUHelpers::Transform::set(LIBSL_MODELVIEW_MATRIX, mtmp);
    m_Square->render();
    // next character on line
    m = translationMatrix(V3F(wdth*sz,0,0)) * m;
  }
}

// ------------------------------------------------------

void NAMESPACE::printString(float x,float y,float t,const char *s,uint clr)
{
  printStringN(x,y,t,s,-1,clr);
}

// ------------------------------------------------------

void NAMESPACE::copyToBuffer(const char *s)
{
  uint l = uint(strlen(s)+1);

  if (s_Buf.size() < l) {
    s_Buf.erase();
    s_Buf.allocate(l);
  }
#if defined(_WIN32) || defined(_WIN64)
  strcpy_s(s_Buf.raw(),s_Buf.size(),s);
#else
  strncpy(s_Buf.raw(),s,s_Buf.size());
#endif
}

// ------------------------------------------------------

void NAMESPACE::printStringN(float x,float y,float t,const char *s,int num,uint clr)
{
  char        *subs;
  uint	       l,ntoprint,nprinted=0;

  copyToBuffer(s);

#ifdef OPENGL
  LIBSL_GL_CHECK_ERROR;
  glBindTexture(GL_TEXTURE_2D, m_Tex->handle());
#ifdef OPENGLES
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->bindTextureUnit(0);
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->enableTexture();
#else
  GLProtectMatrices glmx;
  glPushAttrib(GL_ENABLE_BIT);
  glDisable    (GL_LIGHTING);
  glEnable     (GL_TEXTURE_2D);
#endif
#endif

#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetRenderState      (D3DRS_LIGHTING,FALSE);
  LIBSL_D3D_DEVICE->SetTexture          (0,m_Tex->handle());
  LIBSL_D3D_DEVICE->SetSamplerState     (0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
  LIBSL_D3D_DEVICE->SetSamplerState     (0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
  LIBSL_D3D_DEVICE->SetSamplerState     (0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0); 
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_PROJECTED /*must be there to work ... suspicious*/ | D3DTTFF_COUNT4);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
  LIBSL_D3D_DEVICE->SetRenderState      (D3DRS_TEXTUREFACTOR,clr);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
#endif

  m4x4f m = translationMatrix(V3F(x,y,0));
  int line = 0;
  char *next = NULL;
#if defined(_WIN32) || defined(_WIN64)
  subs       = strtok_s(s_Buf.raw(),"\n",&next);
#else
  subs       = strtok(s_Buf.raw(),"\n");
#endif
  if (subs != NULL) {
    do {
      l = uint(strlen(subs));
      if (num >= 0) {
        if (l+nprinted > uint(num))
          ntoprint = num - nprinted;
      } else {
        ntoprint = l;
      }
      ForIndex(n,ntoprint) {
        renderChar(subs[n],t,m);
      }
      nprinted += ntoprint;
      if (num >= 0 && int(nprinted) >= num) {
        break;
      }
      line++;
      m = translationMatrix(V3F(x, y - t * line, 0));
#if defined(_WIN32) || defined(_WIN64)
    } while ((subs=strtok_s(NULL,"\n",&next)) != NULL);
#else
    } while ((subs=strtok(NULL,"\n")) != NULL);
#endif
  }

#ifdef OPENGL
#ifdef OPENGLES
  LibSL::GLHelpers::GLBasicPipeline::getUniqueInstance()->disableTexture();
  // restore tex matrix
  m4x4f mtex;
  mtex.eqIdentity();
  GPUHelpers::Transform::set(LIBSL_TEXTURE_MATRIX, mtex);
#else
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopAttrib();
  LIBSL_GL_CHECK_ERROR;
#endif
#endif

#ifdef DIRECT3D
  LIBSL_D3D_DEVICE->SetTexture(0,NULL);
  LIBSL_D3D_DEVICE->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
  // restore tex matrix
  m4x4f mtex;
  mtex.eqIdentity();
  GPUHelpers::Transform::set(LIBSL_TEXTURE_MATRIX  ,mtex);
#endif

}

// ------------------------------------------------------

void NAMESPACE::printStringNeed(float t ,const char *s,float *w,float *h)
{
  char  *subs;
  uint   l;
  float  wtmp;

  copyToBuffer(s);
  *h         = 0.0;
  *w         = 0.0;
  char *next = NULL;
#if defined(_WIN32) || defined(_WIN64)
  subs       = strtok_s(s_Buf.raw(),"\n",&next);
#else
  subs       = strtok(s_Buf.raw(),"\n");
#endif
  if (subs != NULL) {
    do {
      *h  += t;
      l    = uint(strlen(subs));
      wtmp = 0.0;
      ForIndex (i,l) {
        wtmp += t * m_CharSizeX[(uchar)subs[i]] / m_MaxCharW;
      }
      if (wtmp > *w) {
        *w = wtmp;
      }
    }
#if defined(_WIN32) || defined(_WIN64)
    while ((subs=strtok_s(NULL,"\n",&next)) != NULL);
#else
    while ((subs=strtok(NULL,"\n")) != NULL);
#endif
  }
}

// ------------------------------------------------------
