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
 * @file ozCore/Stream.hh
 *
 * `Stream` class.
 */

#pragma once

#include "Bitset.hh"
#include "String.hh"
#include "Mat4.hh"
#include "Endian.hh"

namespace oz
{

/**
 * IO stream.
 */
class Stream
{
private:

  /// Granularity used for during internal buffer resizing.
  static const int GRANULARITY = 4096;

  /// Writing to stream is enabled.
  static const int WRITABLE = 0x1;

  /// Stream has its own internal buffer.
  static const int BUFFERED = 0x2;

private:

  char* streamPos   = nullptr; ///< Current position.
  char* streamBegin = nullptr; ///< Beginning.
  char* streamEnd   = nullptr; ///< End.
  int   flags       = 0;       ///< Feature bitfield.

public:

  Endian::Order order = Endian::NATIVE; ///< Stream byte order.

private:

  /**
   * Read consecutive floats. Helper function for readVec3(), readVec4() etc.
   */
  void readFloats(float* values, int count);

  /**
   * Write consecutive floats. Helper function for writeVec3(), writeVec4() etc.
   */
  void writeFloats(const float* values, int count);

public:

  /**
   * Empty stream.
   */
  Stream() = default;

  /**
   * Create a fixed-size stream for reading a given memory range.
   */
  explicit Stream(const char* start, const char* end, Endian::Order order = Endian::NATIVE);

  /**
   * Create a fixed-size stream for reading/writing to a given memory range.
   */
  explicit Stream(char* start, char* end, Endian::Order order = Endian::NATIVE);

  /**
   * Create a buffered stream of given initial size.
   */
  explicit Stream(int size, Endian::Order order = Endian::NATIVE);

  /**
   * Free internal buffer if allocated.
   */
  ~Stream();

  /**
   * Move construction, moves internal buffer.
   */
  Stream(Stream&& s);

  /**
   * Move operator, moves internal buffer.
   */
  Stream& operator = (Stream&& s);

  /**
   * Constant pointer to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* begin() const
  {
    return streamBegin;
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
   * Constant pointer to the end of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* end() const
  {
    return streamEnd;
  }

  /**
   * Pointer to the end of the stream.
   */
  OZ_ALWAYS_INLINE
  char* end()
  {
    return streamEnd;
  }

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
   * True iff writable.
   */
  OZ_ALWAYS_INLINE
  bool isWritable() const
  {
    return flags & WRITABLE;
  }

  /**
   * True iff it has an internal buffer.
   */
  OZ_ALWAYS_INLINE
  bool isBuffered() const
  {
    return flags & BUFFERED;
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
   * Reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char& operator [] (int i)
  {
    hard_assert(uint(i) < uint(streamEnd - streamBegin));

    return streamBegin[i];
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
   * Resize internal buffer.
   *
   * The data after the current position is not preserved.
   */
  void resize(int newSize);

  /**
   * Skip `count` bytes.
   *
   * @return Constant pointer to the beginning of the skipped bytes.
   */
  const char* readSkip(int count);

  /**
   * Skip `count` bytes.
   *
   * Internal buffer is resized if necessary.
   *
   * @return Pointer to the beginning of the skipped bytes.
   */
  char* writeSkip(int count);

  /**
   * Deallocate underlaying buffer if the stream is buffered.
   */
  void free();

  /**
   * Read an array of bytes.
   */
  void read(char* array, int count);

  /**
   * Write an array of bytes.
   */
  void write(const char* array, int count);

  /**
   * Read boolean.
   */
  bool readBool();

  /**
   * Write boolean.
   */
  void writeBool(bool b);

  /**
   * Read character.
   */
  char readChar();

  /**
   * Write character.
   */
  void writeChar(char c);

  /**
   * Read byte.
   */
  byte readByte();

  /**
   * Write byte.
   */
  void writeByte(byte b);

  /**
   * Read unsigned byte.
   */
  ubyte readUByte();

  /**
   * Write unsigned byte.
   */
  void writeUByte(ubyte b);

  /**
   * Read short integer.
   */
  short readShort();

  /**
   * Write short integer.
   */
  void writeShort(short s);

  /**
   * Read unsigned short integer.
   */
  ushort readUShort();

  /**
   * Write unsigned short integer.
   */
  void writeUShort(ushort s);

  /**
   * Read integer.
   */
  int readInt();

  /**
   * Write integer.
   */
  void writeInt(int i);

  /**
   * Read unsigned integer.
   */
  uint readUInt();

  /**
   * Write unsigned integer.
   */
  void writeUInt(uint i);

  /**
   * Read 64-bit integer.
   */
  long64 readLong64();

  /**
   * Write 64-bit integer.
   */
  void writeLong64(long64 l);

  /**
   * Read unsigned 64-bit integer.
   */
  ulong64 readULong64();

  /**
   * Write unsigned 64-bit integer.
   */
  void writeULong64(ulong64 l);

  /**
   * Read float.
   */
  float readFloat();

  /**
   * Write float.
   */
  void writeFloat(float f);

  /**
   * Read double.
   */
  double readDouble();

  /**
   * Write double.
   */
  void writeDouble(double d);

  /**
   * Read string.
   */
  const char* readString();

  /**
   * Write string.
   */
  void writeString(const String& s);

  /**
   * Write C string.
   */
  void writeString(const char* s);

  /**
   * Read 3D vector.
   */
  Vec3 readVec3();

  /**
   * Write 3D vector.
   */
  void writeVec3(const Vec3& v);

  /**
   * Read 3D point.
   */
  Point readPoint();

  /**
   * Write 3D point.
   */
  void writePoint(const Point& p);

  /**
   * Read 3D plane.
   */
  Plane readPlane();

  /**
   * Write 3D plane.
   */
  void writePlane(const Plane& p);

  /**
   * Read 4-component vector.
   */
  Vec4 readVec4();

  /**
   * Write 4-component vector.
   */
  void writeVec4(const Vec4& v);

  /**
   * Read quaternion.
   */
  Quat readQuat();

  /**
   * Write quaternion.
   */
  void writeQuat(const Quat& q);

  /**
   * Read 3x3 matrix.
   */
  Mat3 readMat3();

  /**
   * Write 3x3 matrix.
   */
  void writeMat3(const Mat3& m);

  /**
   * Read 4x4 matrix.
   */
  Mat4 readMat4();

  /**
   * Write 4x4 matrix.
   */
  void writeMat4(const Mat4& m);

  /**
   * Fill bitset with data from a stream.
   */
  template <class BitsetType>
  void readBitset(BitsetType& b)
  {
    for (size_t& unit : b) {
#if __SIZEOF_SIZE_T__ == 4
      unit = readUInt();
#else
      unit = size_t(readULong64());
#endif
    }
  }

  /**
   * Write bitset data.
   */
  template <class BitsetType>
  void writeBitset(const BitsetType& b)
  {
    for (size_t unit : b) {
#if __SIZEOF_SIZE_T__ == 4
      writeUInt(unit);
#else
      writeULong64(unit);
#endif
    }
  }

  /**
   * Read a line.
   *
   * Line delimiter is read but not included in the returned string.
   */
  String readLine();

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const String& s);

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const char* s);

  /**
   * Compress using deflate (ZIP/GZip) algorithm.
   *
   * An empty stream is returned on an error.
   *
   * @param level 0 - none, 1 to 9 - fastest to best, -1 - default level.
   */
  Stream compress(int level = -1) const;

  /**
   * Decompress data compressed with deflate (ZIP/GZip) algorithm.
   *
   * An empty stream is returned on an error.
   */
  Stream decompress() const;

};

}
