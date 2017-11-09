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

Code partially taken from MSDN http://support.microsoft.com/kb/q243285/
Article ID : 243285
Revision   : 1.0

*/

// -----------------------------------------------------
#include "LibSL.precompiled.h"
// -----------------------------------------------------

#include "TrueType.h"
#include "Win32Helpers.h"

#include <LibSL/CppHelpers/CppHelpers.h>
using namespace LibSL::CppHelpers;

#include <windows.h>
#include <wingdi.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <list>

using namespace std;

#include <float.h>
#include <assert.h>

#include <d3dx9.h>

// -----------------------------------------------------

#define NAMESPACE LibSL::Win32Helpers::TrueType

// -----------------------------------------------------

static map<string,HFONT> s_Fonts;
static vector<string>    s_FontNames;
static HFONT             s_CurrentFont =  NULL;
static float             s_fFontMinX   =  1e10f;
static float             s_fFontMaxX   = -1e10f;
static float             s_fFontMinY   =  1e10f;
static float             s_fFontMaxY   = -1e10f;

/****************************************************************************
*  FUNCTION   : IntFromFixed
*  RETURNS    : int value approximating the FIXED value.
****************************************************************************/
static float   FloatFromFixed(FIXED fx)
{
  return (float(fx.value + fx.fract/(float)(1 << 16)));
}

/****************************************************************************
*  FUNCTION   : fxDiv2
*  RETURNS    : (val1 + val2)/2 for FIXED values
****************************************************************************/
static FIXED   fxDiv2(FIXED fxVal1, FIXED fxVal2)
{
  long l;

  l = (*((long far *)&(fxVal1)) + *((long far *)&(fxVal2)))/2;
  return(*(FIXED *)&l);
}

/****************************************************************************
*  FUNCTION   : MakeBezierFromLine
*
*  PURPOSE    : Converts a line define by two points to a four point Bezier
*               spline representation of the line in pPts.
*
*
*  RETURNS    : number of Bezier points placed into the pPts POINT array.
****************************************************************************/
static UINT MakeBezierFromLine( D3DXVECTOR2 *pPts, D3DXVECTOR2 startpt, D3DXVECTOR2 endpt )
{
  UINT cTotal = 0;

  // starting point of Bezier
  pPts[cTotal] = startpt;
  cTotal++;

  // 1rst Control, pt == endpoint makes Bezier a line
  pPts[cTotal].x = endpt.x;
  pPts[cTotal].y = endpt.y;
  cTotal++;

  // 2nd Control, pt == startpoint makes Bezier a line
  pPts[cTotal].x = startpt.x;
  pPts[cTotal].y = startpt.y;
  cTotal++;

  // ending point of Bezier
  pPts[cTotal] = endpt;
  cTotal++;

  return cTotal;
}

/****************************************************************************
*  FUNCTION   : MakeBezierFromQBSpline
*
*  PURPOSE    : Converts a quadratic spline in pSline to a four point Bezier
*               spline in pPts.
*
*
*  RETURNS    : number of Bezier points placed into the pPts D3DXVECTOR2 array.
****************************************************************************/
static UINT MakeBezierFromQBSpline( D3DXVECTOR2 *pPts, POINTFX *pSpline )
{
  D3DXVECTOR2   P0,         // Quadratic on curve start point
    P1,         // Quadratic control point
    P2;         // Quadratic on curve end point
  UINT    cTotal = 0;

  // Convert the Quadratic points to integer
  P0.x = FloatFromFixed( pSpline[0].x );
  P0.y = FloatFromFixed( pSpline[0].y );
  P1.x = FloatFromFixed( pSpline[1].x );
  P1.y = FloatFromFixed( pSpline[1].y );
  P2.x = FloatFromFixed( pSpline[2].x );
  P2.y = FloatFromFixed( pSpline[2].y );

  // conversion of a quadratic to a cubic

  // Cubic P0 is the on curve start point
  pPts[cTotal] = P0;
  cTotal++;

  // Cubic P1 in terms of Quadratic P0 and P1
  pPts[cTotal].x = P0.x + 2*(P1.x - P0.x)/3;
  pPts[cTotal].y = P0.y + 2*(P1.y - P0.y)/3;
  cTotal++;

  // Cubic P2 in terms of Qudartic P1 and P2
  pPts[cTotal].x = P1.x + 1*(P2.x - P1.x)/3;
  pPts[cTotal].y = P1.y + 1*(P2.y - P1.y)/3;
  cTotal++;

  // Cubic P3 is the on curve end point
  pPts[cTotal] = P2;
  cTotal++;

  return cTotal;
}


