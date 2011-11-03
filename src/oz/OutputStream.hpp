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
 * @file oz/OutputStream.hpp
 */

#pragma once

#include "Exception.hpp"
#include "Plane.hpp"
#include "Mat44.hpp"
#include "Endian.hpp"

namespace oz
{

/**
 * Write-only stream.
 *
 * @ingroup oz
 */
class OutputStream
{
  private:

    char*       pos;   ///< Current position.
    char*       start; ///< Beginning.
    const char* end;   ///< End.

  public:

    /**
     * Create a stream with the given beginning and the end.
     */
    OZ_ALWAYS_INLINE
    explicit OutputStream( char* start_, const char* end_ ) :
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
     * Maximum length of the stream.
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
     * True iff there is still some bytes left before end of the stream is reached.
     */
    OZ_ALWAYS_INLINE
    bool isAvailable() const
    {
      return pos != end;
    }

    /**
     * Constant Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    const char* begin() const
    {
      return start;
    }

    /**
     * Pointer to the beginning of the stream.
     */
    OZ_ALWAYS_INLINE
    char* begin()
    {
      return start;
    }

    /**
     * Pointer to the current position.
     */
    OZ_ALWAYS_INLINE
    char* getPos() const
    {
      hard_assert( start <= pos && pos <= end );

      return pos;
    }

    /**
     * Set the current position.
     */
    OZ_ALWAYS_INLINE
    void setPos( char* newPos )
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
     * Move position pointer for <tt>count</tt> bytes forward.
     *
     * @return Pointer to the beginning of the skipped bytes.
     */
    OZ_ALWAYS_INLINE
    char* forward( int count )
    {
      char* oldPos = pos;
      pos += count;

      if( pos > end ) {
        throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                         " bytes during a write of " + String( count ) + " bytes" );
      }
      return oldPos;
    }

    /**
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      bool* data = reinterpret_cast<bool*>( forward( sizeof( bool ) ) );
      *data = b;
    }

    /**
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = reinterpret_cast<char*>( forward( sizeof( char ) ) );
      *data = c;
    }

    /**
     * Write an array of characters.
     */
    OZ_ALWAYS_INLINE
    void writeChars( char *array, int count )
    {
      char* data = reinterpret_cast<char*>( forward( count * int( sizeof( char ) ) ) );
      aCopy( data, array, count );
    }

    /**
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      short* data = reinterpret_cast<short*>( forward( sizeof( short ) ) );
      *data = Endian::shuffle16( s );
    }

    /**
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( int ) ) );
      *data = Endian::shuffle32( i );
    }

    /**
     * Write 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeLong64( long64 l )
    {
      long64* data = reinterpret_cast<long64*>( forward( sizeof( long64 ) ) );
      *data = Endian::shuffle64( l );
    }

    /**
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float ) ) );
      *data = Endian::shuffle32( Math::toBits( f ) );
    }

    /**
     * Write double.
     */
    OZ_ALWAYS_INLINE
    void writeDouble( double d )
    {
      union DoubleToBits
      {
        double d;
        long64 b;
      };
      DoubleToBits db = { d };

      long64* data = reinterpret_cast<long64*>( forward( sizeof( double ) ) );
      *data = Endian::shuffle64( db.b );
    }

    /**
     * Write string.
     */
    OZ_ALWAYS_INLINE
    void writeString( const String& s )
    {
      int size = s.length() + 1;
      char* data = forward( size );

      aCopy( data, s.cstr(), size );
    }

    /**
     * Write C string.
     */
    OZ_ALWAYS_INLINE
    void writeString( const char* s )
    {
      int size = String::length( s ) + 1;
      char* data = forward( size );

      aCopy( data, s, size );
    }

    /**
     * Write string padded with null characters to the given number of bytes.
     */
    OZ_ALWAYS_INLINE
    void writePaddedString( const String& s, int size )
    {
      int length = s.length();
      char* data = forward( size );

      aCopy( data, s.cstr(), length );
      aSet( data + length, '\0', size - length );
    }

    /**
     * Write C string padded with null characters to the given number of bytes.
     */
    OZ_ALWAYS_INLINE
    void writePaddedString( const char* s, int size )
    {
      int length = String::length( s );
      char* data = forward( size );

      aCopy( data, s, length );
      aSet( data + length, '\0', size - length );
    }

    /**
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( v.x ) );
      data[1] = Endian::shuffle32( Math::toBits( v.y ) );
      data[2] = Endian::shuffle32( Math::toBits( v.z ) );
    }

    /**
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( v.x ) );
      data[1] = Endian::shuffle32( Math::toBits( v.y ) );
      data[2] = Endian::shuffle32( Math::toBits( v.z ) );
      data[3] = Endian::shuffle32( Math::toBits( v.w ) );
    }

    /**
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint3( const Point3& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( p.x ) );
      data[1] = Endian::shuffle32( Math::toBits( p.y ) );
      data[2] = Endian::shuffle32( Math::toBits( p.z ) );
    }

    /**
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( p.nx ) );
      data[1] = Endian::shuffle32( Math::toBits( p.ny ) );
      data[2] = Endian::shuffle32( Math::toBits( p.nz ) );
      data[3] = Endian::shuffle32( Math::toBits( p.d ) );
    }

    /**
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      data[0] = Endian::shuffle32( Math::toBits( q.x ) );
      data[1] = Endian::shuffle32( Math::toBits( q.y ) );
      data[2] = Endian::shuffle32( Math::toBits( q.z ) );
      data[3] = Endian::shuffle32( Math::toBits( q.w ) );
    }

    /**
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[16] ) ) );

      data[ 0] = Endian::shuffle32( Math::toBits( m.x.x ) );
      data[ 1] = Endian::shuffle32( Math::toBits( m.x.y ) );
      data[ 2] = Endian::shuffle32( Math::toBits( m.x.z ) );
      data[ 3] = Endian::shuffle32( Math::toBits( m.x.w ) );
      data[ 4] = Endian::shuffle32( Math::toBits( m.y.x ) );
      data[ 5] = Endian::shuffle32( Math::toBits( m.y.y ) );
      data[ 6] = Endian::shuffle32( Math::toBits( m.y.z ) );
      data[ 7] = Endian::shuffle32( Math::toBits( m.y.w ) );
      data[ 8] = Endian::shuffle32( Math::toBits( m.z.x ) );
      data[ 9] = Endian::shuffle32( Math::toBits( m.z.y ) );
      data[10] = Endian::shuffle32( Math::toBits( m.z.z ) );
      data[11] = Endian::shuffle32( Math::toBits( m.z.w ) );
      data[12] = Endian::shuffle32( Math::toBits( m.w.x ) );
      data[13] = Endian::shuffle32( Math::toBits( m.w.y ) );
      data[14] = Endian::shuffle32( Math::toBits( m.w.z ) );
      data[15] = Endian::shuffle32( Math::toBits( m.w.w ) );
    }

};

}
