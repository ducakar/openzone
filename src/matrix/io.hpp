/*
 *  io.hpp
 *
 *  Stream readers/writers and buffer
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{
  struct Endian
  {
    static ushort swap16( ushort s )
    {
      return ushort( s << 8 | s >> 8 );
    }

    static uint swap32( uint i )
    {
#ifdef __GNUC__
      return __builtin_bswap32( i );
#else
      return ( i << 24 ) | ( ( i & 0x0000ff00 ) << 8 ) |
          ( ( i & 0x00ff0000 ) >> 8 ) | ( i >> 24 );
#endif
    }

#ifndef OZ_LONG_32
    static long swap64( long l )
    {
#ifdef __GNUC__
      return __builtin_bswap64( l );
#else
      return ( l << 56 ) | ( ( l & 0x000000000000ff00L ) << 40 ) |
        ( ( l & 0x0000000000ff0000L ) << 24 ) | ( ( l & 0x00000000ff000000L ) << 8 ) |
        ( ( l & 0x000000ff00000000L ) >> 8 ) | ( ( l & 0x0000ff0000000000L ) >> 24 ) |
        ( ( l & 0x00ff000000000000L ) >> 40 ) | ( ulong( l ) >> 56 );
#endif
    }
#endif
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

      bool isPassed() const
      {
        assert( pos <= end );

        return pos == end;
      }

      const char* getPos() const
      {
        assert( pos <= end );

        return pos;
      }

      bool readBool()
      {
        static_assert( sizeof( bool ) == sizeof( char ), "unexpected bool type size" );

        if( pos + sizeof( bool ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const bool* b = reinterpret_cast<const bool*>( pos );
        pos += sizeof( bool );

        return *b;
      }

      byte readByte()
      {
        if( pos + sizeof( byte ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const byte* b = reinterpret_cast<const byte*>( pos );
        pos += sizeof( byte );

        return *b;
      }

      short readShort()
      {
        if( pos + sizeof( short ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const short* s = reinterpret_cast<const short*>( pos );
        pos += sizeof( short );

        return Endian::swap16( *s );
      }

      int readInt()
      {
        if( pos + sizeof( int ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* i = reinterpret_cast<const int*>( pos );
        pos += sizeof( int );

        return Endian::swap32( *i );
      }

      float readFloat()
      {
        if( pos + sizeof( float ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* f = reinterpret_cast<const int*>( pos );
        pos += sizeof( float );

        return Math::fromBits( Endian::swap32( *f ) );
      }

      String readString()
      {
        // check for buffer overruns
        const char* term = pos;
        while( term < end && *term != '\0' ) {
          ++term;
        }

        if( term == end ) {
          throw Exception( "Buffer overrun" );
        }

        const char* p = pos;
        int length = int( term - pos );
        pos += length + 1;

        return String( p, length );
      }

      /**
       * Read a string from input stream (optimised version, omits one string copy).
       */
      void readString( String& s )
      {
        // check for buffer overruns
        const char* term = pos;
        while( term < end && *term != '\0' ) {
          ++term;
        }

        if( term == end ) {
          throw Exception( "Buffer overrun" );
        }

        const char* p = pos;
        int length = int( term - pos );
        pos += length + 1;

        s = String( p, length );
      }

      Vec3 readVec3()
      {
        if( pos + sizeof( Vec3 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* v = reinterpret_cast<const int*>( pos );
        pos += sizeof( Vec3 );

        return Vec3( Math::fromBits( Endian::swap32( v[0] ) ),
                     Math::fromBits( Endian::swap32( v[1] ) ),
                     Math::fromBits( Endian::swap32( v[2] ) ) );
      }

      Quat readQuat()
      {
        if( pos + sizeof( Quat ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* q = reinterpret_cast<const int*>( pos );
        pos += sizeof( Quat );

        return Quat( Math::fromBits( Endian::swap32( q[0] ) ),
                     Math::fromBits( Endian::swap32( q[1] ) ),
                     Math::fromBits( Endian::swap32( q[2] ) ),
                     Math::fromBits( Endian::swap32( q[3] ) ) );
      }

      Mat33 readMat33()
      {
        if( pos + sizeof( Mat33 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* m = reinterpret_cast<const int*>( pos );
        pos += sizeof( Mat33 );

        return Mat33( Math::fromBits( Endian::swap32( m[0] ) ),
                      Math::fromBits( Endian::swap32( m[1] ) ),
                      Math::fromBits( Endian::swap32( m[2] ) ),
                      Math::fromBits( Endian::swap32( m[3] ) ),
                      Math::fromBits( Endian::swap32( m[4] ) ),
                      Math::fromBits( Endian::swap32( m[5] ) ),
                      Math::fromBits( Endian::swap32( m[6] ) ),
                      Math::fromBits( Endian::swap32( m[7] ) ),
                      Math::fromBits( Endian::swap32( m[8] ) ) );
      }

      Mat44 readMat44()
      {
        if( pos + sizeof( Mat44 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* m = reinterpret_cast<const int*>( pos );
        pos += sizeof( Mat44 );

        return Mat44( Math::fromBits( Endian::swap32( m[ 0] ) ),
                      Math::fromBits( Endian::swap32( m[ 1] ) ),
                      Math::fromBits( Endian::swap32( m[ 2] ) ),
                      Math::fromBits( Endian::swap32( m[ 3] ) ),
                      Math::fromBits( Endian::swap32( m[ 4] ) ),
                      Math::fromBits( Endian::swap32( m[ 5] ) ),
                      Math::fromBits( Endian::swap32( m[ 6] ) ),
                      Math::fromBits( Endian::swap32( m[ 7] ) ),
                      Math::fromBits( Endian::swap32( m[ 8] ) ),
                      Math::fromBits( Endian::swap32( m[ 9] ) ),
                      Math::fromBits( Endian::swap32( m[10] ) ),
                      Math::fromBits( Endian::swap32( m[11] ) ),
                      Math::fromBits( Endian::swap32( m[12] ) ),
                      Math::fromBits( Endian::swap32( m[13] ) ),
                      Math::fromBits( Endian::swap32( m[14] ) ),
                      Math::fromBits( Endian::swap32( m[15] ) ) );
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

      bool isPassed() const
      {
        assert( pos <= end );

        return pos == end;
      }

      char* getPos() const
      {
        assert( pos <= end );

        return pos;
      }

      void writeBool( bool b )
      {
        if( pos + sizeof( bool ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        bool* p = reinterpret_cast<bool*>( pos );
        pos += sizeof( bool );

        *p = b;
      }

      void writeByte( byte b )
      {
        if( pos + sizeof( byte ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        byte* p = reinterpret_cast<byte*>( pos );
        pos += sizeof( byte );

        *p = b;
      }

      void writeShort( short s )
      {
        if( pos + sizeof( short ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        short* p = reinterpret_cast<short*>( pos );
        pos += sizeof( short );

        *p = Endian::swap16( s );
      }

      void writeInt( int i )
      {
        if( pos + sizeof( int ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( int );

        *p = Endian::swap32( i );
      }

      void writeFloat( float f )
      {
        if( pos + sizeof( float ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( float );

        *p = Endian::swap32( Math::toBits( f ) );
      }

      void writeString( const String& s )
      {
        int length = s.length();

        if( pos + length >= end ) {
          throw Exception( "Buffer overrun" );
        }

        char* p = pos;
        pos += length + 1;

        aCopy( p, s.cstr(), length + 1 );
      }

      void writeString( const char* s )
      {
        int length = String::length( s );

        if( pos + length >= end ) {
          throw Exception( "Buffer overrun" );
        }

        char* p = pos;
        pos += length + 1;

        aCopy( p, s, length + 1 );
      }

      void writeVec3( const Vec3& v )
      {
        if( pos + sizeof( Vec3 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Vec3 );

        p[0] = Endian::swap32( Math::toBits( v.x ) );
        p[1] = Endian::swap32( Math::toBits( v.y ) );
        p[2] = Endian::swap32( Math::toBits( v.z ) );
      }

      void writeQuat( const Quat& q )
      {
        if( pos + sizeof( Quat ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Quat );

        p[0] = Endian::swap32( Math::toBits( q.x ) );
        p[1] = Endian::swap32( Math::toBits( q.y ) );
        p[2] = Endian::swap32( Math::toBits( q.z ) );
        p[3] = Endian::swap32( Math::toBits( q.w ) );
      }

      void writeMat33( const Mat33& m )
      {
        if( pos + sizeof( Mat33 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Mat33 );

        p[0] = Endian::swap32( Math::toBits( m.x.x ) );
        p[1] = Endian::swap32( Math::toBits( m.x.y ) );
        p[2] = Endian::swap32( Math::toBits( m.x.z ) );
        p[3] = Endian::swap32( Math::toBits( m.y.x ) );
        p[4] = Endian::swap32( Math::toBits( m.y.y ) );
        p[5] = Endian::swap32( Math::toBits( m.y.z ) );
        p[6] = Endian::swap32( Math::toBits( m.z.x ) );
        p[7] = Endian::swap32( Math::toBits( m.z.y ) );
        p[8] = Endian::swap32( Math::toBits( m.z.z ) );
      }

      void writeMat44( const Mat44& m )
      {
        if( pos + sizeof( Mat44 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Mat44 );

        p[ 0] = Endian::swap32( Math::toBits( m.x.x ) );
        p[ 1] = Endian::swap32( Math::toBits( m.x.y ) );
        p[ 2] = Endian::swap32( Math::toBits( m.x.z ) );
        p[ 3] = Endian::swap32( Math::toBits( m.x.w ) );
        p[ 4] = Endian::swap32( Math::toBits( m.y.x ) );
        p[ 5] = Endian::swap32( Math::toBits( m.y.y ) );
        p[ 6] = Endian::swap32( Math::toBits( m.y.z ) );
        p[ 7] = Endian::swap32( Math::toBits( m.y.w ) );
        p[ 8] = Endian::swap32( Math::toBits( m.z.x ) );
        p[ 9] = Endian::swap32( Math::toBits( m.z.y ) );
        p[10] = Endian::swap32( Math::toBits( m.z.z ) );
        p[11] = Endian::swap32( Math::toBits( m.z.w ) );
        p[12] = Endian::swap32( Math::toBits( m.w.x ) );
        p[13] = Endian::swap32( Math::toBits( m.w.y ) );
        p[14] = Endian::swap32( Math::toBits( m.w.z ) );
        p[15] = Endian::swap32( Math::toBits( m.w.w ) );
      }
  };

  /**
   * Memory buffer.
   * It can be filled with data from a file or written to a file.
   */
  class Buffer
  {
    private:

      // Size of block that will be used for block reads from/writes to a file. Optimal size is
      // the same as filesystem block size, that is 4K by default on Linux filesystems.
      static const int BLOCK_SIZE = 4096;

      char* buffer;
      int  count;

    public:

      explicit Buffer() : buffer( null ), count( 0 )
      {}

      explicit Buffer( int size ) : count( ( ( size - 1 ) / BLOCK_SIZE + 1 ) * BLOCK_SIZE )
      {
        buffer = new char[count];
      }

      ~Buffer()
      {
        if( buffer != null ) {
          delete[] buffer;
        }
      }

      bool isEmpty() const
      {
        assert( ( count == 0 ) == ( buffer == null ) );

        return count == 0;
      }

      int length() const
      {
        assert( ( count == 0 ) == ( buffer == null ) );

        return count;
      }

      void create( int size )
      {
        if( buffer != null ) {
          delete[] buffer;
        }
        buffer = new char[size];
        count = size;
      }

      void free()
      {
        if( buffer != null ) {
          delete[] buffer;
          buffer = null;
          count = 0;
        }
      }

      InputStream inputStream() const
      {
        assert( buffer != null );

        return InputStream( buffer, buffer + count );
      }

      OutputStream outputStream() const
      {
        assert( buffer != null );

        return OutputStream( buffer, buffer + count );
      }

      bool load( const char* path );

      bool write( const char* path );

  };

}
