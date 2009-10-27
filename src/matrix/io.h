/*
 *  io.h
 *
 *  Stream readers/writers and buffer
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  /**
   * Read-only nonseekable stream.
   */
  class InputStream
  {
    private:

      const char *pos;
      const char *end;

    public:

      explicit InputStream( const char *start, const char *end_ ) : pos( start ), end( end_ )
      {}

      bool isPassed() const
      {
        return pos >= end;
      }

      const char *getPos() const
      {
        return pos;
      }

      bool readBool()
      {
        bool *b = (bool*) pos;
        pos += sizeof( bool );

        if( pos + sizeof( bool ) > end ) {
          throw Exception( "Buffer overrun" );
        }
        return *b;
      }

      byte readByte()
      {
        byte *b = (byte*) pos;
        pos += sizeof( byte );

        if( pos + sizeof( byte ) > end ) {
          throw Exception( "Buffer overrun" );
        }
        return *b;
      }

      int readInt()
      {
        int *i = (int*) pos;
        pos += sizeof( int );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        return SDL_SwapBE32( *i );
      }

      float readFloat()
      {
        float *f = (float*) pos;
        pos += sizeof( float );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        return *f;
      }

      String readString()
      {
        // check for buffer overruns
        const char *p = pos;
        while( pos < end && *pos != '\0' ) {
          pos++;
        }
        if( pos >= end ) {
          throw Exception( "Buffer overrun" );
        }
        int length = pos - p;
        pos++;
        return String( p, length );
      }

      /**
       * Read a string from input stream (optimized version, omits one string copy).
       */
      void readString( String &s )
      {
        // check for buffer overruns
        const char *p = pos;
        while( pos < end && *pos != '\0' ) {
          pos++;
        }
        if( pos >= end ) {
          throw Exception( "Buffer overrun" );
        }
        int length = pos - p;
        pos++;
        s = String( p, length );
      }

      Vec3 readVec3()
      {
        if( pos + sizeof( Vec3 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        Vec3 *v = (Vec3*) pos;
        pos += sizeof( Vec3 );
        return *v;
      }

      Quat readQuat()
      {
        if( pos + sizeof( Quat ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        Quat *q = (Quat*) pos;
        pos += sizeof( Quat );
        return *q;
      }

      Mat33 readMat33()
      {
        if( pos + sizeof( Mat33 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        Mat33 *m = (Mat33*) pos;
        pos += sizeof( Mat33 );
        return *m;
      }

      Mat44 readMat44()
      {
        if( pos + sizeof( Mat44 ) > end ) {
          throw Exception( "Buffer overrun" );
        }

        Mat44 *m = (Mat44*) pos;
        pos += sizeof( Mat44 );
        return *m;
      }
  };

  /**
   * Write-only nonseekable stream.
   */
  class OutputStream
  {
    private:

      char       *pos;
      const char *end;

    public:

      explicit OutputStream( char *start, const char *end_ ) : pos( start ), end( end_ )
      {}

      bool isPassed() const
      {
        return pos == end;
      }

      char *getPos() const
      {
        return pos;
      }

      void writeBool( bool b )
      {
        bool *p = (bool*) pos;
        pos += sizeof( bool );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = b;
      }

      void writeByte( byte b )
      {
        byte *p = (byte*) pos;
        pos += sizeof( byte );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = b;
      }

      void writeInt( int i )
      {
        int *p = (int*) pos;
        pos += sizeof( int );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = SDL_SwapBE32( i );
      }

      void writeFloat( float f )
      {
        float *p = (float*) pos;
        pos += sizeof( float );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = f;
      }

      void writeString( const String &s )
      {
        int  length = s.length();
        char *p = pos;
        pos += length + 1;

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        aCopy( p, s.cstr(), length + 1 );
      }

      void writeString( const char *s )
      {
        int  length = String::length( s );
        char *p = pos;
        pos += length + 1;

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        aCopy( p, s, length + 1 );
      }

      void writeVec3( const Vec3 &v )
      {
        Vec3 *p = (Vec3*) pos;
        pos += sizeof( Vec3 );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = v;
      }

      void writeQuat( const Quat &q )
      {
        Quat *p = (Quat*) pos;
        pos += sizeof( Quat );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = q;
      }

      void writeMat33( const Mat33 &m )
      {
        Mat33 *p = (Mat33*) pos;
        pos += sizeof( Mat33 );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = m;
      }

      void writeMat44( const Mat44 &m )
      {
        Mat44 *p = (Mat44*) pos;
        pos += sizeof( Mat44 );

        if( pos > end ) {
          throw Exception( "Buffer overrun" );
        }
        *p = m;
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

      char *buffer;
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

      int length() const
      {
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

      bool load( const char *path );

      bool write( const char *path );

  };

}
