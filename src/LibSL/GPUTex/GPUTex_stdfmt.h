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

// 2D textures

typedef Tex2D<LibSL::Math::Tuple<unsigned char,1> >     Tex2DLum;
typedef Tex2DLum::t_AutoPtr                             Tex2DLum_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned char,2> >     Tex2DRG;
typedef Tex2DLum::t_AutoPtr                             Tex2DRG_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned char,3> >     Tex2DRGB;
typedef Tex2DRGB::t_AutoPtr                             Tex2DRGB_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned char,4> >     Tex2DRGBA;
typedef Tex2DRGBA::t_AutoPtr                            Tex2DRGBA_Ptr;

//typedef Tex2D<LibSL::Math::Tuple<unsigned short,3> >    Tex2DRGB16;
//typedef Tex2DRGB16::t_AutoPtr                           Tex2DRGB16_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned short,4> >    Tex2DRGBA16;
typedef Tex2DRGBA16::t_AutoPtr                          Tex2DRGBA16_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned short,1> >    Tex2DLum16;
typedef Tex2DLum16::t_AutoPtr                           Tex2DLum16_Ptr;
typedef Tex2D<LibSL::Math::Tuple<unsigned short,2> >    Tex2DUV16;
typedef Tex2DUV16::t_AutoPtr                            Tex2DUV16_Ptr;
typedef Tex2D<LibSL::Math::Tuple<short,2> >             Tex2DUV16s;
typedef Tex2DUV16s::t_AutoPtr                           Tex2DUV16s_Ptr;

typedef Tex2D<LibSL::Math::Tuple<half,3> >              Tex2DRGB16F;
typedef Tex2DRGB16F::t_AutoPtr                          Tex2DRGB16F_Ptr;
typedef Tex2D<LibSL::Math::Tuple<half,4> >              Tex2DRGBA16F;
typedef Tex2DRGBA16F::t_AutoPtr                         Tex2DRGBA16F_Ptr;
typedef Tex2D<LibSL::Math::Tuple<half,2> >              Tex2DUV16F;
typedef Tex2DUV16F::t_AutoPtr                           Tex2DUV16F_Ptr;
typedef Tex2D<LibSL::Math::Tuple<half,1> >              Tex2DLum16F;
typedef Tex2DLum16F::t_AutoPtr                          Tex2DLum16F_Ptr;

typedef Tex2D<LibSL::Math::Tuple<float,3> >             Tex2DRGB32F;
typedef Tex2DRGB32F::t_AutoPtr                          Tex2DRGB32F_Ptr;
typedef Tex2D<LibSL::Math::Tuple<float,4> >             Tex2DRGBA32F;
typedef Tex2DRGBA32F::t_AutoPtr                         Tex2DRGBA32F_Ptr;
typedef Tex2D<LibSL::Math::Tuple<float,1> >             Tex2DLum32F;
typedef Tex2DLum32F::t_AutoPtr                          Tex2DLum32F_Ptr;

// 2D render targets

typedef RenderTarget2D<LibSL::Math::Tuple<unsigned char,1> >     RenderTarget2DLum;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DLum>       RenderTarget2DLum_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<unsigned char,2> >     RenderTarget2DRG;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRG>        RenderTarget2DRG_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<unsigned char,3> >     RenderTarget2DRGB;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGB>       RenderTarget2DRGB_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<unsigned char,4> >     RenderTarget2DRGBA;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGBA>      RenderTarget2DRGBA_Ptr;

typedef RenderTarget2D<LibSL::Math::Tuple<float,3> >             RenderTarget2DRGB32F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGB32F>    RenderTarget2DRGB32F_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<float,4> >             RenderTarget2DRGBA32F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGBA32F>   RenderTarget2DRGBA32F_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<float,1> >             RenderTarget2DLum32F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DLum32F>    RenderTarget2DLum32F_Ptr;

typedef RenderTarget2D<LibSL::Math::Tuple<half,1> >              RenderTarget2DLum16F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DLum16F>    RenderTarget2DLum16F_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<half,2> >              RenderTarget2DUV16F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DUV16F>     RenderTarget2DUV16F_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<half,3> >              RenderTarget2DRGB16F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGB16F>    RenderTarget2DRGB16F_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<half,4> >              RenderTarget2DRGBA16F;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DRGBA16F>   RenderTarget2DRGBA16F_Ptr;

// TODO: create / bind / unbind have to be modified for these to work

//typedef RenderTarget2D<LibSL::Math::Tuple<depth16,1> >           RenderTarget2DDepth16;
//typedef AutoPtr<RenderTarget2DDepth16>                           RenderTarget2DDepth16_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<depth24,1> >           RenderTarget2DDepth24;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DDepth24>   RenderTarget2DDepth24_Ptr;
typedef RenderTarget2D<LibSL::Math::Tuple<depth32,1> >           RenderTarget2DDepth32;
typedef LibSL::Memory::Pointer::AutoPtr<RenderTarget2DDepth32>   RenderTarget2DDepth32_Ptr;


// 3D textures

typedef Tex3D<LibSL::Math::Tuple<unsigned char,3> >     Tex3DRGB;
typedef Tex3DRGB::t_AutoPtr                             Tex3DRGB_Ptr;
typedef Tex3D<LibSL::Math::Tuple<unsigned char,4> >     Tex3DRGBA;
typedef Tex3DRGBA::t_AutoPtr                            Tex3DRGBA_Ptr;
typedef Tex3D<LibSL::Math::Tuple<unsigned char,1> >     Tex3DLum;
typedef Tex3DLum::t_AutoPtr                             Tex3DLum_Ptr;

typedef Tex3D<LibSL::Math::Tuple<half,4> >              Tex3DRGBA16F;
typedef Tex3DRGBA16F::t_AutoPtr                         Tex3DRGBA16F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<half,3> >              Tex3DRGB16F;
typedef Tex3DRGB16F::t_AutoPtr                          Tex3DRGB16F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<half,2> >              Tex3DUV16F;
typedef Tex3DUV16F::t_AutoPtr                           Tex3DUV16F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<half,1> >              Tex3DLum16F;
typedef Tex3DLum16F::t_AutoPtr                          Tex3DLum16F_Ptr;

typedef Tex3D<LibSL::Math::Tuple<float,4> >             Tex3DRGBA32F;
typedef Tex3DRGBA32F::t_AutoPtr                         Tex3DRGBA32F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<float,3> >             Tex3DRGB32F;
typedef Tex3DRGB32F::t_AutoPtr                          Tex3DRGB32F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<float,2> >             Tex3DUV32F;
typedef Tex3DUV32F::t_AutoPtr                           Tex3DUV32F_Ptr;
typedef Tex3D<LibSL::Math::Tuple<float,1> >             Tex3DLum32F;
typedef Tex3DLum32F::t_AutoPtr                          Tex3DLum32F_Ptr;
