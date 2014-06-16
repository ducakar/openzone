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

InputStream::InputStream(const char* start, const char* end, Endian::Order order_) :
  streamPos(const_cast<char*>(start)), streamBegin(const_cast<char*>(start)), streamEnd(end),
  order(order_)
{}

void InputStream::set(const char* newPos)
{
  if (newPos < streamBegin || streamEnd < newPos) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during setting stream position",
             newPos < streamBegin ? int(newPos - streamBegin) : int(newPos - streamEnd));
  }

  streamPos = const_cast<char*>(newPos);
}

void InputStream::seek(int offset)
{
  if (offset < 0 || int(streamEnd - streamBegin) < offset) {
    OZ_ERROR("oz::InputStream: Overrun for %d B during stream seek",
             offset < 0 ? offset : offset - int(streamEnd - streamBegin));
  }

  streamPos = streamBegin + offset;
}

const char* InputStream::forward(int count)
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
  const char* data = forward(int(sizeof(bool)));
  return bool(*data);
}

char InputStream::readChar()
{
  const char* data = forward(int(sizeof(char)));
  return char(*data);
}

void InputStream::readChars(char* array, int count)
{
  const char* data = forward(count * int(sizeof(char)));
  mCopy(array, data, count);
}

byte InputStream::readByte()
{
  const char* data = forward(int(sizeof(byte)));
  return byte(*data);
}

ubyte InputStream::readUByte()
{
  const char* data = forward(int(sizeof(ubyte)));
  return ubyte(*data);
}

short InputStream::readShort()
{
  const char* data = forward(int(sizeof(short)));

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
  const char* data = forward(int(sizeof(ushort)));

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
  const char* data = forward(int(sizeof(int)));

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
  const char* data = forward(int(sizeof(uint)));

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
  const char* data = forward(int(sizeof(long64)));

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
  const char* data = forward(int(sizeof(ulong64)));

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
  const char* data = forward(int(sizeof(float)));

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
  const char* data = forward(int(sizeof(double)));

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
  const char* data = forward(int(sizeof(float[3])));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
    Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
    Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

    return Vec3(x.value, y.value, z.value);
  }
  else {
    Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
    Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
    Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

    return Vec3(x.value, y.value, z.value);
  }
}

Vec4 InputStream::readVec4()
{
  const char* data = forward(int(sizeof(float[4])));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
    Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
    Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
    Endian::BytesToFloat w = { { data[12], data[13], data[14], data[15] } };

    return Vec4(x.value, y.value, z.value, w.value);
  }
  else {
    Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
    Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
    Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
    Endian::BytesToFloat w = { { data[15], data[14], data[13], data[12] } };

    return Vec4(x.value, y.value, z.value, w.value);
  }
}

Point InputStream::readPoint()
{
  const char* data = forward(int(sizeof(float[3])));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
    Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
    Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };

    return Point(x.value, y.value, z.value);
  }
  else {
    Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
    Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
    Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };

    return Point(x.value, y.value, z.value);
  }
}

Plane InputStream::readPlane()
{
  const char* data = forward(int(sizeof(float[4])));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat nx = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
    Endian::BytesToFloat ny = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
    Endian::BytesToFloat nz = { { data[ 8], data[ 9], data[10], data[11] } };
    Endian::BytesToFloat d  = { { data[12], data[13], data[14], data[15] } };

    return Plane(nx.value, ny.value, nz.value, d.value);
  }
  else {
    Endian::BytesToFloat nx = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
    Endian::BytesToFloat ny = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
    Endian::BytesToFloat nz = { { data[11], data[10], data[ 9], data[ 8] } };
    Endian::BytesToFloat d  = { { data[15], data[14], data[13], data[12] } };

    return Plane(nx.value, ny.value, nz.value, d.value);
  }
}

Quat InputStream::readQuat()
{
  const char* data = forward(int(sizeof(float[4])));

  if (order == Endian::NATIVE) {
    Endian::BytesToFloat x = { { data[ 0], data[ 1], data[ 2], data[ 3] } };
    Endian::BytesToFloat y = { { data[ 4], data[ 5], data[ 6], data[ 7] } };
    Endian::BytesToFloat z = { { data[ 8], data[ 9], data[10], data[11] } };
    Endian::BytesToFloat w = { { data[12], data[13], data[14], data[15] } };

    return Quat(x.value, y.value, z.value, w.value);
  }
  else {
    Endian::BytesToFloat x = { { data[ 3], data[ 2], data[ 1], data[ 0] } };
    Endian::BytesToFloat y = { { data[ 7], data[ 6], data[ 5], data[ 4] } };
    Endian::BytesToFloat z = { { data[11], data[10], data[ 9], data[ 8] } };
    Endian::BytesToFloat w = { { data[15], data[14], data[13], data[12] } };

    return Quat(x.value, y.value, z.value, w.value);
  }
}

Mat3 InputStream::readMat3()
{
  const char* data = forward(int(sizeof(float[9])));

  Mat3 m;
  float* values = m;

  if (order == Endian::NATIVE) {
    for (int i = 0; i < 9; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for (int i = 0; i < 9; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }

  return m;
}

Mat4 InputStream::readMat4()
{
  const char* data = forward(int(sizeof(float[16])));

  Mat4 m;
  float* values = m;

  if (order == Endian::NATIVE) {
    for (int i = 0; i < 16; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for (int i = 0; i < 16; ++i, data += 4, ++values) {
      Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }

  return m;
}

void InputStream::readBitset(ulong* bitset, int bits)
{
  int unitBits    = int(sizeof(ulong)) * 8;
  int unit64Bits  = int(sizeof(ulong64)) * 8;
  int unitCount   = (bits + unitBits - 1) / unitBits;
  int unit64Count = (bits + unit64Bits - 1) / unit64Bits;

  const char* data = forward(unit64Count * 8);

  for (int i = 0; i < unitCount; ++i) {
#if OZ_SIZEOF_LONG == 4
    Endian::BytesToUInt value = { { data[0], data[1], data[2], data[3] } };
#else
    Endian::BytesToULong64 value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };
#endif
    *bitset = ulong(value.value);

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
