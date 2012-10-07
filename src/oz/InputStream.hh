/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/InputStream.hh
 *
 * InputStream class.
 */

#pragma once

#include "String.hh"
#include "Mat44.hh"
#include "System.hh"
#include "Endian.hh"

namespace oz
{

/**
 * Fixed-size read-only stream.
 */
class InputStream
{
  friend class OutputStream;
  friend class BufferStream;

  private:

    const char*   streamPos;   ///< Current position.
    const char*   streamBegin; ///< Beginning.
    const char*   streamEnd;   ///< End.
    Endian::Order order;       ///< Stream byte order.

  public:

    /**
     * Create a stream with the given beginning and the end.
     */
    explicit InputStream( const char* start = nullptr, const char* end = nullptr,
                          Endian::Order order_ = Endian::NATIVE ) :
      streamPos( start ), streamBegin( start ), streamEnd( end ), order( order_ )
    {}

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      hard_assert( streamPos <= streamEnd );

      return int( streamPos - streamBegin );
    }

    /**
     * Length of the stream.
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
     * True iff there is still some bytes left on the stream.
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
      hard_assert( streamBegin <= streamPos && streamPos <= streamEnd );

      return streamPos;
    }

    /**
     * %Set the curent position.
     */
    OZ_ALWAYS_INLINE
    void setPos( const char* newPos )
    {
      hard_assert( streamBegin <= newPos && newPos <= streamEnd );

      streamPos = newPos;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void reset()
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
     * Skip `count` bytes.
     *
     * @return Constant pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    const char* forward( int count )
    {
      const char* oldPos = streamPos;
      streamPos += count;

      if( streamPos > streamEnd ) {
        OZ_ERROR( "Buffer overrun for %d B during a read of %d B",
                  int( streamPos - streamEnd ), count );
      }
      return oldPos;
    }

    /**
     * Read boolean.
     */
    OZ_ALWAYS_INLINE
    bool readBool()
    {
      const bool* data = reinterpret_cast<const bool*>( forward( sizeof( bool ) ) );
      return *data;
    }

    /**
     * Read character.
     */
    OZ_ALWAYS_INLINE
    char readChar()
    {
      const char* data = reinterpret_cast<const char*>( forward( sizeof( char ) ) );
      return *data;
    }

    /**
     * Read an array of characters.
     */
    OZ_ALWAYS_INLINE
    void readChars( char* array, int count )
    {
      const char* data = reinterpret_cast<const char*>( forward( count * int( sizeof( char ) ) ) );
      aCopy<char>( array, data, count );
    }

    /**
     * Read byte.
     */
    OZ_ALWAYS_INLINE
    byte readByte()
    {
      const byte* data = reinterpret_cast<const byte*>( forward( sizeof( byte ) ) );
      return *data;
    }

    /**
     * Read unsigned byte.
     */
    OZ_ALWAYS_INLINE
    ubyte readUByte()
    {
      const ubyte* data = reinterpret_cast<const ubyte*>( forward( sizeof( ubyte ) ) );
      return *data;
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const short* data = reinterpret_cast<const short*>( forward( sizeof( short ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap16( *data );
      }
    }

    /**
     * Read unsigned short integer.
     */
    OZ_ALWAYS_INLINE
    ushort readUShort()
    {
      const ushort* data = reinterpret_cast<const ushort*>( forward( sizeof( ushort ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap16( *data );
      }
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap32( *data );
      }
    }

    /**
     * Read unsigned integer.
     */
    OZ_ALWAYS_INLINE
    uint readUInt()
    {
      const uint* data = reinterpret_cast<const uint*>( forward( sizeof( uint ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap32( *data );
      }
    }

    /**
     * Read 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    long64 readLong64()
    {
      const long64* data = reinterpret_cast<const long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap64( *data );
      }
    }

    /**
     * Read unsigned 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    ulong64 readULong64()
    {
      const ulong64* data = reinterpret_cast<const ulong64*>( forward( sizeof( ulong64 ) ) );

      if( order == Endian::NATIVE ) {
        return *data;
      }
      else {
        return Endian::bswap64( *data );
      }
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        return Math::fromBits( *data );
      }
      else {
        return Math::fromBits( Endian::bswap32( *data ) );
      }
    }

    /**
     * Read double.
     */
    OZ_ALWAYS_INLINE
    double readDouble()
    {
      union BitsToDouble
      {
        long64 b;
        double d;
      };

      const long64* data = reinterpret_cast<const long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        BitsToDouble bd = { *data };
        return bd.d;
      }
      else {
        BitsToDouble bd = { Endian::bswap64( *data ) };
        return bd.d;
      }
    }

    /**
     * Read string.
     */
    const char* readString()
    {
      const char* begin = streamPos;

      while( streamPos < streamEnd && *streamPos != '\0' ) {
        ++streamPos;
      }
      if( streamPos == streamEnd ) {
        OZ_ERROR( "End of buffer reached while looking for the end of a string." );
      }

      ++streamPos;
      return begin;
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        return Vec3( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ) );
      }
      else {
        return Vec3( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ) );
      }
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Vec4( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ),
                     Math::fromBits( data[3] ) );
      }
      else {
        return Vec4( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ),
                     Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point readPoint()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        return Point( Math::fromBits( data[0] ),
                      Math::fromBits( data[1] ),
                      Math::fromBits( data[2] ) );
      }
      else {
        return Point( Math::fromBits( Endian::bswap32( data[0] ) ),
                      Math::fromBits( Endian::bswap32( data[1] ) ),
                      Math::fromBits( Endian::bswap32( data[2] ) ) );
      }
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Plane( Math::fromBits( data[0] ),
                      Math::fromBits( data[1] ),
                      Math::fromBits( data[2] ),
                      Math::fromBits( data[3] ) );
      }
      else {
        return Plane( Math::fromBits( Endian::bswap32( data[0] ) ),
                      Math::fromBits( Endian::bswap32( data[1] ) ),
                      Math::fromBits( Endian::bswap32( data[2] ) ),
                      Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        return Quat( Math::fromBits( data[0] ),
                     Math::fromBits( data[1] ),
                     Math::fromBits( data[2] ),
                     Math::fromBits( data[3] ) );
      }
      else {
        return Quat( Math::fromBits( Endian::bswap32( data[0] ) ),
                     Math::fromBits( Endian::bswap32( data[1] ) ),
                     Math::fromBits( Endian::bswap32( data[2] ) ),
                     Math::fromBits( Endian::bswap32( data[3] ) ) );
      }
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[16] ) ) );

      if( order == Endian::NATIVE ) {
        return Mat44( Math::fromBits( data[ 0] ),
                      Math::fromBits( data[ 1] ),
                      Math::fromBits( data[ 2] ),
                      Math::fromBits( data[ 3] ),
                      Math::fromBits( data[ 4] ),
                      Math::fromBits( data[ 5] ),
                      Math::fromBits( data[ 6] ),
                      Math::fromBits( data[ 7] ),
                      Math::fromBits( data[ 8] ),
                      Math::fromBits( data[ 9] ),
                      Math::fromBits( data[10] ),
                      Math::fromBits( data[11] ),
                      Math::fromBits( data[12] ),
                      Math::fromBits( data[13] ),
                      Math::fromBits( data[14] ),
                      Math::fromBits( data[15] ) );
      }
      else {
        return Mat44( Math::fromBits( Endian::bswap32( data[ 0] ) ),
                      Math::fromBits( Endian::bswap32( data[ 1] ) ),
                      Math::fromBits( Endian::bswap32( data[ 2] ) ),
                      Math::fromBits( Endian::bswap32( data[ 3] ) ),
                      Math::fromBits( Endian::bswap32( data[ 4] ) ),
                      Math::fromBits( Endian::bswap32( data[ 5] ) ),
                      Math::fromBits( Endian::bswap32( data[ 6] ) ),
                      Math::fromBits( Endian::bswap32( data[ 7] ) ),
                      Math::fromBits( Endian::bswap32( data[ 8] ) ),
                      Math::fromBits( Endian::bswap32( data[ 9] ) ),
                      Math::fromBits( Endian::bswap32( data[10] ) ),
                      Math::fromBits( Endian::bswap32( data[11] ) ),
                      Math::fromBits( Endian::bswap32( data[12] ) ),
                      Math::fromBits( Endian::bswap32( data[13] ) ),
                      Math::fromBits( Endian::bswap32( data[14] ) ),
                      Math::fromBits( Endian::bswap32( data[15] ) ) );
      }
    }

    /**
     * Read line from a text file.
     *
     * Line delimiting character is not part of the returned string.
     */
    String readLine()
    {
      const char* begin = streamPos;

      while( streamPos < streamEnd && *streamPos != '\n' && *streamPos != '\r' ) {
        ++streamPos;
      }

      int length = int( streamPos - begin );

      streamPos += ( streamPos < streamEnd ) +
                   ( streamPos < streamEnd - 1 && streamPos[0] == '\r' && streamPos[1] == '\n' );
      return String( begin, length );
    }

};

}