/****************************************************************************
*  FUNCTION   : AppendPolyLineToBezier
*
*  PURPOSE    : Converts line segments into their Bezier point
*               representation and appends them to a list of Bezier points.
*
*               WARNING - The array must have at least one valid
*               start point prior to the address of the element passed.
*
*  RETURNS    : number of Bezier points added to the D3DXVECTOR2 array.
****************************************************************************/
static UINT AppendPolyLineToBezier( D3DXVECTOR2 *pt, POINTFX start, LPTTPOLYCURVE lpCurve )
{
  int     i;
  UINT    cTotal = 0;
  D3DXVECTOR2   endpt;
  D3DXVECTOR2   startpt;
  D3DXVECTOR2   bezier[4];

  endpt.x = FloatFromFixed(start.x);
  endpt.y = FloatFromFixed(start.y);

  for (i = 0; i < lpCurve->cpfx; i++)
  {
    // define the line segment
    startpt = endpt;
    endpt.x = FloatFromFixed(lpCurve->apfx[i].x);
    endpt.y = FloatFromFixed(lpCurve->apfx[i].y);

    // convert a line to a bezier representation
    MakeBezierFromLine( bezier, startpt, endpt );

    // append the Bezier to the existing ones
    // Point 0 is Point 3 of previous.
    pt[cTotal++] = bezier[1];   // Point 1
    pt[cTotal++] = bezier[2];   // Point 2
    pt[cTotal++] = bezier[3];   // Point 3

  }

  return cTotal;
}


/****************************************************************************
*  FUNCTION   : AppendQuadBSplineToBezier
*
*  PURPOSE    : Converts Quadratic spline segments into their Bezier point
*               representation and appends them to a list of Bezier points.
*
*               WARNING - The array must have at least one valid
*               start point prior to the address of the element passed.
*
*  RETURNS    : number of Bezier points added to the D3DXVECTOR2 array.
****************************************************************************/
static UINT AppendQuadBSplineToBezier( D3DXVECTOR2 *pt, POINTFX start, LPTTPOLYCURVE lpCurve )
{
  WORD                i;
  UINT                cTotal = 0;
  POINTFX             spline[3];  // a Quadratic is defined by 3 points
  D3DXVECTOR2         bezier[4];  // a Cubic by 4

  // The initial A point is on the curve.
  spline[0] = start;

  for (i = 0; i < lpCurve->cpfx;)
  {
    // The B point.
    spline[1] = lpCurve->apfx[i++];

    // Calculate the C point.
    if (i == (lpCurve->cpfx - 1))
    {
      // The last C point is described explicitly
      // i.e. it is on the curve.
      spline[2] = lpCurve->apfx[i++];
    }
    else
    {
      // C is midpoint between B and next B point
      // because that is the on curve point of
      // a Quadratic B-Spline.
      spline[2].x = fxDiv2(
        lpCurve->apfx[i-1].x,
        lpCurve->apfx[i].x
        );
      spline[2].y = fxDiv2(
        lpCurve->apfx[i-1].y,
        lpCurve->apfx[i].y
        );
    }

    // convert the Q Spline to a Bezier
    MakeBezierFromQBSpline( bezier, spline );

    // append the Bezier to the existing ones
    // Point 0 is Point 3 of previous.
    pt[cTotal++] = bezier[1];   // Point 1
    pt[cTotal++] = bezier[2];   // Point 2
    pt[cTotal++] = bezier[3];   // Point 3

    // New A point for next slice of spline is the
    // on curve C point of this B-Spline
    spline[0] = spline[2];
  }

  return cTotal;
}

