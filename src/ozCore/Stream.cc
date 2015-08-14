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

#include "Stream.hh"

#include <cstring>
#include <zlib.h>

namespace oz
{

OZ_INTERNAL
void Stream::readFloats(float* values, int count)
{
  const char* data = readSkip(count * sizeof(float));

  if (order == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToValue<float> value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToValue<float> value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }
}

OZ_INTERNAL
void Stream::writeFloats(const float* values, int count)
{
  char* data = writeSkip(count * sizeof(float));

  if (order == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToBytes<float> value = { *values };

      data[0] = value.data[0];
      data[1] = value.data[1];
      data[2] = value.data[2];
      data[3] = value.data[3];
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::ToBytes<float> value = { *values };

      data[0] = value.data[3];
      data[1] = value.data[2];
      data[2] = value.data[1];
      data[3] = value.data[0];
    }
  }
}

Stream::Stream(const char* start, const char* end, Endian::Order order_) :
  streamPos(const_cast<char*>(start)), streamBegin(const_cast<char*>(start)),
  streamEnd(const_cast<char*>(end)), flags(0), order(order_)
{}

Stream::Stream(char* start, char* end, Endian::Order order_) :
  streamPos(start), streamBegin(start), streamEnd(end), flags(WRITABLE), order(order_)
{}

Stream::Stream(int size, Endian::Order order_) :
  streamPos(size == 0 ? nullptr : new char[size]), streamBegin(streamPos),
  streamEnd(streamPos + size), flags(WRITABLE | BUFFERED), order(order_)
{}

Stream::~Stream()
{
  free();
}

Stream::Stream(Stream&& s) :
  streamPos(s.streamPos), streamBegin(s.streamBegin), streamEnd(s.streamEnd), flags(s.flags),
  order(s.order)
{
  s.streamPos   = nullptr;
  s.streamBegin = nullptr;
  s.streamEnd   = nullptr;
  s.order       = Endian::NATIVE;
  s.flags       = 0;
}

Stream& Stream::operator = (Stream&& s)
{
  if (&s != this) {
    free();

    streamPos   = s.streamPos;
    streamBegin = s.streamBegin;
    streamEnd   = s.streamEnd;
    flags       = s.flags;
    order       = s.order;

    s.streamPos   = nullptr;
    s.streamBegin = nullptr;
    s.streamEnd   = nullptr;
    s.flags       = 0;
    s.order       = Endian::NATIVE;
  }
  return *this;
}

void Stream::seek(int offset)
{
  if (offset < 0 || int(streamEnd - streamBegin) < offset) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during stream seek",
             offset < 0 ? offset : offset - int(streamEnd - streamBegin));
  }

  streamPos = streamBegin + offset;
}

void Stream::resize(int newSize)
{
  int length = min<int>(tell(), newSize);

  char* newData = new char[newSize];
  memcpy(newData, streamBegin, length);
  delete[] streamBegin;

  streamBegin = newData;
  streamEnd   = newData + newSize;
  streamPos   = newData + length;
}

const char* Stream::readSkip(int count)
{
  char* oldPos = streamPos;
  streamPos += count;

  if (streamEnd < streamBegin) {
    OZ_ERROR("oz::Stream: Position overflow");
  }
  else if (streamPos > streamEnd) {
    OZ_ERROR("oz::Stream: Overrun for %d B during a read of %d B",
             int(streamPos - streamEnd), count);
  }
  return oldPos;
}

char* Stream::writeSkip(int count)
{
  char* oldPos = streamPos;
  streamPos += count;

  if (!(flags & WRITABLE)) {
    OZ_ERROR("oz::Stream: Writing to read-only stream");
  }
  else if (streamEnd < streamBegin) {
    OZ_ERROR("oz::Stream: Position overflow");
  }
  else if (streamPos > streamEnd) {
    if (flags & BUFFERED) {
      size_t size      = streamEnd - streamBegin;
      size_t length    = streamPos - streamBegin - count;
      size_t newLength = streamPos - streamBegin;

      size *= 2;
      size  = size < newLength ? (newLength + GRANULARITY - 1) & ~(GRANULARITY - 1) : size;

      char* newData = new char[size];
      memcpy(newData, streamBegin, length);
      delete[] streamBegin;

      streamBegin = newData;
      streamEnd   = newData + size;
      streamPos   = newData + newLength;
      oldPos      = newData + length;
    }
    else {
      OZ_ERROR("oz::Stream: Overrun for %d B during a write of %d B",
               int(streamPos - streamEnd), count);
    }
  }
  return oldPos;
}

