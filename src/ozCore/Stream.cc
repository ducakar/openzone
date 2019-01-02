/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include "Stream.hh"

#include "Alloc.hh"

#include <cstring>
#include <zlib.h>

namespace oz
{

void Stream::readFloats(float* values, int count)
{
  const char* data = readSkip(count * sizeof(float));

  if (order_ == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToValue<float> value = {{data[0], data[1], data[2], data[3]}};

      *values = value.value;
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToValue<float> value = {{data[3], data[2], data[1], data[0]}};

      *values = value.value;
    }
  }
}

void Stream::writeFloats(const float* values, int count)
{
  char* data = writeSkip(count * sizeof(float));

  if (order_ == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToBytes<float> value = {*values};

      data[0] = value.data[0];
      data[1] = value.data[1];
      data[2] = value.data[2];
      data[3] = value.data[3];
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToBytes<float> value = {*values};

      data[0] = value.data[3];
      data[1] = value.data[2];
      data[2] = value.data[1];
      data[3] = value.data[0];
    }
  }
}

Stream::Stream(const char* start, const char* end, Endian::Order order)
  : pos_(const_cast<char*>(start)), begin_(const_cast<char*>(start)), end_(const_cast<char*>(end)),
    order_(order)
{}

Stream::Stream(char* start, char* end, Endian::Order order)
  : pos_(start), begin_(start), end_(end), flags_(WRITABLE), order_(order)
{}

Stream::Stream(int size, Endian::Order order)
  : pos_(size == 0 ? nullptr : new char[size]), begin_(pos_), end_(pos_ + size),
    flags_(WRITABLE | BUFFERED), order_(order)
{}

Stream::~Stream()
{
  free();
}

Stream::Stream(Stream&& other) noexcept
  : Stream()
{
  swap(*this, other);
}

Stream& Stream::operator=(Stream&& other) noexcept
{
  swap(*this, other);
  return *this;
}

void Stream::seek(int offset)
{
  if (offset < 0 || int(end_ - begin_) < offset) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during stream seek",
             offset < 0 ? offset : offset - int(end_ - begin_));
  }

  pos_ = begin_ + offset;
}

void Stream::resize(int newSize)
{
  int length = min<int>(tell(), newSize);

  char* newData = new char[newSize];
  memcpy(newData, begin_, length);
  delete[] begin_;

  pos_   = newData + length;
  begin_ = newData;
  end_   = newData + newSize;
}

const char* Stream::readSkip(int count)
{
  char* oldPos = pos_;
  pos_ += count;

  if (end_ < begin_) {
    OZ_ERROR("oz::Stream: Position overflow");
  }
  else if (pos_ > end_) {
    OZ_ERROR("oz::Stream: Overrun for %d B during a read of %d B", int(pos_ - end_), count);
  }
  return oldPos;
}

char* Stream::writeSkip(int count)
{
  char* oldPos = pos_;
  pos_ += count;

  if (!(flags_ & WRITABLE)) {
    OZ_ERROR("oz::Stream: Writing to read-only stream");
  }
  else if (end_ < begin_) {
    OZ_ERROR("oz::Stream: Position overflow");
  }
  else if (pos_ > end_) {
    if (flags_ & BUFFERED) {
      size_t oldLength = oldPos - begin_;
      size_t newLength = pos_ - begin_;
      size_t size      = end_ - begin_;

      size = size == 0 ? 4096 : size + size / 2;
      size = max<size_t>(size, newLength);

      char* newData = new char[size];

      if (begin_ != nullptr) {
        memcpy(newData, begin_, oldLength);
        delete[] begin_;
      }

      begin_ = newData;
      end_   = newData + size;
      pos_   = newData + newLength;
      oldPos = newData + oldLength;
    }
    else {
      OZ_ERROR("oz::Stream: Overrun for %d B during a write of %d B", int(pos_ - end_), count);
    }
  }
  return oldPos;
}

void Stream::free()
{
  if (flags_ & BUFFERED) {
    delete[] begin_;

    pos_   = nullptr;
    begin_ = nullptr;
    end_   = nullptr;
  }
}

