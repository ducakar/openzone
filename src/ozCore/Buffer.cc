/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozCore/Buffer.cc
 */

#include "Buffer.hh"

#include <cstring>
#include <zlib.h>

namespace oz
{

Buffer::Buffer(const String& s) :
  List<char>(s.c(), s.length())
{}

String Buffer::toString() const
{
  return String(data, count);
}

Buffer Buffer::compress(int level) const
{
  Buffer buffer;

  z_stream zstream;
  zstream.zalloc = nullptr;
  zstream.zfree  = nullptr;
  zstream.opaque = nullptr;

  if (deflateInit(&zstream, level) != Z_OK) {
    return buffer;
  }

  // Upper bound for compressed data plus sizeof(int) to write down size of the uncompressed data.
  int newSize = 4 + int(deflateBound(&zstream, count));
  buffer.resize(newSize, true);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data));
  zstream.avail_in  = count;
  zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data + 4);
  zstream.avail_out = buffer.count;

  int ret = ::deflate(&zstream, Z_FINISH);
  deflateEnd(&zstream);

  newSize = ret != Z_STREAM_END ? 0 : 4 + int(zstream.total_out);
  buffer.resize(newSize, true);

  if (newSize != 0) {
    // Write size of the original data (in little endian).
#if OZ_BYTE_ORDER == 4321
    *reinterpret_cast<int*>(buffer.data) = Endian::bswap32(oldSize);
#else
    *reinterpret_cast<int*>(buffer.data) = count;
#endif
  }
  return buffer;
}

Buffer Buffer::decompress() const
{
  Buffer buffer;

#if OZ_BYTE_ORDER == 4321
  int newSize = Endian::bswap32(*reinterpret_cast<int*>(data));
#else
  int newSize = *reinterpret_cast<int*>(data);
#endif

  z_stream zstream;
  zstream.zalloc = nullptr;
  zstream.zfree  = nullptr;
  zstream.opaque = nullptr;

  if (inflateInit(&zstream) != Z_OK) {
    return buffer;
  }

  buffer.resize(newSize, true);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data + 4));
  zstream.avail_in  = count - 4;
  zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data);
  zstream.avail_out = buffer.count;

  int ret = ::inflate(&zstream, Z_FINISH);
  inflateEnd(&zstream);

  if (ret != Z_STREAM_END) {
    buffer.resize(0, true);
  }
  return buffer;
}

}
