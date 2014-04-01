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
class OutputStream
{
private:

  /// Capacity is always a multiple of `GRANULARITY`.
  static const int GRANULARITY = 4096;

  char*         streamPos;   ///< Current position.
  char*         streamBegin; ///< Beginning.
  const char*   streamEnd;   ///< End.
  Endian::Order order;       ///< Stream byte order.
  bool          buffered;    ///< If stream writes to an internal buffer.

public:

  /**
   * Create a fixed-size stream for reading/writing a given memory range.
   */
  explicit OutputStream( char* start = nullptr, const char* end = nullptr,
                         Endian::Order order_ = Endian::NATIVE ) :
    streamPos( start ), streamBegin( start ), streamEnd( end ), order( order_ ), buffered( false )
  {}

  /**
   * Create a buffered stream with dynamically growing buffer.
   */
  explicit OutputStream( int size, Endian::Order order_ = Endian::NATIVE ) :
    streamPos( size == 0 ? nullptr : new char[size] ), streamBegin( streamPos ),
    streamEnd( streamBegin + size ), order( order_ ), buffered( true )
  {}

  /**
   * Destructor.
   */
  ~OutputStream()
  {
    if( buffered ) {
      delete[] streamBegin;
    }
  }

  /**
   * Copy constructor, copies buffer if source stream is buffered.
   */
  OutputStream( const OutputStream& os );

  /**
   * Move constructor, moves buffer if source stream is buffered.
   */
  OutputStream( OutputStream&& os );

  /**
   * Copy operator, copies buffer if source stream is buffered.
   *
   * Existing storage is reused if its size matches.
   */
  OutputStream& operator = ( const OutputStream& os );

  /**
   * Move operator, moves buffer if source stream is buffered.
   */
  OutputStream& operator = ( OutputStream&& os );

  /**
   * Create `InputStream` for reading this stream (position is not reset).
   */
  InputStream inputStream() const;

  /**
   * Iff stream uses internal buffer instead of given storage.
   */
  OZ_ALWAYS_INLINE
  bool isBuffered()
  {
    return buffered;
  }

  /**
   * Maximum length of the stream.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    hard_assert( streamPos <= streamEnd );

    return int( streamEnd - streamBegin );
  }

  /**
   * Number of bytes left before end of the stream is reached.
   */
  OZ_ALWAYS_INLINE
  int available() const
  {
    hard_assert( streamPos <= streamEnd );

    return int( streamEnd - streamPos );
  }

