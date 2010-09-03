/*
 *  Buffer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  /**
   * Memory buffer.
   * It can be filled with data from a file or written to a file.
   */
  class Buffer
  {
    private:

      char*   data;
      Buffer* next;
      int     count;

    public:

      explicit Buffer() : data( null ), next( null ), count( 0 )
      {}

      explicit Buffer( int size ) :
          data( new char[size] ), next( null ), count( size )
      {}

      ~Buffer()
      {
        free();
      }

      bool isEmpty() const
      {
        assert( ( count == 0 ) == ( data == null ) );

        return count == 0;
      }

      void create( int size )
      {
        free();

        data = new char[size];
        count = size;
      }

      void free()
      {
        Buffer* currentBuffer = this;

        while( currentBuffer != null ) {
          Buffer* next = currentBuffer->next;

          delete[] currentBuffer->data;
          currentBuffer->data = null;
          currentBuffer->next = null;
          currentBuffer->count = 0;

          currentBuffer = next;
        }
      }

//      void append( char* bytes, int nBytes )
//      {
//        assert( nBytes > 0 );
//
//        Buffer* currentBuffer = this;
//        int bufferSpace = currentBuffer->size - currentBuffer->count;
//
//        while( nBytes > bufferSpace ) {
//          aCopy( currentBuffer->data + currentBuffer->count, bytes, bufferSpace );
//
//          currentBuffer = new Buffer( size );
//          bytes += bufferSpace;
//          nBytes -= bufferSpace;
//          bufferSpace = size;
//        }
//        aCopy( currentBuffer->data + currentBuffer->count, bytes, nBytes );
//      }

      InputStream inputStream() const
      {
        assert( data != null );

        return InputStream( data, data + count );
      }

      OutputStream outputStream() const
      {
        assert( data != null );

        return OutputStream( data, data + count );
      }

      bool load( const char* path );

      bool write( const char* path );

  };

}