void Stream::read(char* array, int count)
{
  const char* data = readSkip(count * sizeof(char));
  memcpy(array, data, count);
}

void Stream::write(const char* array, int count)
{
  char* data = writeSkip(count * sizeof(char));
  memcpy(data, array, count);
}

bool Stream::readBool()
{
  const char* data = readSkip(sizeof(bool));
  return bool(*data);
}

void Stream::writeBool(bool b)
{
  char* data = writeSkip(sizeof(bool));
  *data = char(b);
}

char Stream::readChar()
{
  const char* data = readSkip(sizeof(char));
  return char(*data);
}

void Stream::writeChar(char c)
{
  char* data = writeSkip(sizeof(char));
  *data = char(c);
}

byte Stream::readByte()
{
  const char* data = readSkip(sizeof(byte));
  return byte(*data);
}

void Stream::writeByte(byte b)
{
  char* data = writeSkip(sizeof(byte));
  *data = char(b);
}

ubyte Stream::readUByte()
{
  const char* data = readSkip(sizeof(ubyte));
  return ubyte(*data);
}

void Stream::writeUByte(ubyte b)
{
  char* data = writeSkip(sizeof(ubyte));
  *data = char(b);
}

int16 Stream::readInt16()
{
  Endian::ToValue<int16> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeInt16(int16 s)
{
  Endian::ToBytes<int16> value = {order_ == Endian::NATIVE ? s : Endian::bswap(s)};

  write(value.data, sizeof(value.data));
}

uint16 Stream::readUInt16()
{
  Endian::ToValue<uint16> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeUInt16(uint16 s)
{
  Endian::ToBytes<uint16> value = {order_ == Endian::NATIVE ? s : Endian::bswap(s)};

  write(value.data, sizeof(value.data));
}

int Stream::readInt()
{
  Endian::ToValue<int> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeInt(int i)
{
  Endian::ToBytes<int> value = {order_ == Endian::NATIVE ? i : Endian::bswap(i)};

  write(value.data, sizeof(value.data));
}

uint Stream::readUInt()
{
  Endian::ToValue<uint> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeUInt(uint i)
{
  Endian::ToBytes<uint> value = {order_ == Endian::NATIVE ? i : Endian::bswap(i)};

  write(value.data, sizeof(value.data));
}

int64 Stream::readInt64()
{
  Endian::ToValue<int64> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeInt64(int64 l)
{
  Endian::ToBytes<int64> value = {order_ == Endian::NATIVE ? l : Endian::bswap(l)};

  write(value.data, sizeof(value.data));
}

uint64 Stream::readUInt64()
{
  Endian::ToValue<uint64> value;
  read(value.data, sizeof(value.data));

  return order_ == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeUInt64(uint64 l)
{
  Endian::ToBytes<uint64> value = {order_ == Endian::NATIVE ? l : Endian::bswap(l)};

  write(value.data, sizeof(value.data));
}

float Stream::readFloat()
{
  const char* data = readSkip(sizeof(float));

  if (order_ == Endian::NATIVE) {
    Endian::ToValue<float> value = {{data[0], data[1], data[2], data[3]}};

    return value.value;
  }
  else {
    Endian::ToValue<float> value = {{data[3], data[2], data[1], data[0]}};

    return value.value;
  }
}

void Stream::writeFloat(float f)
{
  char* data = writeSkip(sizeof(float));

  Endian::ToBytes<float> value = {f};

  if (order_ == Endian::NATIVE) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
  }
  else {
    data[0] = value.data[3];
    data[1] = value.data[2];
    data[2] = value.data[1];
    data[3] = value.data[0];
  }
}

double Stream::readDouble()
{
  const char* data = readSkip(sizeof(double));

  if (order_ == Endian::NATIVE) {
    Endian::ToValue<double> value = {
      {data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]}
    };

    return value.value;
  }
  else {
    Endian::ToValue<double> value = {
      {data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0]}
    };

    return value.value;
  }
}

void Stream::writeDouble(double d)
{
  char* data = writeSkip(sizeof(double));

  Endian::ToBytes<double> value = {d};

  if (order_ == Endian::NATIVE) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
  }
  else {
    data[0] = value.data[7];
    data[1] = value.data[6];
    data[2] = value.data[5];
    data[3] = value.data[4];
    data[4] = value.data[3];
    data[5] = value.data[2];
    data[6] = value.data[1];
    data[7] = value.data[0];
  }
}

const char* Stream::readString()
{
  const char* begin = pos_;

  while (pos_ < end_ && *pos_ != '\0') {
    ++pos_;
  }
  if (pos_ == end_) {
    OZ_ERROR("oz::Stream: Overrun while looking for the end of a string.");
  }

  ++pos_;
  return begin;
}

void Stream::writeString(const String& s)
{
  write(s, s.length() + 1);
}

void Stream::writeString(const char* s)
{
  write(s, String::length(s) + 1);
}

String Stream::readLine()
{
  const char* begin = pos_;

  while (pos_ < end_ && *pos_ != '\n' && *pos_ != '\r') {
    ++pos_;
  }

  int length = int(pos_ - begin);

  pos_ += size_t(pos_ < end_);
  pos_ += size_t(pos_ + 1 == end_ && pos_[-1] == '\r' && pos_[0] == '\n');

  return String(begin, length);
}

void Stream::writeLine()
{
  write("\n", 1);
}

void Stream::writeLine(const String& s)
{
  write(s, s.length() + 1);
  pos_[-1] = '\n';
}

void Stream::writeLine(const char* s)
{
  write(s, String::length(s) + 1);
  pos_[-1] = '\n';
}

Stream Stream::compress(int level) const
{
  z_stream zstream;
  zstream.zalloc = nullptr;
  zstream.zfree  = nullptr;
  zstream.opaque = nullptr;

  if (deflateInit(&zstream, level) != Z_OK) {
    return Stream();
  }

  // Upper bound for compressed data plus 2 * sizeof(int) for meta-data.
  int    outSize    = 8 + int(deflateBound(&zstream, tell()));
  Stream out        = Stream(outSize, Endian::LITTLE);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(begin_));
  zstream.avail_in  = tell();
  zstream.next_out  = reinterpret_cast<ubyte*>(out.begin_ + 8);
  zstream.avail_out = outSize - 8;

  int ret = ::deflate(&zstream, Z_FINISH);
  deflateEnd(&zstream);

  if (ret != Z_STREAM_END) {
    return Stream();
  }

  outSize = 8 + int(zstream.total_out);

  out.seek(outSize);
  out.resize(outSize);

  // Write size and order of the original data (in little endian).
  int* start = reinterpret_cast<int*>(out.begin_);

#if OZ_BYTE_ORDER == 4321
  start[0] = Endian::bswap32(tell());
  start[1] = Endian::bswap32(order_);
#else
  start[0] = tell();
  start[1] = order_;
#endif

  return out;
}

Stream Stream::decompress() const
{
  if (capacity() < 8) {
    return Stream();
  }

  z_stream zstream;
  zstream.zalloc = nullptr;
  zstream.zfree  = nullptr;
  zstream.opaque = nullptr;

  if (inflateInit(&zstream) != Z_OK) {
    return Stream();
  }

  const int* start = reinterpret_cast<int*>(begin_);

#if OZ_BYTE_ORDER == 4321
  int           outSize  = Endian::bswap32(start[0]);
  Endian::Order outOrder = Endian::Order(Endian::bswap32(start[1]));
#else
  int           outSize  = start[0];
  Endian::Order outOrder = Endian::Order(start[1]);
#endif

  Stream out(outSize, outOrder);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(begin_ + 8));
  zstream.avail_in  = capacity() - 8;
  zstream.next_out  = reinterpret_cast<ubyte*>(out.begin_);
  zstream.avail_out = outSize;

  int ret = ::inflate(&zstream, Z_FINISH);
  inflateEnd(&zstream);

  return ret == Z_STREAM_END ? static_cast<Stream&&>(out) : Stream();
}

}
