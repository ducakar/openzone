/*
 * liboz - OpenZone core library.
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

#include "Exception.hh"
#include "String.hh"
#include "Plane.hh"
#include "Mat44.hh"
#include "Endian.hh"

namespace oz
{

/**
 * Fixed-size read-only stream.
 *
 * @ingroup oz
 */
class InputStream
{
  private:

    const char*   pos;   ///< Current position.
    const char*   start; ///< Beginning.
    const char*   end;   ///< End.
    Endian::Order order; ///< Stream byte order.

  public:

    /**
     * Create uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    InputStream() :
      pos( null ), start( null ), end( null ), order( Endian::NATIVE )
    {}

    /**
     * Create a stream with the given beginning and the end.
     */
    OZ_ALWAYS_INLINE
    explicit InputStream( const char* start_, const char* end_,
                          Endian::Order order_ = Endian::NATIVE ) :
      pos( start_ ), start( start_ ), end( end_ ), order( order_ )
    {}

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return int( pos - start );
    }

    /**
     * Length of the stream.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return int( end - start );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      return int( end - pos );
    }

    /**
     * True iff there is still some bytes left on the stream.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      return pos != end;
    }

    /**
     * Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char* begin() const
    {
      return start;
    }

    /**
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    const char* getPos() const
    {
      hard_assert( start <= pos && pos <= end );

      return pos;
    }

    /**
     * Set the curent position.
     */
    OZ_ALWAYS_INLINE
    void setPos( const char* newPos )
    {
      hard_assert( start <= newPos && newPos <= end );

      pos = newPos;
    }

    /**
     * Rewind current position to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    void reset()
    {
      pos = start;
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
     * Set byte order.
     */
    OZ_ALWAYS_INLINE
    void setEndian( Endian::Order order_ )
    {
      order = order_;
    }

    /**
     * Skip <tt>count</tt> bytes.
     *
     * @return Constant pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    const char* forward( int count )
    {
      const char* oldPos = pos;
      pos += count;

      if( pos > end ) {
        throw Exception( "Buffer overrun for %d B during a read of %d B",
                         int( pos + count - end ), count );
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
    OZ_ALWAYS_INLINE
    String readString()
    {
      int length = 0;
      while( pos + length < end && pos[length] != '\0' ) {
        ++length;
      }
      if( pos + length == end ) {
        throw Exception( "End of buffer reached while looking for the end of a string." );
      }

      return String( length, forward( length + 1 ) );
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

};

}
