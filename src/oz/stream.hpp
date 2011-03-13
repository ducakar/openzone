/*
 *  stream.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Exception.hpp"
#include "String.hpp"
#include "Plane.hpp"
#include "Mat44.hpp"

namespace oz
{

  struct Endian
  {
    OZ_ALWAYS_INLINE
    static ushort shuffle16( ushort s )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return s;
#else
      return ushort( s << 8 | s >> 8 );
#endif
    }

    OZ_ALWAYS_INLINE
    static uint shuffle32( uint i )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return i;
#else
# ifdef __GNUC__
      return __builtin_bswap32( i );
# else
      return ( i << 24 ) | ( i << 8 & 0x00ff0000 ) |
          ( i >> 8 & 0x0000ff00 ) | ( i >> 24 );
# endif
#endif
    }

    OZ_ALWAYS_INLINE
    static ulong64 shuffle64( ulong64 l )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return l;
#else
# ifdef __GNUC__
      return __builtin_bswap64( l );
# else
      return ( l << 56 ) | ( l << 40 & 0x00ff000000000000 ) | ( l << 24 & 0x0000ff0000000000 ) |
          ( l << 8 & 0x000000ff00000000 ) | ( l >> 8 & 0x00000000ff000000 ) |
          ( l >> 24 & 0x0000000000ff0000 ) | ( l >> 40 & 0x000000000000ff00 ) | ( l >> 56 );
# endif
#endif
    }
  };

  /**
   * Read-only non-seekable stream.
   */
  class InputStream
  {
    private:

      const char* pos;
      const char* start;
      const char* end;

    public:

      OZ_ALWAYS_INLINE
      explicit InputStream( const char* start_, const char* end_ ) :
          pos( start_ ), start( start_ ), end( end_ )
      {}

      OZ_ALWAYS_INLINE
      int length() const
      {
        return int( size_t( pos - start ) );
      }

      OZ_ALWAYS_INLINE
      int available() const
      {
        return int( size_t( end - pos ) );
      }

      OZ_ALWAYS_INLINE
      bool isAvailable() const
      {
        return pos != end;
      }

      OZ_ALWAYS_INLINE
      void reset()
      {
        pos = start;
      }

      OZ_ALWAYS_INLINE
      const char* getPos() const
      {
        hard_assert( start <= pos && pos <= end );

        return pos;
      }

      OZ_ALWAYS_INLINE
      const char* prepareRead( int count )
      {
        const char* oldPos = pos;
        pos += count;

#ifdef OZ_STREAM_READ_CHECK
        if( pos > end ) {
          pos -= count;
          throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                           " bytes during a read of " + String( count ) + " bytes" );
        }
#endif
        return oldPos;
      }

      OZ_ALWAYS_INLINE
      bool readBool()
      {
        const bool* data = reinterpret_cast<const bool*>( prepareRead( sizeof( bool ) ) );
        return *data;
      }

      OZ_ALWAYS_INLINE
      char readChar()
      {
        const char* data = reinterpret_cast<const char*>( prepareRead( sizeof( char ) ) );
        return *data;
      }

      OZ_ALWAYS_INLINE
      void readChars( char* array, int count )
      {
        const char* data = reinterpret_cast<const char*>( prepareRead( count ) );
        aCopy( array, data, count );
      }

      OZ_ALWAYS_INLINE
      short readShort()
      {
        const short* data = reinterpret_cast<const short*>( prepareRead( sizeof( short ) ) );
        return Endian::shuffle16( *data );
      }

      OZ_ALWAYS_INLINE
      int readInt()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( int ) ) );
        return Endian::shuffle32( *data );
      }

      OZ_ALWAYS_INLINE
      float readFloat()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( int ) ) );
        return Math::fromBits( Endian::shuffle32( *data ) );
      }

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
        return String( prepareRead( length + 1 ), length );
      }

      OZ_ALWAYS_INLINE
      String readPaddedString( int size )
      {
        const char* data = reinterpret_cast<const char*>( prepareRead( size ) );
        if( data[size - 1] != '\0' ) {
          throw Exception( "Padded string does not end with NULL character." );
        }
        return String( data );
      }

      OZ_ALWAYS_INLINE
      Vec3 readVec3()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[3] ) ) );
        return Vec3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ) );
      }

      OZ_ALWAYS_INLINE
      Vec4 readVec4()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
        return Vec4( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ),
                     Math::fromBits( Endian::shuffle32( data[3] ) ) );
      }

      OZ_ALWAYS_INLINE
      Point3 readPoint3()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[3] ) ) );
        return Point3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                       Math::fromBits( Endian::shuffle32( data[1] ) ),
                       Math::fromBits( Endian::shuffle32( data[2] ) ) );
      }

      OZ_ALWAYS_INLINE
      Plane readPlane()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
        return Plane( Math::fromBits( Endian::shuffle32( data[0] ) ),
                      Math::fromBits( Endian::shuffle32( data[1] ) ),
                      Math::fromBits( Endian::shuffle32( data[2] ) ),
                      Math::fromBits( Endian::shuffle32( data[3] ) ) );
      }

      OZ_ALWAYS_INLINE
      Quat readQuat()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[4] ) ) );
        return Quat( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ),
                     Math::fromBits( Endian::shuffle32( data[3] ) ) );
      }

      OZ_ALWAYS_INLINE
      Mat44 readMat44()
      {
        const int* data = reinterpret_cast<const int*>( prepareRead( sizeof( float[16] ) ) );
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

  /**
   * Write-only non-seekable stream.
   */
  class OutputStream
  {
    private:

      char*       pos;
      char*       start;
      const char* end;

    public:

      OZ_ALWAYS_INLINE
      explicit OutputStream( char* start_, const char* end_ ) :
          pos( start_ ), start( start_ ), end( end_ )
      {}

      OZ_ALWAYS_INLINE
      int length() const
      {
        return int( size_t( pos - start ) );
      }

      OZ_ALWAYS_INLINE
      int available() const
      {
        return int( size_t( end - pos ) );
      }

      OZ_ALWAYS_INLINE
      bool isAvailable() const
      {
        return pos != end;
      }

      OZ_ALWAYS_INLINE
      void reset()
      {
        pos = start;
      }

      OZ_ALWAYS_INLINE
      char* getPos() const
      {
        hard_assert( start <= pos && pos <= end );

        return pos;
      }

      OZ_ALWAYS_INLINE
      char* prepareWrite( int count )
      {
        char* oldPos = pos;
        pos += count;

        if( pos > end ) {
          pos -= count;
          throw Exception( "Buffer overrun for " + String( int( ptrdiff_t( pos + count - end ) ) ) +
                           " bytes during a write of " + String( count ) + " bytes" );
        }
        return oldPos;
      }

      OZ_ALWAYS_INLINE
      void writeBool( bool b )
      {
        bool* data = reinterpret_cast<bool*>( prepareWrite( sizeof( bool ) ) );
        *data = b;
      }

      OZ_ALWAYS_INLINE
      void writeChar( char c )
      {
        char* data = reinterpret_cast<char*>( prepareWrite( sizeof( char ) ) );
        *data = c;
      }

      OZ_ALWAYS_INLINE
      void writeChars( char *array, int count )
      {
        char* data = reinterpret_cast<char*>( prepareWrite( count * sizeof( char ) ) );
        aCopy( data, array, count );
      }

      OZ_ALWAYS_INLINE
      void writeShort( short s )
      {
        short* data = reinterpret_cast<short*>( prepareWrite( sizeof( short ) ) );
        *data = Endian::shuffle16( s );
      }

      OZ_ALWAYS_INLINE
      void writeInt( int i )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( int ) ) );
        *data = Endian::shuffle32( i );
      }

      OZ_ALWAYS_INLINE
      void writeFloat( float f )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float ) ) );
        *data = Endian::shuffle32( Math::toBits( f ) );
      }

      OZ_ALWAYS_INLINE
      void writeString( const String& s )
      {
        int size = s.length() + 1;
        char* data = prepareWrite( size );

        aCopy( data, s.cstr(), size );
      }

      OZ_ALWAYS_INLINE
      void writeString( const char* s )
      {
        int size = String::length( s ) + 1;
        char* data = prepareWrite( size );

        aCopy( data, s, size );
      }

      OZ_ALWAYS_INLINE
      void writePaddedString( const String& s, int size )
      {
        int length = s.length();
        char* data = prepareWrite( size );

        aCopy( data, s.cstr(), length );
        aSet( data + length, '\0', size - length );
      }

      OZ_ALWAYS_INLINE
      void writePaddedString( const char* s, int size )
      {
        int length = String::length( s );
        char* data = prepareWrite( size );

        aCopy( data, s, length );
        aSet( data + length, '\0', size - length );
      }

      OZ_ALWAYS_INLINE
      void writeVec3( const Vec3& v )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[3] ) ) );

        data[0] = Endian::shuffle32( Math::toBits( v.x ) );
        data[1] = Endian::shuffle32( Math::toBits( v.y ) );
        data[2] = Endian::shuffle32( Math::toBits( v.z ) );
      }

      OZ_ALWAYS_INLINE
      void writeVec4( const Vec4& v )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

        data[0] = Endian::shuffle32( Math::toBits( v.x ) );
        data[1] = Endian::shuffle32( Math::toBits( v.y ) );
        data[2] = Endian::shuffle32( Math::toBits( v.z ) );
        data[3] = Endian::shuffle32( Math::toBits( v.w ) );
      }

      OZ_ALWAYS_INLINE
      void writePoint3( const Point3& p )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[3] ) ) );

        data[0] = Endian::shuffle32( Math::toBits( p.x ) );
        data[1] = Endian::shuffle32( Math::toBits( p.y ) );
        data[2] = Endian::shuffle32( Math::toBits( p.z ) );
      }

      OZ_ALWAYS_INLINE
      void writePlane( const Plane& p )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

        data[0] = Endian::shuffle32( Math::toBits( p.n.x ) );
        data[1] = Endian::shuffle32( Math::toBits( p.n.y ) );
        data[2] = Endian::shuffle32( Math::toBits( p.n.z ) );
        data[3] = Endian::shuffle32( Math::toBits( p.d ) );
      }

      OZ_ALWAYS_INLINE
      void writeQuat( const Quat& q )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[4] ) ) );

        data[0] = Endian::shuffle32( Math::toBits( q.x ) );
        data[1] = Endian::shuffle32( Math::toBits( q.y ) );
        data[2] = Endian::shuffle32( Math::toBits( q.z ) );
        data[3] = Endian::shuffle32( Math::toBits( q.w ) );
      }

      OZ_ALWAYS_INLINE
      void writeMat44( const Mat44& m )
      {
        int* data = reinterpret_cast<int*>( prepareWrite( sizeof( float[16] ) ) );

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
