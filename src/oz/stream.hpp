/*
 *  stream.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  struct Endian
  {
    static_assert( sizeof( short ) == 2, "sizeof( short ) should be 2" );
    static_assert( sizeof( int ) == 4, "sizeof( int ) should be 4" );

    static ushort shuffle16( ushort s )
    {
#if defined( OZ_BIG_ENDIAN_DATA ) == defined( OZ_BIG_ENDIAN_ARCH )
      return s;
#else
      return ushort( s << 8 | s >> 8 );
#endif
    }

    static uint shuffle32( uint i )
    {
#if defined( OZ_BIG_ENDIAN_DATA ) == defined( OZ_BIG_ENDIAN_ARCH )
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

    static ulong64 shuffle64( ulong64 l )
    {
#if defined( OZ_BIG_ENDIAN_DATA ) == defined( OZ_BIG_ENDIAN_ARCH )
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
      const char* end;

    public:

      explicit InputStream( const char* start, const char* end_ ) : pos( start ), end( end_ )
      {}

      bool hasData() const
      {
        return pos < end;
      }

      const char* getPos() const
      {
        assert( pos <= end );

        return pos;
      }

      const char* readBytes( int count )
      {
        const char* oldPos = pos;
        pos += count;

        if( pos > end ) {
          throw Exception( String( "Buffer overrun for " ) +
                           String( int( ptrdiff_t( pos - end ) ) ) + " bytes during read of " +
                           String( count ) + " bytes" );
        }
        return oldPos;
      }

      bool readBool()
      {
        const bool* data = reinterpret_cast<const bool*>( readBytes( sizeof( bool ) ) );
        return *data;
      }

      char readChar()
      {
        const char* data = reinterpret_cast<const char*>( readBytes( sizeof( char ) ) );
        return *data;
      }

      short readShort()
      {
        const short* data = reinterpret_cast<const short*>( readBytes( sizeof( short ) ) );
        return Endian::shuffle16( *data );
      }

      int readInt()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( int ) ) );
        return Endian::shuffle32( *data );
      }

      float readFloat()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( int ) ) );
        return Math::fromBits( Endian::shuffle32( *data ) );
      }

      String readString()
      {
        int length = 0;
        while( pos + length < end && pos[length] != '\0' ) {
          ++length;
        }
        return String( readBytes( length ), length );
      }

      /**
       * Read a string from input stream (optimised, omits one string copy).
       */
      void readString( String& s )
      {
        int length = 0;
        while( pos + length < end && pos[length] != '\0' ) {
          ++length;
        }
        s = String( readBytes( length ), length );
      }

      Vec3 readVec3()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( float[3] ) ) );
        return Vec3( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ) );
      }

      Quat readQuat()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( float[4] ) ) );
        return Quat( Math::fromBits( Endian::shuffle32( data[0] ) ),
                     Math::fromBits( Endian::shuffle32( data[1] ) ),
                     Math::fromBits( Endian::shuffle32( data[2] ) ),
                     Math::fromBits( Endian::shuffle32( data[3] ) ) );
      }

      Mat33 readMat33()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( float[9] ) ) );
        return Mat33( Math::fromBits( Endian::shuffle32( data[0] ) ),
                      Math::fromBits( Endian::shuffle32( data[1] ) ),
                      Math::fromBits( Endian::shuffle32( data[2] ) ),
                      Math::fromBits( Endian::shuffle32( data[3] ) ),
                      Math::fromBits( Endian::shuffle32( data[4] ) ),
                      Math::fromBits( Endian::shuffle32( data[5] ) ),
                      Math::fromBits( Endian::shuffle32( data[6] ) ),
                      Math::fromBits( Endian::shuffle32( data[7] ) ),
                      Math::fromBits( Endian::shuffle32( data[8] ) ) );
      }

      Mat44 readMat44()
      {
        const int* data = reinterpret_cast<const int*>( readBytes( sizeof( float[16] ) ) );
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
      const char* end;

    public:

      explicit OutputStream( char* start, const char* end_ ) : pos( start ), end( end_ )
      {}

      char* getPos() const
      {
        assert( pos <= end );

        return pos;
      }

      char* writeBytes( int count )
      {
        char* oldPos = pos;
        pos += count;

        if( pos > end ) {
          throw Exception( String( "Buffer overrun for " ) +
                           String( int( ptrdiff_t( pos - end ) ) ) + " bytes during read of " +
                           String( count ) + " bytes" );
        }
        return oldPos;
      }

      void writeBool( bool b )
      {
        bool* p = reinterpret_cast<bool*>( writeBytes( sizeof( bool ) ) );
        *p = b;
      }

      void writeChar( char c )
      {
        char* p = reinterpret_cast<char*>( writeBytes( sizeof( char ) ) );
        *p = c;
      }

      void writeShort( short s )
      {
        short* p = reinterpret_cast<short*>( writeBytes( sizeof( short ) ) );
        *p = Endian::shuffle16( s );
      }

      void writeInt( int i )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( int ) ) );
        *p = Endian::shuffle32( i );
      }

      void writeFloat( float f )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( float ) ) );
        *p = Endian::shuffle32( Math::toBits( f ) );
      }

      void writeString( const String& s )
      {
        int length = s.length() + 1;
        char* p = writeBytes( length );

        aCopy( p, s.cstr(), length );
      }

      void writeString( const char* s )
      {
        int length = String::length( s ) + 1;
        char* p = writeBytes( length );

        aCopy( p, s, length );
      }

      void writeVec3( const Vec3& v )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( float[3] ) ) );

        p[0] = Endian::shuffle32( Math::toBits( v.x ) );
        p[1] = Endian::shuffle32( Math::toBits( v.y ) );
        p[2] = Endian::shuffle32( Math::toBits( v.z ) );
      }

      void writeQuat( const Quat& q )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( float[4] ) ) );

        p[0] = Endian::shuffle32( Math::toBits( q.x ) );
        p[1] = Endian::shuffle32( Math::toBits( q.y ) );
        p[2] = Endian::shuffle32( Math::toBits( q.z ) );
        p[3] = Endian::shuffle32( Math::toBits( q.w ) );
      }

      void writeMat33( const Mat33& m )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( float[9] ) ) );

        p[0] = Endian::shuffle32( Math::toBits( m.x.x ) );
        p[1] = Endian::shuffle32( Math::toBits( m.x.y ) );
        p[2] = Endian::shuffle32( Math::toBits( m.x.z ) );
        p[3] = Endian::shuffle32( Math::toBits( m.y.x ) );
        p[4] = Endian::shuffle32( Math::toBits( m.y.y ) );
        p[5] = Endian::shuffle32( Math::toBits( m.y.z ) );
        p[6] = Endian::shuffle32( Math::toBits( m.z.x ) );
        p[7] = Endian::shuffle32( Math::toBits( m.z.y ) );
        p[8] = Endian::shuffle32( Math::toBits( m.z.z ) );
      }

      void writeMat44( const Mat44& m )
      {
        int* p = reinterpret_cast<int*>( writeBytes( sizeof( float[16] ) ) );

        p[ 0] = Endian::shuffle32( Math::toBits( m.x.x ) );
        p[ 1] = Endian::shuffle32( Math::toBits( m.x.y ) );
        p[ 2] = Endian::shuffle32( Math::toBits( m.x.z ) );
        p[ 3] = Endian::shuffle32( Math::toBits( m.x.w ) );
        p[ 4] = Endian::shuffle32( Math::toBits( m.y.x ) );
        p[ 5] = Endian::shuffle32( Math::toBits( m.y.y ) );
        p[ 6] = Endian::shuffle32( Math::toBits( m.y.z ) );
        p[ 7] = Endian::shuffle32( Math::toBits( m.y.w ) );
        p[ 8] = Endian::shuffle32( Math::toBits( m.z.x ) );
        p[ 9] = Endian::shuffle32( Math::toBits( m.z.y ) );
        p[10] = Endian::shuffle32( Math::toBits( m.z.z ) );
        p[11] = Endian::shuffle32( Math::toBits( m.z.w ) );
        p[12] = Endian::shuffle32( Math::toBits( m.w.x ) );
        p[13] = Endian::shuffle32( Math::toBits( m.w.y ) );
        p[14] = Endian::shuffle32( Math::toBits( m.w.z ) );
        p[15] = Endian::shuffle32( Math::toBits( m.w.w ) );
      }
  };

}