/****************************************************************************
*  FUNCTION   : CloseContour
*
*  PURPOSE    : Adds a bezier line to close the circuit defined in pt.
*
*
*  RETURNS    : number of points aded to the pt D3DXVECTOR2 array.
****************************************************************************/
static UINT CloseContour( LPD3DXVECTOR2 pt, UINT cTotal )
{
  D3DXVECTOR2               endpt,
    startpt;    // definition of a line
  D3DXVECTOR2               bezier[4];

  // connect the first and last points by a line segment
  startpt = pt[cTotal-1];
  endpt   = pt[0];

  // convert a line to a bezier representation
  MakeBezierFromLine( bezier, startpt, endpt );

  // append the Bezier to the existing ones
  // Point 0 is Point 3 of previous.
  pt[cTotal++] = bezier[1];   // Point 1
  pt[cTotal++] = bezier[2];   // Point 2
  pt[cTotal++] = bezier[3];   // Point 3

  return 3;
}

static void TesselateBezier(int res,D3DXVECTOR2 *pt,int num,vector<v2f>& lines)
{
  int i=0;
  while (i+4 <= num) {
    D3DXVECTOR2 p0((float)pt[i  ].x,(float)pt[i  ].y);
    D3DXVECTOR2 p1((float)pt[i+1].x,(float)pt[i+1].y);
    D3DXVECTOR2 p2((float)pt[i+2].x,(float)pt[i+2].y);
    D3DXVECTOR2 p3((float)pt[i+3].x,(float)pt[i+3].y);
    for (int it=0;it<res;it++) {
      float t   =it/(float)(res-1);
      float invt=1.0f-t;
      D3DXVECTOR2 p=invt*invt*invt*p0 + 3.0f*t*invt*invt*p1 + 3.0f*t*t*invt*p2 + t*t*t*p3;
      lines.push_back(V2F(p.x,p.y));
    }
    i+=3;
  }
}


/****************************************************************************
*  FUNCTION   : DrawT2Outline
*
*  PURPOSE    : Decode the GGO_NATIVE outline, create a sequence of Beziers
*               for each contour, draw with PolyBezier.  Color and relative
*               positioning provided by caller. The coordinates of hDC are
*               assumed to have MM_TEXT orientation.
*
*               The outline data is not scaled. To draw a glyph unhinted
*               the caller should create the font at its EMSquare size
*               and retrieve the outline data. Then setup a mapping mode
*               prior to calling this function.
*
*  RETURNS    : none.
****************************************************************************/

