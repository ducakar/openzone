/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

  /// Writing to stream is enabled.
  static constexpr int WRITABLE = 0x1;

  /// Stream has its own internal buffer.
  static constexpr int BUFFERED = 0x2;

private:

  char*         pos_   = nullptr;        ///< Current position.
  char*         begin_ = nullptr;        ///< Beginning.
  char*         end_   = nullptr;        ///< End.
  int           flags_ = 0;              ///< Feature bitfield.
  Endian::Order order_ = Endian::NATIVE; ///< Stream byte order.

private:

  /**
   * Read consecutive floats. Helper function for read<Vec3>(), readVec4() etc.
   */
  void readFloats(float* values, int count);

  /**
   * Write consecutive floats. Helper function for write<Vec3>(), write<Vec4>() etc.
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
  Stream(Stream&& other) noexcept;

  /**
   * Move operator, moves internal buffer.
   */
  Stream& operator=(Stream&& other) noexcept;

  /**
   * Swap instances.
   */
  friend void swap(Stream& a, Stream& b) noexcept;

  /**
   * Constant pointer to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* begin() const noexcept
  {
    return begin_;
  }

  /**
   * Pointer to the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char* begin() noexcept
  {
    return begin_;
  }

  /**
   * Constant pointer to the end of the stream.
   */
  OZ_ALWAYS_INLINE
  const char* end() const noexcept
  {
    return end_;
  }

  /**
   * Pointer to the end of the stream.
   */
  OZ_ALWAYS_INLINE
  char* end() noexcept
  {
    return end_;
  }

  /**
   * Pointer to the current stream position.
   */
  OZ_ALWAYS_INLINE
  const char* pos() const noexcept
  {
    return pos_;
  }

  /**
   * Pointer to the current stream position.
   */
  OZ_ALWAYS_INLINE
  char* pos() noexcept
  {
    return pos_;
  }

  /**
   * Position offset from the beginning.
   */
  OZ_ALWAYS_INLINE
  int size() const noexcept
  {
    OZ_ASSERT(pos_ <= end_);

    return int(pos_ - begin_);
  }

  /**
   * Length of the stream.
   */
  OZ_ALWAYS_INLINE
  int capacity() const noexcept
  {
    OZ_ASSERT(pos_ <= end_);

    return int(end_ - begin_);
  }

  /**
   * Number of bytes left before end of the stream is reached.
   */
  OZ_ALWAYS_INLINE
  int available() const noexcept
  {
    OZ_ASSERT(pos_ <= end_);

    return int(end_ - pos_);
  }

  /**
   * True iff writable.
   */
  OZ_ALWAYS_INLINE
  bool isWritable() const
  {
    return flags_ & WRITABLE;
  }

  /**
   * True iff it has an internal buffer.
   */
  OZ_ALWAYS_INLINE
  bool isBuffered() const
  {
    return flags_ & BUFFERED;
  }

  /**
   * %Endian order.
   */
  OZ_ALWAYS_INLINE
  Endian::Order order() const
  {
    return order_;
  }

  /**
   * %Set endian order.
   */
  OZ_ALWAYS_INLINE
  void setOrder(Endian::Order order)
  {
    order_ = order;
  }

  /**
   * Constant reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char& operator[](int i) const
  {
    OZ_ASSERT(uint(i) < uint(end_ - begin_));

    return begin_[i];
  }

  /**
   * Reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char& operator[](int i)
  {
    OZ_ASSERT(uint(i) < uint(end_ - begin_));

    return begin_[i];
  }

  /**
   * Offset of the current position from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  int tell() const
  {
    OZ_ASSERT(pos_ <= end_);

    return int(pos_ - begin_);
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
    pos_ = begin_;
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
   * Internal buffer is increased if necessary with growth factor 1.5 or to (at least) 4 KiB as the
   * initial allocation.
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
   * Read 16-bit integer.
   */
  int16 readInt16();

  /**
   * Write 16-bit integer.
   */
  void writeInt16(int16 s);

  /**
   * Read unsigned 16-bit integer.
   */
  uint16 readUInt16();

  /**
   * Write unsigned 16-bit integer.
   */
  void writeUInt16(uint16 s);

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
  int64 readInt64();

  /**
   * Write 64-bit integer.
   */
  void writeInt64(int64 l);

  /**
   * Read unsigned 64-bit integer.
   */
  uint64 readUInt64();

  /**
   * Write unsigned 64-bit integer.
   */
  void writeUInt64(uint64 l);

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
   * Read a line.
   *
   * Line delimiter is read but not included in the returned string.
   */
  String readLine();

  /**
   * Write UNIX newline.
   */
  void writeLine();

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const String& s);

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine(const char* s);

  /**
   * Fill bitset with data from a stream.
   */
  template <class BitsetType>
  void readBitset(BitsetType& b)
  {
    for (uint64& unit : b) {
      unit = readUInt64();
    }
  }

  /**
   * Write bitset data.
   */
  template <class BitsetType>
  void writeBitset(const BitsetType& b)
  {
    for (uint64 unit : b) {
      writeUInt64(unit);
    }
  }

  /**
   * Read a vector, quaternion, matrix etc.
   */
  template <class Vector>
  Vector read()
  {
    Vector v;
    readFloats(v, int(sizeof(Vector) / sizeof(float)));
    return v;
  }

  /**
   * Write a vector, quaternion, matrix etc.
   */
  template <class Vector>
  void write(const Vector& v)
  {
    writeFloats(v, int(sizeof(Vector) / sizeof(float)));
  }

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
