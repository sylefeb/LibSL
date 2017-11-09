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
// LibSL::GPUHelpers::Font
// ------------------------------------------------------
//
// Font class
// Code from libTexture library - under GNU GPL Licence
// 
//    Use this class to load a font and print some text on screen.
//    Fonts are images with all characters on a line. There must be at least on pixel of
//    pure black between two characters. 
//    <BR>&nbsp;<BR>
//    The character set is<BR>
//    !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~<BR>&nbsp;<BR>
//    Note that a default font is provided in libtexture/font/default.tga (this font as a low resolution 
//    to have a small memory cost)
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2001-07-20
// ------------------------------------------------------

#pragma once

#include <LibSL/LibSL.common.h>

// ------------------------------------------------------

#include <LibSL/GPUTex/GPUTex.h>
#include <LibSL/GPUHelpers/GPUHelpers.h>
#include <LibSL/Image/Image.h>
#include <LibSL/GPUHelpers/Shapes.h>
#include <LibSL/Errors/Errors.h>
#include <LibSL/System/Types.h>
#include <LibSL/Math/Tuple.h>
#include <LibSL/Math/Matrix4x4.h>

// ------------------------------------------------------

#define LIBSL_FONT_FIRST_CHAR    33
#define LIBSL_FONT_LAST_CHAR     126
#define LIBSL_FONT_NUM_CHAR      (LIBSL_FONT_LAST_CHAR - LIBSL_FONT_FIRST_CHAR + 1)
#define LIBSL_FONT_TABLE_SIZE    16 // power of two

// ------------------------------------------------------

namespace LibSL {
  namespace GPUHelpers {

    class LIBSL_DLL Font
    {
    private:

      static LibSL::Memory::Array::Array<char>                           s_Buf;
      LibSL::Memory::Pointer::AutoPtr<LibSL::GPUHelpers::Shapes::Square> m_Square;
      LibSL::GPUTex::Tex2DRGBA_Ptr                                       m_Tex;

      float 	      m_CharSizeX [256];
      float	        m_CharBeginX[256];
      float 	      m_CharEndX  [256];
      float	        m_CharMinY;
      float	        m_CharMaxY;
      float 	      m_MaxCharW;

	    void	        computeWidth   (LibSL::Image::ImageRGBA_Ptr img);
      void          computeHeight  (LibSL::Image::ImageRGBA_Ptr img);
      LibSL::Image::ImageRGBA_Ptr createFontTable(LibSL::Image::ImageRGBA_Ptr img);
      void	        copyChar(int chr,uint n,LibSL::Image::ImageRGBA_Ptr src,LibSL::Image::ImageRGBA_Ptr dst);
      void          copyToBuffer(const char *s);
      void          renderChar(uchar chr,float sz,LibSL::Math::m4x4f& m);

    public:

      /**
      Creates the default font
      */
      Font(bool alpha = true);
      /**
      Creates a font from an image file. If alpha is <B>true</B> the black background is replaced by transparancy.
      */
      Font(const char *,bool alpha=true);
      ~Font();

      /**
      Prints the string s at (x,y) with size t.
      Note that the text is printed in a plane but that nothing prevents using an appropriate modelview
      to draw it in 3d.
      */
      void	printString(float x,float y,float t,const char *s,uint clr = 0xFFFFFFFF);

      /**
      Prints N first characters of string s at (x,y) with size t.
      */
      void	printStringN(float x,float y,float t,const char *s,int n,uint clr);

      /**
      Retrieve the width and height needed by printString to print string s with size t.
      */
      void	printStringNeed(float t,const char *s,float *w,float *h);

    };

  } //namespace LibSL::GPUHelpers
} //namespace LibSL

//------------------------------------------------------------------------
