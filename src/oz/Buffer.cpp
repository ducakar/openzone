/*
 *  Buffer.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file oz/Buffer.cpp
 */

#include "Buffer.hpp"

#include <cstdio>
#include <sys/stat.h>

namespace oz
{

Buffer::Buffer() : data( null ), size( 0 )
{}

Buffer::~Buffer()
{
  dealloc();
}

Buffer::Buffer( const Buffer& b ) : data( null ), size( b.size )
{
  if( b.size != 0 ) {
    data = new char[size];
    aCopy( data, b.data, size );
  }
}

Buffer::Buffer( Buffer&& b ) : data( b.data ), size( b.size )
{
  b.data = null;
  b.size = 0;
}

Buffer& Buffer::operator = ( const Buffer& b )
{
  if( &b == this ) {
    soft_assert( &b != this );
    return *this;
  }

  if( size < b.size ) {
    dealloc();
    alloc( b.size );
  }

  aCopy( data, b.data, b.size );
  return *this;
}

Buffer& Buffer::operator = ( Buffer&& b )
{
  if( &b == this ) {
    soft_assert( &b != this );
    return *this;
  }

  delete[] data;

  data = b.data;
  size = b.size;

  b.data = null;
  b.size = 0;

  return *this;
}

Buffer::Buffer( int initSize ) : data( new char[initSize] ), size( initSize )
{}

Buffer::Buffer( const char* file ) : data( null ), size( 0 )
{
  read( file );
}

void Buffer::alloc( int initSize )
{
  hard_assert( size == 0 && initSize > 0 );

  data = new char[initSize];
  size = initSize;
}

void Buffer::dealloc()
{
  delete[] data;

  data = null;
  size = 0;
}

InputStream Buffer::inputStream() const
{
  hard_assert( data != null );

  return InputStream( data, data + size );
}

OutputStream Buffer::outputStream() const
{
  hard_assert( data != null );

  return OutputStream( data, data + size );
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

  if( size < int( fileStat.st_size ) ) {
    dealloc();
    alloc( int( fileStat.st_size ) );
  }

  size_t blocksRead = fread( data, size_t( fileStat.st_size ), 1, handle );
  fclose( handle );

  if( blocksRead != 1 && fileStat.st_size != 0 ) {
    return false;
  }
  return true;
}

bool Buffer::write( const char* path, int count )
{
  hard_assert( size >= count );

  FILE* handle = fopen( path, "wb" );
  if( handle == null ) {
    return false;
  }

  size_t result = fwrite( data, size_t( count ), 1, handle );
  fclose( handle );

  return result == 1;
}

}
