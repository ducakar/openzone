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

#ifdef OZ_LZ4
# include <lz4.h>
#endif
#include <zlib.h>

namespace oz
{

Buffer::Buffer(int size_) :
  data(mReallocate(nullptr, 0, size_)), size(size_)
{}

Buffer::Buffer(const char* data_, int size_) :
  data(mReallocate(nullptr, 0, size_)), size(size_)
{
  mCopy(data, data_, size);
}

Buffer::Buffer(const String& s) :
  data(mReallocate(nullptr, 0, s.length())), size(s.length())
{
  mCopy(data, s.cstr(), size);
}

Buffer::~Buffer()
{
  delete[] data;
}

Buffer::Buffer(const Buffer& b) :
  data(mReallocate(nullptr, 0, b.size)), size(b.size)
{
  mCopy(data, b.data, size);
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

    mCopy(data, b.data, b.size);
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
  return size == b.size && mCompare(data, b.data, size) == 0;
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
  char*  buffer;
  String s = String::create(size, &buffer);

  mCopy(buffer, data, size);
  return s;
}

Buffer Buffer::compress(int level) const
{
  Buffer buffer;

  if (level == -2) {
#ifdef OZ_LZ4

    int newSize = LZ4_compressBound(size) + 4;
    buffer.resize(newSize);

    int bytesWritten = LZ4_compress(data, buffer.data + 4, size);

    if (bytesWritten == 0) {
      buffer.resize(0);
    }
    else {
      if (bytesWritten + 4 != newSize) {
        buffer.resize(bytesWritten + 4);
      }
      // Write size of the original data, ensure portability between little and big endian platforms.
# if OZ_BYTE_ORDER == 4321
      *reinterpret_cast<int*>(buffer.data) = ~Endian::bswap32(size);
# else
      *reinterpret_cast<int*>(buffer.data) = ~size;
# endif
    }

#else

    OZ_ERROR("oz::Buffer: LZ4 requested but compiled without OZ_LZ4");

#endif
  }
  else {
    z_stream zstream;
    zstream.zalloc = nullptr;
    zstream.zfree  = nullptr;
    zstream.opaque = nullptr;

    if (deflateInit(&zstream, level) != Z_OK) {
      return buffer;
    }

    // Upper bound for compressed data plus sizeof(int) to write down size of the uncompressed data.
    int newSize = int(deflateBound(&zstream, ulong(size))) + 4;
    buffer.resize(newSize);

    zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data));
    zstream.avail_in  = uint(size);
    zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data + 4);
    zstream.avail_out = uint(newSize);

    int ret = ::deflate(&zstream, Z_FINISH);
    deflateEnd(&zstream);

    if (ret != Z_STREAM_END) {
      buffer.resize(0);
    }
    else {
      if (zstream.total_out + 4 != uint(newSize)) {
        buffer.resize(int(zstream.total_out + 4));
      }
      // Write size of the original data, ensure portability between little and big endian platforms.
#if OZ_BYTE_ORDER == 4321
      *reinterpret_cast<int*>(buffer.data) = Endian::bswap32(size);
#else
      *reinterpret_cast<int*>(buffer.data) = size;
#endif
    }
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

  if (newSize < 0) {
#ifdef OZ_LZ4

    newSize = ~newSize;

    buffer.resize(newSize);

    int ret = LZ4_decompress_safe(data + 4, buffer.data, size - 4, newSize);

    if (ret < 0) {
      buffer.resize(0);
    }

#else

    OZ_ERROR("oz::Buffer: LZ4 requested but compiled without OZ_LZ4");

#endif
  }
  else {
    buffer.resize(newSize);

    z_stream zstream;
    zstream.zalloc = nullptr;
    zstream.zfree  = nullptr;
    zstream.opaque = nullptr;

    if (inflateInit(&zstream) != Z_OK) {
      return buffer;
    }

    zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(data + 4));
    zstream.avail_in  = uint(size - 4);
    zstream.next_out  = reinterpret_cast<ubyte*>(buffer.data);
    zstream.avail_out = uint(newSize);

    int ret = ::inflate(&zstream, Z_FINISH);
    inflateEnd(&zstream);

    if (ret != Z_STREAM_END) {
      buffer.resize(0);
    }
  }
  return buffer;
}

void Buffer::resize(int newSize)
{
  hard_assert(newSize >= 0);

  if (newSize != size) {
    data = mReallocate(data, size, newSize);
    size = newSize;
  }
}

}
