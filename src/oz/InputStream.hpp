/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/InputStream.hpp
 */

#pragma once

#include "Exception.hpp"
#include "Plane.hpp"
#include "Mat44.hpp"
#include "Endian.hpp"

namespace oz
{

/**
 * Read-only stream.
 *
 * @ingroup oz
 */
class InputStream
{
  private:

    const char* pos;   ///< Current position.
    const char* start; ///< Beginning.
    const char* end;   ///< End.

  public:

    /**
     * Create a stream with the given beginning and the end.
     */
    OZ_ALWAYS_INLINE
    explicit InputStream( const char* start_, const char* end_ ) :
        pos( start_ ), start( start_ ), end( end_ )
    {}

    /**
     * Number of bytes from the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return int( size_t( pos - start ) );
    }

    /**
     * Length of the stream.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return int( size_t( end - start ) );
    }

    /**
     * Number of bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    int available() const
    {
      return int( size_t( end - pos ) );
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
        throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                         " bytes during a read of " + String( count ) + " bytes" );
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
      const char* data = reinterpret_cast<const char*>( forward( count ) );
      aCopy( array, data, count );
    }

    /**
     * Read short integer.
     */
    OZ_ALWAYS_INLINE
    short readShort()
    {
      const short* data = reinterpret_cast<const short*>( forward( sizeof( short ) ) );
      return Endian::shuffle16( *data );
    }

    /**
     * Read integer.
     */
    OZ_ALWAYS_INLINE
    int readInt()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );
      return Endian::shuffle32( *data );
    }

    /**
     * Read 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    long64 readLong64()
    {
      const long64* data = reinterpret_cast<const long64*>( forward( sizeof( long64 ) ) );
      return Endian::shuffle64( *data );
    }

    /**
     * Read float.
     */
    OZ_ALWAYS_INLINE
    float readFloat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( int ) ) );
      return Math::fromBits( Endian::shuffle32( *data ) );
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
      BitsToDouble bd = { Endian::shuffle64( *data ) };
      return bd.d;
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
        throw Exception( "End of buffer reached while looking for end of string." );
      }
      return String( forward( length + 1 ), length );
    }

    /**
     * Read string, padded with null character till the length of <tt>size</tt> bytes.
     */
    OZ_ALWAYS_INLINE
    String readPaddedString( int size )
    {
      const char* data = reinterpret_cast<const char*>( forward( size ) );
      if( data[size - 1] != '\0' ) {
        throw Exception( "Padded string does not end with NULL character." );
      }
      return String( data );
    }

    /**
     * Read 3D vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 readVec3()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );
      return Vec3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ) );
    }

    /**
     * Read 4-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 readVec4()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );
      return Vec4( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ),
                   Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point3 readPoint3()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );
      return Point3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ) );
    }

    /**
     * Read 3D plane.
     */
    OZ_ALWAYS_INLINE
    Plane readPlane()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );
      return Plane( Math::fromBits( Endian::shuffle32( data[0] ) ),
                    Math::fromBits( Endian::shuffle32( data[1] ) ),
                    Math::fromBits( Endian::shuffle32( data[2] ) ),
                    Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat readQuat()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[4] ) ) );
      return Quat( Math::fromBits( Endian::shuffle32( data[0] ) ),
                   Math::fromBits( Endian::shuffle32( data[1] ) ),
                   Math::fromBits( Endian::shuffle32( data[2] ) ),
                   Math::fromBits( Endian::shuffle32( data[3] ) ) );
    }

    /**
     * Read 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 readMat44()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[16] ) ) );
      return Mat44( Math::fromBits( Endian::shuffle32( data[ 0] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 1] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 2] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 3] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 4] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 5] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 6] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 7] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 8] ) ),
                    Math::fromBits( Endian::shuffle32( data[ 9] ) ),
                    Math::fromBits( Endian::shuffle32( data[10] ) ),
                    Math::fromBits( Endian::shuffle32( data[11] ) ),
                    Math::fromBits( Endian::shuffle32( data[12] ) ),
                    Math::fromBits( Endian::shuffle32( data[13] ) ),
                    Math::fromBits( Endian::shuffle32( data[14] ) ),
                    Math::fromBits( Endian::shuffle32( data[15] ) ) );
    }

};

}