static void GetT2Outline(LPTTPOLYGONHEADER lpHeader, 
                         DWORD size, 
                         uint  resolution,
                         vector<vector<v2f> >& polylines)
{
  WORD                i;
  UINT                cTotal = 0; // Total points in a contour.
  LPTTPOLYGONHEADER   lpStart;    // the start of the buffer
  LPTTPOLYCURVE       lpCurve;    // the current curve of a contour
  D3DXVECTOR2        *pt;         // the bezier buffer
  POINTFX             ptStart;    // The starting point of a curve
  DWORD               dwMaxPts = size/sizeof(POINTFX); // max possible pts.
  DWORD               dwBuffSize;

  polylines.clear();

  dwBuffSize = dwMaxPts *     // Maximum possible # of contour points.
    sizeof(D3DXVECTOR2) * // sizeof buffer element
    3;             // Worst case multiplier of one additional point
  // of line expanding to three points of a bezier

  lpStart = lpHeader;
  pt = (D3DXVECTOR2*)new D3DXVECTOR2[dwMaxPts*3];

  // Loop until we have processed the entire buffer of contours.
  // The buffer may contain one or more contours that begin with
  // a TTPOLYGONHEADER. We have them all when we the end of the buffer.
  while ((DWORD)lpHeader < (DWORD)(((LPSTR)lpStart) + size) && pt != NULL)
  {
    if (lpHeader->dwType == TT_POLYGON_TYPE)
      // Draw each coutour, currently this is the only valid
      // type of contour.
    {
      // Convert the starting point. It is an on curve point.
      // All other points are continuous from the "last"
      // point of the contour. Thus the start point the next
      // bezier is always pt[cTotal-1] - the last point of the
      // previous bezier. See PolyBezier.
      cTotal = 1;
      pt[0].x = FloatFromFixed(lpHeader->pfxStart.x);
      pt[0].y = FloatFromFixed(lpHeader->pfxStart.y);

      // Get to first curve of contour -
      // it starts at the next byte beyond header
      lpCurve = (LPTTPOLYCURVE) (lpHeader + 1);

      // Walk this contour and process each curve( or line ) segment
      // and add it to the Beziers
      while ((DWORD)lpCurve < (DWORD)(((LPSTR)lpHeader) + lpHeader->cb))
      {
        //**********************************************
        // Format assumption:
        //   The bytes immediately preceding a POLYCURVE
        //   structure contain a valid POINTFX.
        //
        //   If this is first curve, this points to the
        //      pfxStart of the POLYGONHEADER.
        //   Otherwise, this points to the last point of
        //      the previous POLYCURVE.
        //
        //   In either case, this is representative of the
        //      previous curve's last point.
        //**********************************************

        ptStart = *(LPPOINTFX)((LPSTR)lpCurve - sizeof(POINTFX));
        if (lpCurve->wType == TT_PRIM_LINE)
        {
          // convert the line segments to Bezier segments
          cTotal += AppendPolyLineToBezier( &pt[cTotal], ptStart, lpCurve );
          i = lpCurve->cpfx;
        }
        else if (lpCurve->wType == TT_PRIM_QSPLINE)
        {
          // Decode each Quadratic B-Spline segment, convert to bezier,
          // and append to the Bezier segments
          cTotal += AppendQuadBSplineToBezier( &pt[cTotal], ptStart, lpCurve );
          i = lpCurve->cpfx;
        }
        else
          // Oops! A POLYCURVE format we don't understand.
          ; // error, error, error

        // Move on to next curve in the contour.
        lpCurve = (LPTTPOLYCURVE)&(lpCurve->apfx[i]);
      }

      // Add points to close the contour.
      // All contours are implied closed by TrueType definition.
      // Depending on the specific font and glyph being used, these
      // may not always be needed.
      if ( pt[cTotal-1].x != pt[0].x || pt[cTotal-1].y != pt[0].y )  {
        cTotal += CloseContour( pt, cTotal );
      }

      polylines.push_back(vector<v2f>());
      TesselateBezier(resolution,pt,cTotal,polylines.back());
      // PolyBezier( hDC, pt, cTotal );
    }
    else
      // Bad, bail, must have a bogus buffer.
      break; // error, error, error

    // Move on to next Contour.
    // Its header starts immediate after this contour
    lpHeader = (LPTTPOLYGONHEADER)(((LPSTR)lpHeader) + lpHeader->cb);
  }

  delete [](pt);
}

// -----------------------------------------------------

static void computeFontBoundaries()
{
  s_fFontMinX= 999999.0f;
  s_fFontMaxX=-999999.0f;
  s_fFontMinY= 999999.0f;
  s_fFontMaxY=-999999.0f;

  for (int l='0';l<'z';l++) {
    vector<vector<v2f> > polylines;
    NAMESPACE::letter2Polyline(l,polylines,4,false);
    for (int p=0;p<int(polylines.size());p++) {
      for (int n=0;n<int(polylines[p].size());n++) {
        s_fFontMinX = min(s_fFontMinX,polylines[p][n][0]);
        s_fFontMaxX = max(s_fFontMaxX,polylines[p][n][0]);
        s_fFontMinY = min(s_fFontMinY,polylines[p][n][1]);
        s_fFontMaxY = max(s_fFontMaxY,polylines[p][n][1]);
      }
    }
  }
}

// -----------------------------------------------------

int NAMESPACE::getNumFonts()
{
  return (int(s_Fonts.size()));
}

// -----------------------------------------------------

const char *NAMESPACE::getFontName(int n)
{
  return (s_FontNames[n].c_str());
}

// -----------------------------------------------------

