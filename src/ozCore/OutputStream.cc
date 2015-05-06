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
 * @file ozCore/OutputStream.cc
 */

#include "OutputStream.hh"

#include "System.hh"

#include <cstring>

namespace oz
{

OZ_HIDDEN
void OutputStream::writeFloats(const float* values, int count)
{
  char* data = skip(count * sizeof(float));

  if (order == Endian::NATIVE) {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[0];
      data[1] = value.data[1];
      data[2] = value.data[2];
      data[3] = value.data[3];
    }
  }
  else {
    for (int i = 0; i < count; ++i, data += 4, ++values) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[3];
      data[1] = value.data[2];
      data[2] = value.data[1];
      data[3] = value.data[0];
    }
  }
}

OutputStream::OutputStream(char* start, const char* end, Endian::Order order) :
  InputStream(start, end, order)
{}

OutputStream::OutputStream(Buffer& buffer, Endian::Order order) :
  InputStream(buffer.begin(), buffer.end(), order)
{}

OutputStream::OutputStream(Buffer* backBuffer_, Endian::Order order) :
  InputStream(backBuffer_->begin(), backBuffer_->end(), order), backBuffer(backBuffer_)
{}

char* OutputStream::skip(int count)
{
  char* oldPos = streamPos;
  streamPos += count;

  if (streamPos > streamEnd) {
    if (backBuffer == nullptr) {
      OZ_ERROR("oz::OutputStream: Overrun for %d B during a read or write of %d B",
               int(streamPos - streamEnd), count);
    }
    else {
      int newLength = int(streamPos - streamBegin);

      backBuffer->resize(newLength);

      streamBegin = backBuffer->begin();
      streamEnd   = backBuffer->end();
      streamPos   = streamBegin + newLength;
      oldPos      = streamPos - count;
    }
  }
  return oldPos;
}

void OutputStream::writeBool(bool b)
{
  char* data = skip(sizeof(bool));
  *data = char(b);
}

void OutputStream::writeChar(char c)
{
  char* data = skip(sizeof(char));
  *data = char(c);
}

void OutputStream::writeChars(const char* array, int count)
{
  char* data = skip(count * sizeof(char));
  memcpy(data, array, count);
}

void OutputStream::writeByte(byte b)
{
  char* data = skip(sizeof(byte));
  *data = char(b);
}

void OutputStream::writeUByte(ubyte b)
{
  char* data = skip(sizeof(ubyte));
  *data = char(b);
}

void OutputStream::writeShort(short s)
{
  char* data = skip(sizeof(short));

  Endian::ShortToBytes value = { s };

  if (order == Endian::NATIVE) {
    data[0] = value.data[0];
    data[1] = value.data[1];
  }
  else {
    data[0] = value.data[1];
    data[1] = value.data[0];
  }
}

void OutputStream::writeUShort(ushort s)
{
  char* data = skip(sizeof(ushort));

  Endian::UShortToBytes value = { s };

  if (order == Endian::NATIVE) {
    data[0] = value.data[0];
    data[1] = value.data[1];
  }
  else {
    data[0] = value.data[1];
    data[1] = value.data[0];
  }
}

void OutputStream::writeInt(int i)
{
  char* data = skip(sizeof(int));

  Endian::IntToBytes value = { i };

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

void OutputStream::writeUInt(uint i)
{
  char* data = skip(sizeof(uint));

  Endian::UIntToBytes value = { i };

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

void OutputStream::writeLong64(long64 l)
{
  char* data = skip(sizeof(long64));

  Endian::Long64ToBytes value = { l };

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

void OutputStream::writeULong64(ulong64 l)
{
  char* data = skip(sizeof(ulong64));

  Endian::ULong64ToBytes value = { l };

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

void OutputStream::writeFloat(float f)
{
  char* data = skip(sizeof(float));

  Endian::FloatToBytes value = { f };

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

void OutputStream::writeDouble(double d)
{
  char* data = skip(sizeof(double));

  Endian::DoubleToBytes value = { d };

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

void OutputStream::writeString(const String& s)
{
  int   size = s.length() + 1;
  char* data = skip(size);

  memcpy(data, s.cstr(), size);
}

void OutputStream::writeString(const char* s)
{
  int   size = String::length(s) + 1;
  char* data = skip(size);

  memcpy(data, s, size);
}

void OutputStream::writeVec3(const Vec3& v)
{
  writeFloats(v, 3);
}

void OutputStream::writeVec4(const Vec4& v)
{
  writeFloats(v, 4);
}

void OutputStream::writePoint(const Point& p)
{
  writeFloats(p, 3);
}

void OutputStream::writePlane(const Plane& p)
{
  writeFloats(p, 4);
}

void OutputStream::writeQuat(const Quat& q)
{
  writeFloats(q, 4);
}

void OutputStream::writeMat3(const Mat3& m)
{
  writeFloats(m, 9);
}

void OutputStream::writeMat4(const Mat4& m)
{
  writeFloats(m, 16);
}

void OutputStream::writeBitset(const ulong* bitset, int nBits)
{
  int unitBits    = sizeof(ulong) * 8;
  int unit64Bits  = sizeof(ulong64) * 8;
  int unitCount   = (nBits + unitBits - 1) / unitBits;
  int unit64Count = (nBits + unit64Bits - 1) / unit64Bits;

  char* data = skip(unit64Count * 8);

  for (int i = 0; i < unitCount; ++i) {
#if OZ_SIZEOF_LONG == 4
    Endian::UIntToBytes value = { *reinterpret_cast<const ulong*>(bitset) };

    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
#else
    Endian::ULong64ToBytes value = { *reinterpret_cast<const ulong*>(bitset) };

    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
#endif

    bitset += 1;
    data   += sizeof(ulong);
  }

#if OZ_SIZEOF_LONG == 4
  if (unitCount % 2 != 0) {
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
  }
#endif
}

void OutputStream::writeLine(const String& s)
{
  int   length = s.length();
  char* data   = skip(length + 1);

  memcpy(data, s, length);
  data[length] = '\n';
}

void OutputStream::writeLine(const char* s)
{
  int   length = String::length(s);
  char* data   = skip(length + 1);

  memcpy(data, s, length);
  data[length] = '\n';
}

void OutputStream::free()
{
  if (backBuffer) {
    backBuffer->resize(0, true);

    streamPos   = nullptr;
    streamBegin = nullptr;
    streamEnd   = nullptr;
  }
}

}