void Stream::free()
{
  if (flags & BUFFERED) {
    delete[] streamBegin;

    streamPos   = nullptr;
    streamBegin = nullptr;
    streamEnd   = nullptr;
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

short Stream::readShort()
{
  Endian::ToValue<short> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeShort(short s)
{
  Endian::ToBytes<short> value = { order == Endian::NATIVE ? s : Endian::bswap(s) };

  write(value.data, sizeof(value.data));
}

ushort Stream::readUShort()
{
  Endian::ToValue<ushort> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeUShort(ushort s)
{
  Endian::ToBytes<ushort> value = { order == Endian::NATIVE ? s : Endian::bswap(s) };

  write(value.data, sizeof(value.data));
}

int Stream::readInt()
{
  Endian::ToValue<int> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeInt(int i)
{
  Endian::ToBytes<int> value = { order == Endian::NATIVE ? i : Endian::bswap(i) };

  write(value.data, sizeof(value.data));
}

uint Stream::readUInt()
{
  Endian::ToValue<uint> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeUInt(uint i)
{
  Endian::ToBytes<uint> value = { order == Endian::NATIVE ? i : Endian::bswap(i) };

  write(value.data, sizeof(value.data));
}

long64 Stream::readLong64()
{
  Endian::ToValue<long64> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeLong64(long64 l)
{
  Endian::ToBytes<long64> value = { order == Endian::NATIVE ? l : Endian::bswap(l) };

  write(value.data, sizeof(value.data));
}

ulong64 Stream::readULong64()
{
  Endian::ToValue<ulong64> value;
  read(value.data, sizeof(value.data));

  return order == Endian::NATIVE ? value.value : Endian::bswap(value.value);
}

void Stream::writeULong64(ulong64 l)
{
  Endian::ToBytes<ulong64> value = { order == Endian::NATIVE ? l : Endian::bswap(l) };

  write(value.data, sizeof(value.data));
}

float Stream::readFloat()
{
  const char* data = readSkip(sizeof(float));

  if (order == Endian::NATIVE) {
    Endian::ToValue<float> value = { { data[0], data[1], data[2], data[3] } };

    return value.value;
  }
  else {
    Endian::ToValue<float> value = { { data[3], data[2], data[1], data[0] } };

    return value.value;
  }
}

void Stream::writeFloat(float f)
{
  char* data = writeSkip(sizeof(float));

  Endian::ToBytes<float> value = { f };

  if (order == Endian::NATIVE) {
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

  if (order == Endian::NATIVE) {
    Endian::ToValue<double> value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };

    return value.value;
  }
  else {
    Endian::ToValue<double> value = {
      { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
    };

    return value.value;
  }
}

void Stream::writeDouble(double d)
{
  char* data = writeSkip(sizeof(double));

  Endian::ToBytes<double> value = { d };

  if (order == Endian::NATIVE) {
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
  const char* begin = streamPos;

  while (streamPos < streamEnd && *streamPos != '\0') {
    ++streamPos;
  }
  if (streamPos == streamEnd) {
    OZ_ERROR("oz::Stream: Overrun while looking for the end of a string.");
  }

  ++streamPos;
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
  const char* begin = streamPos;

  while (streamPos < streamEnd && *streamPos != '\n' && *streamPos != '\r') {
    ++streamPos;
  }

  int length = int(streamPos - begin);

  streamPos += (streamPos < streamEnd) +
               (streamPos < streamEnd - 1 && streamPos[0] == '\r' && streamPos[1] == '\n');

  return String(begin, length);
}

void Stream::writeLine(const String& s)
{
  write(s, s.length() + 1);
  streamPos[-1] = '\n';
}

void Stream::writeLine(const char* s)
{
  write(s, String::length(s) + 1);
  streamPos[-1] = '\n';
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

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(streamBegin));
  zstream.avail_in  = tell();
  zstream.next_out  = reinterpret_cast<ubyte*>(out.streamBegin + 8);
  zstream.avail_out = outSize - 8;

  int ret = ::deflate(&zstream, Z_FINISH);
  deflateEnd(&zstream);

  if (ret != Z_STREAM_END) {
    return Stream();
  }
  else {
    outSize = 8 + int(zstream.total_out);

    out.seek(outSize);
    out.resize(outSize);

    // Write size and order of the original data (in little endian).
    int* start = reinterpret_cast<int*>(out.streamBegin);

#if OZ_BYTE_ORDER == 4321
    start[0] = Endian::bswap32(tell());
    start[1] = Endian::bswap32(order);
#else
    start[0] = tell();
    start[1] = order;
#endif

    return out;
  }
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

  const int* start = reinterpret_cast<int*>(streamBegin);

#if OZ_BYTE_ORDER == 4321
  int           outSize  = Endian::bswap32(start[0]);
  Endian::Order outOrder = Endian::Order(Endian::bswap32(start[1]));
#else
  int           outSize  = start[0];
  Endian::Order outOrder = Endian::Order(start[1]);
#endif

  Stream out(outSize, outOrder);

  zstream.next_in   = reinterpret_cast<ubyte*>(const_cast<char*>(streamBegin + 8));
  zstream.avail_in  = capacity() - 8;
  zstream.next_out  = reinterpret_cast<ubyte*>(out.streamBegin);
  zstream.avail_out = outSize;

  int ret = ::inflate(&zstream, Z_FINISH);
  inflateEnd(&zstream);

  if (ret != Z_STREAM_END) {
    return Stream();
  }
  else {
    return out;
  }
}

}
