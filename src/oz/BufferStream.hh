/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/BufferStream.hh
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
 * Buffered read/write stream.
 *
 * Stream uses an underlying buffer that is dynamically growing.
 *
 * @ingroup oz
 */
class BufferStream
{
  private:

    /// Capacity is always a multiple of <tt>GRANULARITY</tt>.
    static const int GRANULARITY = 4096;

    char*         pos;   ///< Current position.
    char*         start; ///< Beginning, also pointer to the buffer.
    const char*   end;   ///< End.
    Endian::Order order; ///< Stream byte order.

  public:

    /**
     * Create an empty stream.
     */
    BufferStream( Endian::Order order_ = Endian::NATIVE ) :
        pos( null ), start( null ), end( null ), order( order_ )
    {}

    /**
     * Destructor.
     */
    ~BufferStream()
    {
      delete[] start;
    }

    /**
     * Copy constructor, copies buffer.
     */
    BufferStream( const BufferStream& s )
    {
      int length = int( s.pos - s.start );
      int size   = int( s.end - s.start );

      start = size == 0 ? null : new char[size];
      end   = start + size;
      pos   = start + length;
      order = s.order;

      aCopy( start, s.start, size );
    }

    /**
     * Move constructor, moves buffer.
     */
    BufferStream( BufferStream&& s ) : pos( s.pos ), start( s.start ), end( s.end )
    {
      s.pos   = null;
      s.start = null;
      s.end   = null;
      s.order = Endian::NATIVE;
    }

    /**
     * Copy operator, copies buffer.
     *
     * Reuse existing buffer if it suffices.
     */
    BufferStream& operator = ( const BufferStream& s )
    {
      if( &s == this ) {
        return *this;
      }

      int length = int( s.pos - s.start );
      int size   = int( s.end - s.start );

      if( int( end - start ) < size ) {
        delete[] start;

        start = new char[size];
        end   = start + size;
      }

      pos = start + length;
      order = s.order;
      aCopy( start, s.start, size );

      return *this;
    }

    /**
     * Move operator, moves buffer.
     */
    BufferStream& operator = ( BufferStream&& s )
    {
      if( &s == this ) {
        return *this;
      }

      delete[] start;

      pos   = s.pos;
      start = s.start;
      end   = s.end;
      order = s.order;

      s.pos   = null;
      s.start = null;
      s.end   = null;
      s.order = Endian::NATIVE;

      return *this;
    }

    /**
     * Create a stream with the given size of the buffer.
     */
    explicit BufferStream( int initSize ) :
        pos( new char[initSize] ), start( pos ), end( start + initSize ), order( Endian::NATIVE )
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
     * Maximum length of the stream.
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
        int length  = int( pos - start );
        int size    = int( end - start );
        int newSize = size == 0 ? GRANULARITY : 2 * size;