  /**
   * True iff there is still some bytes left before end of the stream is reached.
   */
  OZ_ALWAYS_INLINE
  bool isAvailable() const
  {
    hard_assert( streamPos <= streamEnd );

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
   * Pointer to the current position.
   */
  OZ_ALWAYS_INLINE
  const char* pos() const
  {
    hard_assert( streamBegin <= streamPos && streamPos <= streamEnd );

    return streamPos;
  }

  /**
   * Pointer to the current position.
   */
  OZ_ALWAYS_INLINE
  char* pos()
  {
    hard_assert( streamBegin <= streamPos && streamPos <= streamEnd );

    return streamPos;
  }

  /**
   * Offset of the current position from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  int tell() const
  {
    hard_assert( streamPos <= streamEnd );

    return int( streamPos - streamBegin );
  }

  /**
   * %Set stream position.
   */
  void set( char* newPos );

  /**
   * %Set stream position relative to the beginning of the stream.
   */
  void seek( int offset );

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
  void setEndian( Endian::Order order_ )
  {
    order = order_;
  }

  /**
   * Constant reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  const char& operator [] ( int i ) const
  {
    hard_assert( uint( i ) < uint( streamEnd - streamBegin ) );

    return streamBegin[i];
  }

  /**
   * Reference to the `i`-th byte from the beginning of the stream.
   */
  OZ_ALWAYS_INLINE
  char& operator [] ( int i )
  {
    hard_assert( uint( i ) < uint( streamEnd - streamBegin ) );

    return streamBegin[i];
  }

  /**
   * Move position pointer for `count` bytes forward.
   *
   * @return Pointer to the beginning of the skipped bytes.
   */
  char* forward( int count );

  /**
   * Read boolean.
   */
  bool readBool();

  /**
   * Write a boolean.
   */
  void writeBool( bool b );

  /**
   * Read character.
   */
  char readChar();

  /**
   * Write a character.
   */
  void writeChar( char c );

  /**
   * Read an array of characters.
   */
  void readChars( char* array, int count );

  /**
   * Write an array of characters.
   */
  void writeChars( const char* array, int count );

  /**
   * Read byte.
   */
  byte readByte();

  /**
   * Write byte.
   */
  void writeByte( byte b );

  /**
   * Read unsigned byte.
   */
  ubyte readUByte();

  /**
   * Write unsigned byte.
   */
  void writeUByte( ubyte b );

  /**
   * Read short integer.
   */
  short readShort();

  /**
   * Write short integer.
   */
  void writeShort( short s );

  /**
   * Read unsigned short integer.
   */
  ushort readUShort();

  /**
   * Write unsigned short integer.
   */
  void writeUShort( ushort s );

  /**
   * Read integer.
   */
  int readInt();

  /**
   * Write integer.
   */
  void writeInt( int i );

  /**
   * Read unsigned integer.
   */
  uint readUInt();

  /**
   * Write unsigned integer.
   */
  void writeUInt( uint i );

  /**
   * Read 64-bit integer.
   */
  long64 readLong64();

  /**
   * Write 64-bit integer.
   */
  void writeLong64( long64 l );

  /**
   * Read unsigned 64-bit integer.
   */
  ulong64 readULong64();

  /**
   * Write unsigned 64-bit integer.
   */
  void writeULong64( ulong64 l );

  /**
   * Read float.
   */
  float readFloat();

  /**
   * Write float.
   */
  void writeFloat( float f );

  /**
   * Read double.
   */
  double readDouble();

  /**
   * Write double.
   */
  void writeDouble( double d );

  /**
   * Read string.
   */
  const char* readString();

  /**
   * Write string.
   */
  void writeString( const String& s );

  /**
   * Write C string.
   */
  void writeString( const char* s );

  /**
   * Read 3D vector.
   */
  Vec3 readVec3();

  /**
   * Write 3D vector.
   */
  void writeVec3( const Vec3& v );

  /**
   * Read 4-component vector.
   */
  Vec4 readVec4();

  /**
   * Write 4-component vector.
   */
  void writeVec4( const Vec4& v );

  /**
   * Read 3D point.
   */
  Point readPoint();

  /**
   * Write 3D point.
   */
  void writePoint( const Point& p );

  /**
   * Read 3D plane.
   */
  Plane readPlane();

  /**
   * Write 3D plane.
   */
  void writePlane( const Plane& p );

  /**
   * Read quaternion.
   */
  Quat readQuat();

  /**
   * Write quaternion.
   */
  void writeQuat( const Quat& q );

  /**
   * Read 3x3 matrix.
   */
  Mat3 readMat3();

  /**
   * Write 3x3 matrix.
   */
  void writeMat3( const Mat3& m );

  /**
   * Read 4x4 matrix.
   */
  Mat4 readMat4();

  /**
   * Write 4x4 matrix.
   */
  void writeMat4( const Mat4& m );

  /**
   * Fill bitset with data from a stream.
   */
  void readBitset( ulong* bitset, int nBits );

  /**
   * Write bitset data.
   */
  void writeBitset( const ulong* bitset, int nBits );

  /**
   * Read a line.
   *
   * Line delimiter is read but not included in the returned string.
   */
  String readLine();

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine( const String& s );

  /**
   * Write a line replacing terminating null byte with UNIX newline.
   */
  void writeLine( const char* s );

  /**
   * Deallocate internal buffer if stream is buffered.
   */
  void deallocate();

};

}