HFONT getFontByName(const char *name)
{
  return (s_Fonts[string(name)]);
}

// -----------------------------------------------------

void addFont(const LPLOGFONT f)
{
  s_FontNames.push_back(string(toChar(f->lfFaceName)));
  s_Fonts[string(toChar(f->lfFaceName))] = CreateFontIndirect(f);
}

// -----------------------------------------------------

void NAMESPACE::letter2Polyline(
                     unsigned char         letter,
                     vector<vector<v2f> >& polylines,
                     int                   res,
                     bool                  normalize)
{
  HWND hwnd = LibSL::Win32Helpers::mainHWND();
  HDC dc    = GetDC(hwnd);

  //static char fontName[256];
  //GetTextFace(dc,256,fontName);
  //cerr << "Font: " << fontName << endl;

  GLYPHMETRICS gm;
  gm.gmBlackBoxX = 0;
  gm.gmBlackBoxY = 0;
  gm.gmCellIncX  = 0;
  gm.gmCellIncY  = 0;
  gm.gmptGlyphOrigin.x = 0;
  gm.gmptGlyphOrigin.y = 0;
  MAT2 tr;
  tr.eM11.value = 1;
  tr.eM11.fract = 0;
  tr.eM12.value = 0;
  tr.eM12.fract = 0;
  tr.eM21.value = 0;
  tr.eM21.fract = 0;
  tr.eM22.value = 1;
  tr.eM22.fract = 0;
  DWORD sz = GetGlyphOutline(dc, letter, GGO_NATIVE, &gm , 0, NULL, &tr );
  if (sz == GDI_ERROR) {
    cerr << "ERROR: GDI error" << endl;
    return;
  }
  unsigned char *data=new unsigned char[sz];
  GetGlyphOutline(dc, letter, GGO_NATIVE, &gm, sz, data, &tr );
  TTPOLYGONHEADER *glyph_header=(TTPOLYGONHEADER *)data;

  GetT2Outline(glyph_header,sz,res,polylines);

  if (normalize) {
    float letter_min_x=s_fFontMaxX;
    float letter_min_y=s_fFontMaxY;
    for (int p=0;p<int(polylines.size());p++) {
      for (int n=0;n<int(polylines[p].size());n++) {
        letter_min_x = min(letter_min_x,polylines[p][n][0]);
        letter_min_y = min(letter_min_y,polylines[p][n][1]);
      }
    }
    float maxl = Math::max(s_fFontMaxX-s_fFontMinX , s_fFontMaxY-s_fFontMinY);
    for (int p=0;p<int(polylines.size());p++) {
      for (int n=0;n<int(polylines[p].size());n++) {
        polylines[p][n][0] = (polylines[p][n][0] -letter_min_x) / maxl;
        polylines[p][n][1] = (polylines[p][n][1] - s_fFontMinY) / maxl;
      }
    }
  }

  delete [](data);
}

// -----------------------------------------------------

void NAMESPACE::selectFont(const char *name)
{
  HWND hwnd = LibSL::Win32Helpers::mainHWND();
  SelectObject(GetDC(hwnd),getFontByName(name));
  computeFontBoundaries();
}

// -----------------------------------------------------

static BOOL CALLBACK init_callback(LPLOGFONT       lplf, 
                                   LPNEWTEXTMETRIC lpntm, 
                                   DWORD           FontType, 
                                   LPVOID          aFontCount)
{
 if (FontType & TRUETYPE_FONTTYPE) {
   //cerr << "True type font '" << lplf->lfFaceName << '\'' << endl;
   addFont(lplf);
 }
 return TRUE;
}

// -----------------------------------------------------

void NAMESPACE::init()
{
  HWND hwnd = LibSL::Win32Helpers::mainHWND();
  HDC  dc   = GetDC(hwnd);
  EnumFontFamilies(dc, (LPCTSTR) NULL, (FONTENUMPROC) init_callback, 0);
  if (getNumFonts() == 0) {
    cerr << "[TrueType::init] *WARNING* no font found!" << endl;
  }
  selectFont(getFontName(0));
}

// -----------------------------------------------------
