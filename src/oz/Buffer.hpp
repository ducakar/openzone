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

      explicit Buffer();
      explicit Buffer( int size );
      explicit Buffer( const char* file );

      Buffer( const Buffer& b );

      ~Buffer();

      Buffer& operator = ( const Buffer& b );

      bool isEmpty() const;

      void alloc( int size );
      void dealloc();

      InputStream inputStream() const;
      OutputStream outputStream() const;

      bool read( const char* path );
      bool write( const char* path, int size );

  };

  extern Buffer buffer;

}
