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
// LibSL::TemplateHelpers::ExecuteOnTypeList
// ------------------------------------------------------
//
// Executes a functor on a type list - Useful when different 
// templates are hidden behind a common virtual interface (see below).
//
//
// [Usage exemple]
//
// Scenario:
//
//   An Image_generic class, templated by a pixel format, inherits from 
//   a virtual interface Image.
//   When it comes to saving an image, the true pixel format must be
//   retrieved (RGB, RGBA, etc...). A dynamic cast must be done on all 
//   supported formats - However writting this code is a pain.
//   ExecuteOnTypeList makes this process easier:
//
// class data 
// { 
//  public: 
//   Image *image;
// };
// 
// template <class T> class functor 
// {
//  public: 
//   void operator()(data& d) 
//   {
//     if (dynamic_cast<T *>(d.image) != NULL) {
//       std::cerr << "Image has type " << typeid(T).name() << std::endl;
//       saveImage<T>(...)
//     }
//   }
// };
// 
// void main(...)
// {
//   ...
//   data d;
//   d.image=image_with_unknown_format;
//   ExecuteOnTypeList<LOKI_TYPELIST_2(ImageRGB,ImageRGBA),functor,data> test(d);
//   ...
// }
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-23-03
// ------------------------------------------------------

#pragma once

// ------------------------------------------------------

#include <loki/Typelist.h>

// ------------------------------------------------------

namespace LibSL {
  namespace TemplateHelpers {

    /// Execute type dependent functor, with data in/out

    template <class T_List,template <class> class T_Functor,class T_Data> 
    class ExecuteOnTypeList;

    template <template <class> class T_Functor,class T_Data> 
    class ExecuteOnTypeList< Loki::NullType, T_Functor, T_Data >
    {
    protected:
      T_Data& m_RefData;
      T_Data& data() {return (m_RefData);}
    public:
      ExecuteOnTypeList(T_Data& data) : m_RefData(data) {}
    };

    template <class T_Head,class T_Tail,template <class> class T_Functor,class T_Data> 
    class ExecuteOnTypeList< Loki::Typelist<T_Head,T_Tail>, T_Functor, T_Data >
      : public ExecuteOnTypeList<T_Tail,T_Functor,T_Data>
    {
    public:

      ExecuteOnTypeList(T_Data& data) : ExecuteOnTypeList<T_Tail,T_Functor,T_Data>(data)
      {
        T_Functor<T_Head> f;
        f(ExecuteOnTypeList<T_Tail,T_Functor,T_Data>::data());
      }

    };

  } //namespace LibSL::TemplateHelpers
} //namespace LibSL

// ------------------------------------------------------
