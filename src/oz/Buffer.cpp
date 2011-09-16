/*
 *  Buffer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Buffer.hpp"

#include <cstdio>
#include <sys/stat.h>

namespace oz
{

  Buffer buffer;

  Buffer::Buffer() : data( null ), count( 0 )
  {}

  Buffer::~Buffer()
  {
    dealloc();
  }

  Buffer::Buffer( const Buffer& b ) : data( null ), count( b.count )
  {
    if( b.count != 0 ) {
      data = new char[count];
      aCopy( data, b.data, count );
    }
  }

  Buffer& Buffer::operator = ( const Buffer& b )
  {
    if( &b == this ) {
      soft_assert( &b != this );
      return *this;
    }

    if( count < b.count ) {
      dealloc();
      alloc( b.count );
    }

    aCopy( data, b.data, b.count );
    return *this;
  }

  Buffer::Buffer( int size ) : data( new char[size] ), count( size )
  {}

  Buffer::Buffer( const char* file ) : data( null ), count( 0 )
  {
    read( file );
  }

  void Buffer::alloc( int size )
  {
    hard_assert( data == null && count == 0 );

    data = new char[size];
    count = size;
  }

  void Buffer::dealloc()
  {
    delete[] data;

    data  = null;
    count = 0;
  }

  InputStream Buffer::inputStream() const
  {
    hard_assert( data != null );

    return InputStream( data, data + count );
  }

  OutputStream Buffer::outputStream() const
  {
    hard_assert( data != null );

    return OutputStream( data, data + count );
  }

  bool Buffer::read( const char* path )
  {
    struct stat fileStat;
    if( stat( path, &fileStat ) != 0 || fileStat.st_size > 0x7fffffffl ) {
      return false;
    }

    FILE* handle = fopen( path, "rb" );
    if( handle == null ) {
      return false;
    }

    if( count < int( fileStat.st_size ) ) {
      dealloc();
      alloc( int( fileStat.st_size ) );
    }

    int blocksRead = int( fread( data, size_t( fileStat.st_size ), 1, handle ) );
    fclose( handle );

    if( blocksRead != 1 && fileStat.st_size != 0 ) {
      return false;
    }
    return true;
  }

  bool Buffer::write( const char* path, int size )
  {
    hard_assert( data > 0 );

    FILE* handle = fopen( path, "wb" );
    if( handle == null ) {
      return false;
    }

    int result = int( fwrite( data, size_t( size ), 1, handle ) );
    fclose( handle );

    return result == 1;
  }

}
