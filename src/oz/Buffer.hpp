/*
 *  Buffer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stream.hpp"

namespace oz
{

  /**
   * Memory buffer.
   * It can be filled with data from a file or written to a file.
   */
  class Buffer
  {
    private:

      char* data;
      int   count;

    public:

      explicit Buffer() : data( null ), count( 0 )
      {}

      explicit Buffer( int size ) : data( new char[size] ), count( size )
      {}

      explicit Buffer( const char* file ) : data( null ), count( 0 )
      {
        read( file );
      }

      Buffer( const Buffer& b ) : data( null ), count( b.count )
      {
        if( b.count != 0 ) {
          data = new char[b.count];
          aCopy( data, b.data, b.count );
        }
      }

      ~Buffer()
      {
        free();
      }

      Buffer& operator = ( const Buffer& b )
      {
        delete[] data;
        data  = null;
        count = b.count;

        if( b.count != 0 ) {
          data = new char[b.count];
          aCopy( data, b.data, b.count );
        }
        return *this;
      }

      bool isEmpty() const
      {
        hard_assert( ( count == 0 ) == ( data == null ) );

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
        delete[] data;

        data  = null;
        count = 0;
      }

      InputStream inputStream() const
      {
        hard_assert( data != null );

        return InputStream( data, data + count );
      }

      OutputStream outputStream() const
      {
        hard_assert( data != null );

        return OutputStream( data, data + count );
      }

      bool read( const char* path );
      bool write( const char* path );

  };

}
