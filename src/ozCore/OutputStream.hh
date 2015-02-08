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
 * @file ozCore/OutputStream.hh
 *
 * `OutputStream` class.
 */

#pragma once

#include "InputStream.hh"

namespace oz
{

/**
 * Fixed-size or buffered read/write stream.
 *
 * @sa `oz::InputStream`
 */
class OutputStream : public InputStream
{
private:

  /// Capacity is always a multiple of `GRANULARITY`.
  static const int GRANULARITY = 4096;

private:

  bool buffered; ///< Whether stream writes to an internal buffer.

public:

  using InputStream::begin;
  using InputStream::pos;
  using InputStream::operator [];

  /**
   * Create a fixed-size stream for reading/writing a given memory range.
   */
  explicit OutputStream(char* start = nullptr, const char* end = nullptr,
                        Endian::Order order = Endian::NATIVE);

  /**
   * Create a buffered stream with dynamically growing buffer.
   */
  explicit OutputStream(int size, Endian::Order order = Endian::NATIVE);

  /**
   * Destructor.
   */
  ~OutputStream();

  /**
   * Copy constructor, copies buffer if source stream is buffered.
   */
  OutputStream(const OutputStream& os);

  /**
   * Move constructor, moves buffer if source stream is buffered.
   */
  OutputStream(OutputStream&& os);

  /**
   * Copy operator, copies buffer if source stream is buffered.
   *
   * Existing storage is reused if its size matches.
   */
  OutputStream& operator = (const OutputStream& os);

  /**
   * Move operator, moves buffer if source stream is buffered.
   */
  OutputStream& operator = (OutputStream&& os);

  /**
   * Iff stream uses internal buffer instead of given storage.
   */
  OZ_ALWAYS_INLINE
  bool isBuffered()
  {
    return buffered;
  }

  /**
   * Pointer to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char* begin()
  {
    return streamBegin;
  }

  /**
   * Pointer to the current position.
   */
  OZ_ALWAYS_INLINE
  char* pos()
  {
    hard_assert(streamBegin <= streamPos && streamPos <= streamEnd);

    return streamPos;
  }

  /**
   * Reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char& operator [] (int i)
  {
    hard_assert(uint(i) < uint(streamEnd - streamBegin));

    return streamBegin[i];
  }

  /**
   * Move position pointer for `count` bytes forward.
   *
   * @return Pointer to the beginning of the skipped bytes.
   */
  char* forward(int count);

  /**
   * Write a boolean.
   */
  void writeBool(bool b);

  /**
   * Write a character.
   */
  void writeChar(char c);

  /**
   * Write an array of characters.
   */
  void writeChars(const char* array, int count);

  /**
   * Write byte.
   */
  void writeByte(byte b);

  /**
   * Write unsigned byte.
   */
  void writeUByte(ubyte b);

  /**
   * Write short integer.
   */
  void writeShort(short s);

  /**
   * Write unsigned short integer.
   */
  void writeUShort(ushort s);

  /**
   * Write integer.
   */
  void writeInt(int i);

  /**
   * Write unsigned integer.
   */
  void writeUInt(uint i);

  /**
   * Write 64-bit integer.
   */
  void writeLong64(long64 l);

  /**
   * Write unsigned 64-bit integer.
   */
  void writeULong64(ulong64 l);

  /**
   * Write float.
   */
  void writeFloat(float f);

  /**
   * Write double.
   */
  void writeDouble(double d);

  /**
   * Write string.
   */
  void writeString(const String& s);

  /**
   * Write C string.
   */
  void writeString(const char* s);

  /**
   * Write 3D vector.
   */
  void writeVec3(const Vec3& v);

  /**
   * Write 4-component vector.
   */
  void writeVec4(const Vec4& v);

  /**
   * Write 3D point.
   */
  void writePoint(const Point& p);

  /**
   * Write 3D plane.
   */
  void writePlane(const Plane& p);

  /**
   * Write quaternion.
   */
  void writeQuat(const Quat& q);

  /**
   * Write 3x3 matrix.
   */
  void writeMat3(const Mat3& m);

  /**
   * Write 4x4 matrix.
   */
  void writeMat4(const Mat4& m);

  /**
   * Write bitset data.
   */
  void writeBitset(const ulong* bitset, int nBits);

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const String& s);

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const char* s);

  /**
   * Deallocate internal buffer if stream is buffered.
   */
  void free();

};

}
