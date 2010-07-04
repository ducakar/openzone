/*
 *  io.hpp
 *
 *  Stream readers/writers and buffer
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{

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

        return SDL_SwapBE16( *s );
      }

      int readInt()
      {
        if( pos + sizeof( int ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* i = reinterpret_cast<const int*>( pos );
        pos += sizeof( int );

        return SDL_SwapBE32( *i );
      }

      float readFloat()
      {
        if( pos + sizeof( float ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* f = reinterpret_cast<const int*>( pos );
        pos += sizeof( float );

        return Math::fromBits( SDL_SwapBE32( *f ) );
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
        int length = term - pos;
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
        int length = term - pos;
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

        return Vec3( Math::fromBits( SDL_SwapBE32( v[0] ) ),
                     Math::fromBits( SDL_SwapBE32( v[1] ) ),
                     Math::fromBits( SDL_SwapBE32( v[2] ) ) );
      }

      Quat readQuat()
      {
        if( pos + sizeof( Quat ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* q = reinterpret_cast<const int*>( pos );
        pos += sizeof( Quat );

        return Quat( Math::fromBits( SDL_SwapBE32( q[0] ) ),
                     Math::fromBits( SDL_SwapBE32( q[1] ) ),
                     Math::fromBits( SDL_SwapBE32( q[2] ) ),
                     Math::fromBits( SDL_SwapBE32( q[3] ) ) );
      }

      Mat33 readMat33()
      {
        if( pos + sizeof( Mat33 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* m = reinterpret_cast<const int*>( pos );
        pos += sizeof( Mat33 );

        return Mat33( Math::fromBits( SDL_SwapBE32( m[0] ) ),
                      Math::fromBits( SDL_SwapBE32( m[1] ) ),
                      Math::fromBits( SDL_SwapBE32( m[2] ) ),
                      Math::fromBits( SDL_SwapBE32( m[3] ) ),
                      Math::fromBits( SDL_SwapBE32( m[4] ) ),
                      Math::fromBits( SDL_SwapBE32( m[5] ) ),
                      Math::fromBits( SDL_SwapBE32( m[6] ) ),
                      Math::fromBits( SDL_SwapBE32( m[7] ) ),
                      Math::fromBits( SDL_SwapBE32( m[8] ) ) );
      }

      Mat44 readMat44()
      {
        if( pos + sizeof( Mat44 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        const int* m = reinterpret_cast<const int*>( pos );
        pos += sizeof( Mat44 );

        return Mat44( Math::fromBits( SDL_SwapBE32( m[ 0] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 1] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 2] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 3] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 4] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 5] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 6] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 7] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 8] ) ),
                      Math::fromBits( SDL_SwapBE32( m[ 9] ) ),
                      Math::fromBits( SDL_SwapBE32( m[10] ) ),
                      Math::fromBits( SDL_SwapBE32( m[11] ) ),
                      Math::fromBits( SDL_SwapBE32( m[12] ) ),
                      Math::fromBits( SDL_SwapBE32( m[13] ) ),
                      Math::fromBits( SDL_SwapBE32( m[14] ) ),
                      Math::fromBits( SDL_SwapBE32( m[15] ) ) );
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

        *p = SDL_SwapBE16( s );
      }

      void writeInt( int i )
      {
        if( pos + sizeof( int ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( int );

        *p = SDL_SwapBE32( i );
      }

      void writeFloat( float f )
      {
        if( pos + sizeof( float ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( float );

        *p = SDL_SwapBE32( Math::toBits( f ) );
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

        p[0] = SDL_SwapBE32( Math::toBits( v.x ) );
        p[1] = SDL_SwapBE32( Math::toBits( v.y ) );
        p[2] = SDL_SwapBE32( Math::toBits( v.z ) );
      }

      void writeQuat( const Quat& q )
      {
        if( pos + sizeof( Quat ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Quat );

        p[0] = SDL_SwapBE32( Math::toBits( q.x ) );
        p[1] = SDL_SwapBE32( Math::toBits( q.y ) );
        p[2] = SDL_SwapBE32( Math::toBits( q.z ) );
        p[3] = SDL_SwapBE32( Math::toBits( q.w ) );
      }

      void writeMat33( const Mat33& m )
      {
        if( pos + sizeof( Mat33 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Mat33 );

        p[0] = SDL_SwapBE32( Math::toBits( m.x.x ) );
        p[1] = SDL_SwapBE32( Math::toBits( m.x.y ) );
        p[2] = SDL_SwapBE32( Math::toBits( m.x.z ) );
        p[3] = SDL_SwapBE32( Math::toBits( m.y.x ) );
        p[4] = SDL_SwapBE32( Math::toBits( m.y.y ) );
        p[5] = SDL_SwapBE32( Math::toBits( m.y.z ) );
        p[6] = SDL_SwapBE32( Math::toBits( m.z.x ) );
        p[7] = SDL_SwapBE32( Math::toBits( m.z.y ) );
        p[8] = SDL_SwapBE32( Math::toBits( m.z.z ) );
      }

      void writeMat44( const Mat44& m )
      {
        if( pos + sizeof( Mat44 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        int* p = reinterpret_cast<int*>( pos );
        pos += sizeof( Mat44 );

        p[ 0] = SDL_SwapBE32( Math::toBits( m.x.x ) );
        p[ 1] = SDL_SwapBE32( Math::toBits( m.x.y ) );
        p[ 2] = SDL_SwapBE32( Math::toBits( m.x.z ) );
        p[ 3] = SDL_SwapBE32( Math::toBits( m.x.w ) );
        p[ 4] = SDL_SwapBE32( Math::toBits( m.y.x ) );
        p[ 5] = SDL_SwapBE32( Math::toBits( m.y.y ) );
        p[ 6] = SDL_SwapBE32( Math::toBits( m.y.z ) );
        p[ 7] = SDL_SwapBE32( Math::toBits( m.y.w ) );
        p[ 8] = SDL_SwapBE32( Math::toBits( m.z.x ) );
        p[ 9] = SDL_SwapBE32( Math::toBits( m.z.y ) );
        p[10] = SDL_SwapBE32( Math::toBits( m.z.z ) );
        p[11] = SDL_SwapBE32( Math::toBits( m.z.w ) );
        p[12] = SDL_SwapBE32( Math::toBits( m.w.x ) );
        p[13] = SDL_SwapBE32( Math::toBits( m.w.y ) );
        p[14] = SDL_SwapBE32( Math::toBits( m.w.z ) );
        p[15] = SDL_SwapBE32( Math::toBits( m.w.w ) );
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
