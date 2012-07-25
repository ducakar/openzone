/*
 *  io.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "io.h"

#include <sys/stat.h>

namespace oz
{

  IO io;

  bool Buffer::load( const char *path )
  {
    if( buffer != null ) {
      delete[] buffer;
      buffer = null;
      count = 0;
    }

    struct stat fileStat;
#ifdef WIN32
    if( _stat( path, &fileStat ) != 0 || fileStat.st_size > 0x7fffffffl ) {
#else
    if( stat( path, &fileStat ) != 0 || fileStat.st_size > 0x7fffffffl ) {
#endif
      return false;
    }

    FILE *handle = fopen( path, "rb" );
    if( handle == null ) {
      return false;
    }

    count  = (int) fileStat.st_size;
    buffer = new char[count];

    int blocksToRead = ( count - 1 ) / io.blockSize + 1;
    int blocksRead = 1;

    while( blocksToRead > 0 && blocksRead > 0 ) {
      blocksRead = fread( buffer, io.blockSize, blocksToRead, handle );
      blocksToRead -= blocksRead;
    }
    fclose( handle );

    if( blocksToRead > 0 ) {
      delete[] buffer;
      buffer = null;
      count = 0;
      return false;
    }
    return true;
  }

  bool Buffer::write( const char *path )
  {
    FILE *handle = fopen( path, "wb" );
    if( handle == null ) {
      return false;
    }

    int blocksToWrite = ( count - 1 ) / io.blockSize + 1;
    int blocksWritten = 1;

    while( blocksToWrite > 0 && blocksWritten > 0 ) {
      blocksWritten = fwrite( buffer, io.blockSize, blocksToWrite, handle );
      blocksToWrite -= blocksWritten;
    }
    fclose( handle );

    if( blocksToWrite > 0 ) {
      delete[] buffer;
      buffer = null;
      count = 0;
      return false;
    }
    return true;
  }

  IO::IO() : blockSize( 4096 )
  {}

  void IO::initBlockSize( const char *file )
  {
    struct stat fileStat;
#ifdef WIN32
    if( _stat( file, &fileStat ) ) {
#else
    if( stat( file, &fileStat ) ) {
#endif
      blockSize = fileStat.st_blksize;
    }
  }

}
