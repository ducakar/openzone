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

Buffer::Buffer(int size_) :
  data(new char[size_]), size(size_)
{}

Buffer::Buffer(const char* data_, int size_) :
  data(new char[size_]), size(size_)
{
  memcpy(data, data_, size);
}

Buffer::Buffer(const String& s) :
  data(new char[s.length()]), size(s.length())
{
  memcpy(data, s.cstr(), size);
}

Buffer::~Buffer()
{
  delete[] data;
}

Buffer::Buffer(const Buffer& b) :
  data(new char[b.size]), size(b.size)
{
  memcpy(data, b.data, size);
}

Buffer::Buffer(Buffer&& b) :
  data(b.data), size(b.size)
{
  b.data = nullptr;
  b.size = 0;
}

Buffer& Buffer::operator = (const Buffer& b)
{
  if (&b != this) {
    if (size != b.size) {
      delete[] data;

      data = new char[b.size];
      size = b.size;
    }

    memcpy(data, b.data, b.size);
  }
  return *this;
}

Buffer& Buffer::operator = (Buffer&& b)
{
  if (&b != this) {
    delete[] data;

    data = b.data;
    size = b.size;

    b.data = nullptr;
    b.size = 0;
  }
  return *this;
}

bool Buffer::operator == (const Buffer& b) const
{
  return size == b.size && memcmp(data, b.data, size) == 0;
}

bool Buffer::operator != (const Buffer& b) const
{
  return !operator == (b);
}

InputStream Buffer::inputStream(Endian::Order order) const
{
  return InputStream(data, data + size, order);
}

OutputStream Buffer::outputStream(Endian::Order order)
{
  return OutputStream(data, data + size, order);
}

String Buffer::toString() const
{
  return String(data, size);
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
  int newSize = 4 + int(deflateBound(&zstream, size));
  buffer.resize(newSize);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data));
  zstream.avail_in  = size;
  zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data + 4);
  zstream.avail_out = buffer.size;

  int ret = ::deflate(&zstream, Z_FINISH);
  deflateEnd(&zstream);

  newSize = ret != Z_STREAM_END ? 0 : 4 + int(zstream.total_out);
  buffer.resize(newSize);

  if (newSize != 0) {
    // Write size of the original data (in little endian).
#if OZ_BYTE_ORDER == 4321
    *reinterpret_cast<int*>(buffer.data) = Endian::bswap32(oldSize);
#else
    *reinterpret_cast<int*>(buffer.data) = size;
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

  buffer.resize(newSize);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data + 4));
  zstream.avail_in  = size - 4;
  zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data);
  zstream.avail_out = buffer.size;

  int ret = ::inflate(&zstream, Z_FINISH);
  inflateEnd(&zstream);

  if (ret != Z_STREAM_END) {
    buffer.resize(0);
  }
  return buffer;
}

void Buffer::resize(int newSize)
{
  hard_assert(newSize >= 0);

  if (newSize != size) {
    char* newData = new char[newSize];
    memcpy(newData, data, min<int>(size, newSize));
    delete[] data;

    data = newData;
    size = newSize;
  }
}

}
