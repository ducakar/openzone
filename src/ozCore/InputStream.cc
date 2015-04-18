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
 * @file ozCore/InputStream.cc
 */

#include "InputStream.hh"

#include "System.hh"

namespace oz
{

OZ_HIDDEN
InputStream::InputStream(char* pos, char* start, const char* end, Endian::Order order_) :
  streamPos(pos), streamBegin(start), streamEnd(end), order(order_)
{}

OZ_HIDDEN
void InputStream::readFloats(float* values, int count)
{
  const char* data = skip(count * sizeof(float));

  if (order == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }
}

InputStream::InputStream(const char* start, const char* end, Endian::Order order_) :
  streamPos(const_cast<char*>(start)), streamBegin(const_cast<char*>(start)), streamEnd(end),
  order(order_)
{}

void InputStream::seek(int offset)
{
  if (offset < 0 || int(streamEnd - streamBegin) < offset) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during stream seek",
             offset < 0 ? offset : offset - int(streamEnd - streamBegin));
  }

  streamPos = streamBegin + offset;
}

const char* InputStream::skip(int count)
{
  const char* oldPos = streamPos;
  streamPos += count;

  if (streamPos > streamEnd) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during a read of %d B",
             int(streamPos - streamEnd), count);
  }
  return oldPos;
}

bool InputStream::readBool()
{
  const char* data = skip(sizeof(bool));
  return bool(*data);
}

char InputStream::readChar()
{
  const char* data = skip(sizeof(char));
  return char(*data);
}

void InputStream::readChars(char* array, int count)
{
  const char* data = skip(count * sizeof(char));
  mCopy(array, data, count);
}

byte InputStream::readByte()
{
  const char* data = skip(sizeof(byte));
  return byte(*data);
}

ubyte InputStream::readUByte()
{
  const char* data = skip(sizeof(ubyte));
  return ubyte(*data);
}

short InputStream::readShort()
{
  const char* data = skip(sizeof(short));

  if (order == Endian::NATIVE) {
    Endian::BytesToShort value = { { data[0], data[1] } };

    return value.value;
  }
  else {
    Endian::BytesToShort value = { { data[1], data[0] } };

    return value.value;
  }
}

ushort InputStream::readUShort()
{
  const char* data = skip(sizeof(ushort));

  if (order == Endian::NATIVE) {
    Endian::BytesToUShort value = { { data[0], data[1] } };

    return value.value;
  }
  else {
    Endian::BytesToUShort value = { { data[1], data[0] } };

    return value.value;
  }
}

int InputStream::readInt()
{
  const char* data = skip(sizeof(int));

  if (order == Endian::NATIVE) {
    Endian::BytesToInt value = { { data[0], data[1], data[2], data[3] } };

    return value.value;
  }
  else {
    Endian::BytesToInt value = { { data[3], data[2], data[1], data[0] } };

    return value.value;
  }
}

uint InputStream::readUInt()
{
  const char* data = skip(sizeof(uint));

  if (order == Endian::NATIVE) {
    Endian::BytesToUInt value = { { data[0], data[1], data[2], data[3] } };

    return value.value;
  }
  else {
    Endian::BytesToUInt value = { { data[3], data[2], data[1], data[0] } };

    return value.value;
  }
}

long64 InputStream::readLong64()
{
  const char* data = skip(sizeof(long64));

  if (order == Endian::NATIVE) {
    Endian::BytesToLong64 value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };

    return value.value;
  }
  else {
    Endian::BytesToLong64 value = {
      { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
    };

    return value.value;
  }
}

ulong64 InputStream::readULong64()
{
  const char* data = skip(sizeof(ulong64));

  if (order == Endian::NATIVE) {
    Endian::BytesToULong64 value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };

    return value.value;
  }
  else {
    Endian::BytesToULong64 value = {
      { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
    };

    return value.value;
  }
}

float InputStream::readFloat()
{
  const char* data = skip(sizeof(float));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

    return value.value;
  }
  else {
    Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

    return value.value;
  }
}

double InputStream::readDouble()
{
  const char* data = skip(sizeof(double));

  if (order == Endian::NATIVE) {
    Endian::BytesToDouble value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };

    return value.value;
  }
  else {
    Endian::BytesToDouble value = {
      { data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0] }
    };

    return value.value;
  }
}

const char* InputStream::readString()
{
  const char* begin = streamPos;

  while (streamPos < streamEnd && *streamPos != '\0') {
    ++streamPos;
  }
  if (streamPos == streamEnd) {
    OZ_ERROR("oz::OutputStream: Buffer overrun while looking for the end of a string.");
  }

  ++streamPos;
  return begin;
}

Vec3 InputStream::readVec3()
{
  Vec3 v;
  readFloats(v, 3);
  return v;
}

Vec4 InputStream::readVec4()
{
  Vec4 v;
  readFloats(v, 4);
  return v;
}

Point InputStream::readPoint()
{
  Point p;
  readFloats(p, 3);
  return p;
}

Plane InputStream::readPlane()
{
  Plane p;
  readFloats(p, 4);
  return p;
}

Quat InputStream::readQuat()
{
  Quat q;
  readFloats(q, 4);
  return q;
}

Mat3 InputStream::readMat3()
{
  Mat3 m;
  readFloats(m, 9);
  return m;
}

Mat4 InputStream::readMat4()
{
  Mat4 m;
  readFloats(m, 16);
  return m;
}

void InputStream::readBitset(ulong* bitset, int bits)
{
  int unitBits    = sizeof(ulong) * 8;
  int unit64Bits  = sizeof(ulong64) * 8;
  int unitCount   = (bits + unitBits - 1) / unitBits;
  int unit64Count = (bits + unit64Bits - 1) / unit64Bits;

  const char* data = skip(unit64Count * 8);

  for (int i = 0; i < unitCount; ++i) {
#if OZ_SIZEOF_LONG == 4
    Endian::BytesToUInt value = { { data[0], data[1], data[2], data[3] } };
#else
    Endian::BytesToULong64 value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };
#endif
    *bitset = value.value;

    bitset += 1;
    data   += sizeof(ulong);
  }
}

String InputStream::readLine()
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

}