        if( newSize < length + count ) {
          newSize = ( ( length + count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;
        }

        start  = aRealloc( start, size, newSize );
        end    = start + newSize;
        pos    = start + length;
        oldPos = pos - count;
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
     * Write a boolean.
     */
    OZ_ALWAYS_INLINE
    void writeBool( bool b )
    {
      bool* data = reinterpret_cast<bool*>( forward( sizeof( bool ) ) );
      *data = b;
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
     * Write a character.
     */
    OZ_ALWAYS_INLINE
    void writeChar( char c )
    {
      char* data = reinterpret_cast<char*>( forward( sizeof( char ) ) );
      *data = c;
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
     * Write an array of characters.
     */
    OZ_ALWAYS_INLINE
    void writeChars( char *array, int count )
    {
      char* data = reinterpret_cast<char*>( forward( count * int( sizeof( char ) ) ) );
      aCopy( data, array, count );
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
     * Write short integer.
     */
    OZ_ALWAYS_INLINE
    void writeShort( short s )
    {
      short* data = reinterpret_cast<short*>( forward( sizeof( short ) ) );

      if( order == Endian::NATIVE ) {
        *data = s;
      }
      else {
        *data = Endian::bswap16( s );
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
     * Write integer.
     */
    OZ_ALWAYS_INLINE
    void writeInt( int i )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( int ) ) );

      if( order == Endian::NATIVE ) {
        *data = i;
      }
      else {
        *data = Endian::bswap32( i );
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
     * Write 64-bit integer.
     */
    OZ_ALWAYS_INLINE
    void writeLong64( long64 l )
    {
      long64* data = reinterpret_cast<long64*>( forward( sizeof( long64 ) ) );

      if( order == Endian::NATIVE ) {
        *data = l;
      }
      else {
        *data = Endian::bswap64( l );
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
     * Write float.
     */
    OZ_ALWAYS_INLINE
    void writeFloat( float f )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float ) ) );

      if( order == Endian::NATIVE ) {
        *data = Math::toBits( f );
      }
      else {
        *data = Endian::bswap32( Math::toBits( f ) );
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
        BitsToDouble bd = { Endian::bswap64( *data ) };
        return bd.d;
      }
      else {
        BitsToDouble bd = { *data };
        return bd.d;
      }
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

      if( order == Endian::NATIVE ) {
        *data = db.b;
      }
      else {
        *data = Endian::bswap64( db.b );
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
      return String( forward( length + 1 ), length );
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
     * Write 3D vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec3( const Vec3& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( v.x );
        data[1] = Math::toBits( v.y );
        data[2] = Math::toBits( v.z );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( v.x ) );
        data[1] = Endian::bswap32( Math::toBits( v.y ) );
        data[2] = Endian::bswap32( Math::toBits( v.z ) );
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
     * Write 4-component vector.
     */
    OZ_ALWAYS_INLINE
    void writeVec4( const Vec4& v )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( v.x );
        data[1] = Math::toBits( v.y );
        data[2] = Math::toBits( v.z );
        data[3] = Math::toBits( v.w );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( v.x ) );
        data[1] = Endian::bswap32( Math::toBits( v.y ) );
        data[2] = Endian::bswap32( Math::toBits( v.z ) );
        data[3] = Endian::bswap32( Math::toBits( v.w ) );
      }
    }

    /**
     * Read 3D point.
     */
    OZ_ALWAYS_INLINE
    Point3 readPoint3()
    {
      const int* data = reinterpret_cast<const int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        return Point3( Math::fromBits( data[0] ),
                       Math::fromBits( data[1] ),
                       Math::fromBits( data[2] ) );
      }
      else {
        return Point3( Math::fromBits( Endian::bswap32( data[0] ) ),
                       Math::fromBits( Endian::bswap32( data[1] ) ),
                       Math::fromBits( Endian::bswap32( data[2] ) ) );
      }
    }

    /**
     * Write 3D point.
     */
    OZ_ALWAYS_INLINE
    void writePoint3( const Point3& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[3] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( p.x );
        data[1] = Math::toBits( p.y );
        data[2] = Math::toBits( p.z );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( p.x ) );
        data[1] = Endian::bswap32( Math::toBits( p.y ) );
        data[2] = Endian::bswap32( Math::toBits( p.z ) );
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
     * Write 3D plane.
     */
    OZ_ALWAYS_INLINE
    void writePlane( const Plane& p )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( p.nx );
        data[1] = Math::toBits( p.ny );
        data[2] = Math::toBits( p.nz );
        data[3] = Math::toBits( p.d );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( p.nx ) );
        data[1] = Endian::bswap32( Math::toBits( p.ny ) );
        data[2] = Endian::bswap32( Math::toBits( p.nz ) );
        data[3] = Endian::bswap32( Math::toBits( p.d ) );
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
     * Write quaternion.
     */
    OZ_ALWAYS_INLINE
    void writeQuat( const Quat& q )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[4] ) ) );

      if( order == Endian::NATIVE ) {
        data[0] = Math::toBits( q.x );
        data[1] = Math::toBits( q.y );
        data[2] = Math::toBits( q.z );
        data[3] = Math::toBits( q.w );
      }
      else {
        data[0] = Endian::bswap32( Math::toBits( q.x ) );
        data[1] = Endian::bswap32( Math::toBits( q.y ) );
        data[2] = Endian::bswap32( Math::toBits( q.z ) );
        data[3] = Endian::bswap32( Math::toBits( q.w ) );
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
     * Write 4x4 matrix.
     */
    OZ_ALWAYS_INLINE
    void writeMat44( const Mat44& m )
    {
      int* data = reinterpret_cast<int*>( forward( sizeof( float[16] ) ) );

      if( order == Endian::NATIVE ) {
        data[ 0] = Math::toBits( m.x.x );
        data[ 1] = Math::toBits( m.x.y );
        data[ 2] = Math::toBits( m.x.z );
        data[ 3] = Math::toBits( m.x.w );
        data[ 4] = Math::toBits( m.y.x );
        data[ 5] = Math::toBits( m.y.y );
        data[ 6] = Math::toBits( m.y.z );
        data[ 7] = Math::toBits( m.y.w );
        data[ 8] = Math::toBits( m.z.x );
        data[ 9] = Math::toBits( m.z.y );
        data[10] = Math::toBits( m.z.z );
        data[11] = Math::toBits( m.z.w );
        data[12] = Math::toBits( m.w.x );
        data[13] = Math::toBits( m.w.y );
        data[14] = Math::toBits( m.w.z );
        data[15] = Math::toBits( m.w.w );
      }
      else {
        data[ 0] = Endian::bswap32( Math::toBits( m.x.x ) );
        data[ 1] = Endian::bswap32( Math::toBits( m.x.y ) );
        data[ 2] = Endian::bswap32( Math::toBits( m.x.z ) );
        data[ 3] = Endian::bswap32( Math::toBits( m.x.w ) );
        data[ 4] = Endian::bswap32( Math::toBits( m.y.x ) );
        data[ 5] = Endian::bswap32( Math::toBits( m.y.y ) );
        data[ 6] = Endian::bswap32( Math::toBits( m.y.z ) );
        data[ 7] = Endian::bswap32( Math::toBits( m.y.w ) );
        data[ 8] = Endian::bswap32( Math::toBits( m.z.x ) );
        data[ 9] = Endian::bswap32( Math::toBits( m.z.y ) );
        data[10] = Endian::bswap32( Math::toBits( m.z.z ) );
        data[11] = Endian::bswap32( Math::toBits( m.z.w ) );
        data[12] = Endian::bswap32( Math::toBits( m.w.x ) );
        data[13] = Endian::bswap32( Math::toBits( m.w.y ) );
        data[14] = Endian::bswap32( Math::toBits( m.w.z ) );
        data[15] = Endian::bswap32( Math::toBits( m.w.w ) );
      }
    }

    /**
     * Deallocate allocated buffer.
     */
    void dealloc()
    {
      delete[] start;

      pos   = null;
      start = null;
      end   = null;
    }

};

}
