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
#include "LibSL.precompiled.h"
// ------------------------------------------------------

#include "ArrayHelpers.h"
#include <LibSL/Errors/Errors.h>

#include <zlib.h>

// ------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// ------------------------------------------------------

#define NAMESPACE LibSL::ArrayHelpers

// ------------------------------------------------------

void NAMESPACE::pack(const uchar *data, uint sz, Array<uchar>& _packed)
{
  // compress
  int      ret;
  z_stream strm;
  // allocate deflate state
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  int level = Z_DEFAULT_COMPRESSION;
  ret = deflateInit(&strm, level);
  sl_assert(ret == Z_OK);
  strm.avail_in = sz;
  strm.next_in = (uchar*)data;
  _packed.truncate(_packed.allocatedSize());
  strm.avail_out = _packed.size();
  strm.next_out = _packed.raw();
  ret = deflate(&strm, Z_FINISH);
  if (ret == Z_STREAM_ERROR) {
    throw Fatal("[ArrayHelpers::pack] error");
  }
  sl_assert(ret != Z_STREAM_ERROR);
  sl_assert(strm.avail_out > 0);
  deflateEnd(&strm);
  uint packed_sz = _packed.size() - strm.avail_out;
  _packed.truncate(packed_sz);
}

// ------------------------------------------------------------------------

void NAMESPACE::unpack(const uchar *data, uint sz, Array<uchar>& _unpacked)
{
  // decompression
  int      ret;
  z_stream strm;
  // allocate inflate state 
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  sl_assert(ret == Z_OK);
  // decompress until deflate stream ends or end of file
  _unpacked.truncate(_unpacked.allocatedSize());
  strm.avail_in = sz;
  strm.next_in = (uchar*)data;
  strm.avail_out = _unpacked.size();
  strm.next_out = _unpacked.raw();
  ret = inflate(&strm, Z_NO_FLUSH);
  if (ret == Z_STREAM_ERROR) {
    throw Fatal("[ArrayHelpers::unpack] error");
  }
  sl_assert(ret != Z_STREAM_ERROR);
  switch (ret) {
  case Z_NEED_DICT:
  case Z_DATA_ERROR:
  case Z_MEM_ERROR:
    inflateEnd(&strm);
    throw Fatal("[ArrayHelpers::unpack] error");
  }
  // clean up
  inflateEnd(&strm);
  // set size
  uint unpacked_sz = _unpacked.size() - strm.avail_out;
  _unpacked.truncate(unpacked_sz);
}

// ------------------------------------------------------

// base64 encoding/decoding, see http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

static const std::string default_base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/"
"="; // special marker

std::string NAMESPACE::base64_encode(const uchar* buf, uint bufLen)
{
  return base64_encode(buf, bufLen, default_base64_chars);
}

void NAMESPACE::base64_decode(std::string const& encoded_string, Array<uchar>& _decoded)
{
  base64_decode(encoded_string, _decoded, default_base64_chars);
}

// ------------------------------------------------------

std::string NAMESPACE::base64_encode(const uchar* buf, uint bufLen, const std::string& base64_chars)
{
  std::string ret;
  int i = 0;
  int j = 0;
  uchar char_array_3[3];
  uchar char_array_4[4];
  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;
      for (i = 0; (i < 4); i++) {
        ret += base64_chars[char_array_4[i]];
      }
      i = 0;
    }
  }
  if (i) {
    for (j = i; j < 3; j++) {
      char_array_3[j] = '\0';
    }
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;
    for (j = 0; (j < i + 1); j++) {
      ret += base64_chars[char_array_4[j]];
    }
    while (i++ < 3) {
      ret += base64_chars[64];
    }
  }
  return ret;
}

// ------------------------------------------------------

static inline bool is_base64(unsigned char c, const std::string& base64_chars)
{
  ForIndex(i, 64) { if (c == base64_chars[i]) return true; } // NOTE: Could be made fast depending on base64_chars set. Make it a predicate?
  return false;
}

void NAMESPACE::base64_decode(std::string const& encoded_string, Array<uchar>& _decoded, const std::string& base64_chars)
{
  int in_len = (int)encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  _decoded.truncate(_decoded.allocatedSize());
  uint num_out = 0;
  while (in_len-- && (encoded_string[in_] != base64_chars[64]) && is_base64(encoded_string[in_],base64_chars)) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++) {
        char_array_4[i] = (uchar)base64_chars.find(char_array_4[i]);
      }
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
      for (i = 0; (i < 3); i++) {
        if (num_out == _decoded.size()) {
          throw Fatal("[ArrayHelpers::base64_decode] output buffer is not large enough");
        }
        _decoded[num_out] = char_array_3[i];
        num_out++;
      }
      i = 0;
    }
  }
  if (i) {
    for (j = i; j < 4; j++) {
      char_array_4[j] = 0;
    }
    for (j = 0; j < 4; j++) {
      char_array_4[j] = (uchar)base64_chars.find(char_array_4[j]);
    }
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
    for (j = 0; (j < i - 1); j++) {
      if (num_out == _decoded.size()) {
        throw Fatal("[ArrayHelpers::base64_decode] output buffer is not large enough");
      }
      _decoded[num_out] = char_array_3[j];
      num_out++;
    }
  }
  _decoded.truncate(num_out);
}

// ------------------------------------------------------
