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
 * @file ozCore/InputStream.hh
 *
 * `InputStream` class.
 */

#pragma once

#include "String.hh"
#include "Mat4.hh"
#include "Endian.hh"

namespace oz
{

/**
 * Fixed-size read-only stream.
 *
 * @sa `oz::OutputStream`
 */
class InputStream
{
protected:

  char*         streamPos   = nullptr;        ///< Current position.
  char*         streamBegin = nullptr;        ///< Beginning.
  const char*   streamEnd   = nullptr;        ///< End.
  Endian::Order order       = Endian::NATIVE; ///< Stream byte order.

protected:

  /**
   * Internal constructor for `OutputStream`.
   */
  explicit InputStream(char* pos, char* start, const char* end, Endian::Order order_);

  /**
   * Read consecutive floats. Helper function for readVec3(), readVec4() etc.
   */
  void readFloats(float* values, int count);

public:

  /**
   * Empty stream.
   */
  InputStream() = default;

  /**
   * Create a stream for reading a given memory range.
   */
  explicit InputStream(const char* start, const char* end, Endian::Order order_ = Endian::NATIVE);

  /**
   * Length of the stream.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    hard_assert(streamPos <= streamEnd);

    return int(streamEnd - streamBegin);
  }

  /**
   * Number of bytes left before end of the stream is reached.
   */
  OZ_ALWAYS_INLINE
  int available() const
  {
    hard_assert(streamPos <= streamEnd);

    return int(streamEnd - streamPos);
  }

  /**
   * True iff there is still some bytes left on the stream.
   */
  OZ_ALWAYS_INLINE
  bool isAvailable() const
  {
    hard_assert(streamPos <= streamEnd);

    return streamPos != streamEnd;
  }

  /**
   * Constant pointer to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* begin() const
  {
    return streamBegin;
  }

  /**
   * Constant pointer to the eng of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* end() const
  {
    return streamEnd;
  }

  /**
   * Pointer to the current position.
   */
  OZ_ALWAYS_INLINE
  const char* pos() const
  {
    hard_assert(streamBegin <= streamPos && streamPos <= streamEnd);

    return streamPos;
  }

  /**
   * Offset of the current position from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  int tell() const
  {
    hard_assert(streamPos <= streamEnd);

    return int(streamPos - streamBegin);
  }

  /**
   * %Set stream position.
   */
  void set(const char* newPos);

  /**
   * %Set stream position relative to the beginning of the stream.
   */
  void seek(int offset);

  /**
   * Rewind current position to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  void rewind()
  {
    streamPos = streamBegin;
  }

  /**
   * Get byte order.
   */
  OZ_ALWAYS_INLINE
  Endian::Order endian() const
  {
    return order;
  }

  /**
   * %Set byte order.
   */
  OZ_ALWAYS_INLINE
  void setEndian(Endian::Order order_)
  {
    order = order_;
  }

  /**
   * Constant reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char& operator [] (int i) const
  {
    hard_assert(uint(i) < uint(streamEnd - streamBegin));

    return streamBegin[i];
  }

  /**
   * Skip `count` bytes.
   *
   * @return Constant pointer to the beginning of the skipped bytes.
   */
  const char* forward(int count);

  /**
   * Read boolean.
   */
  bool readBool();

  /**
   * Read character.
   */
  char readChar();

  /**
   * Read an array of characters.
   */
  void readChars(char* array, int count);

  /**
   * Read byte.
   */
  byte readByte();

  /**
   * Read unsigned byte.
   */
  ubyte readUByte();

  /**
   * Read short integer.
   */
  short readShort();

  /**
   * Read unsigned short integer.
   */
  ushort readUShort();

  /**
   * Read integer.
   */
  int readInt();

  /**
   * Read unsigned integer.
   */
  uint readUInt();

  /**
   * Read 64-bit integer.
   */
  long64 readLong64();

  /**
   * Read unsigned 64-bit integer.
   */
  ulong64 readULong64();

  /**
   * Read float.
   */
  float readFloat();

  /**
   * Read double.
   */
  double readDouble();

  /**
   * Read string.
   */
  const char* readString();

  /**
   * Read 3D vector.
   */
  Vec3 readVec3();

  /**
   * Read 4-component vector.
   */
  Vec4 readVec4();

  /**
   * Read 3D point.
   */
  Point readPoint();

  /**
   * Read 3D plane.
   */
  Plane readPlane();

  /**
   * Read quaternion.
   */
  Quat readQuat();

  /**
   * Read 3x3 matrix.
   */
  Mat3 readMat3();

  /**
   * Read 4x4 matrix.
   */
  Mat4 readMat4();

  /**
   * Fill bitset with data from a stream.
   */
  void readBitset(ulong* bitset, int bits);

  /**
   * Read a line.
   *
   * Line delimiter is read but not included in the returned string.
   */
  String readLine();

};

}
