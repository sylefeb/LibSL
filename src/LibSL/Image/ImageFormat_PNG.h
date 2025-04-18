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
// LibSL::Image::ImageFormat_PNG
// ------------------------------------------------------
//
// Load/Save PNG file format
//
// ------------------------------------------------------
// Sylvain Lefebvre - 2006-03-21
// ------------------------------------------------------

#pragma once

#include <LibSL/Image/Image.h>

#include <map>

namespace LibSL {
  namespace Image {

    class ImageFormat_PNG : public ImageFormat_plugin
    {
    public:
      ImageFormat_PNG();

      void        save(const char *,const Image *)  const;
      Image      *load(const char *)                const;
      const char *signature()                       const {return "png";}

      void        save(const char*, const Image*, const std::map<std::string,std::string>& key_value_text) const;
      Image*      load(const char*, std::map<std::string, std::string>& _key_value_text)                   const;

      #ifndef EMSCRIPTEN // SL 2025-04-17 hotfix as this result in Emscripten compilation error
      // NS 2023-01-12: new interface to save in streams
      template<typename TChar>
      void        save(std::basic_ostream<TChar>&,
                       const Image*,
                       const std::map<std::string,std::string>& key_value_text) const;
      #endif
    };

  } //namespace LibSL::Image
} //namespace LibSL

// ------------------------------------------------------

//#define LIBSL_ENABLE_IMAGE_FORMAT(F) static LibSL::Image::ImageFormat_##F s_ImageFormat_##F;
//extern LibSL::Image::ImageFormat_PNG s_PNG;

// ------------------------------------------------------
