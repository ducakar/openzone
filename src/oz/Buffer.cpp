/*
 *  Buffer.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz.hpp"

#include <cstdio>
#include <sys/stat.h>

namespace oz
{

  bool Buffer::load( const char* path )
  {
    free();

    struct stat fileStat;
    if( stat( path, &fileStat ) != 0 || fileStat.st_size > 0x7fffffffl ) {
      return false;
    }

    FILE* handle = fopen( path, "rb" );
    if( handle == null ) {
      return false;
    }

    count = int( fileStat.st_size );
    data = new char[count];

    int blocksRead = int( fread( data, count, 1, handle ) );
    fclose( handle );

    if( blocksRead != 1 ) {
      free();
      return false;
    }
    return true;
  }

  bool Buffer::write( const char* path )
  {
    FILE* handle = fopen( path, "wb" );
    if( handle == null ) {
      return false;
    }

    Buffer* currentBuffer = this;
    int blocksWritten = -1;
    do {
      blocksWritten = int( fwrite( currentBuffer->data, currentBuffer->count, 1, handle ) );
      currentBuffer = currentBuffer->next;
    }
    while( currentBuffer != null && blocksWritten == 1 );
    fclose( handle );

    return blocksWritten == 1;
  }

}
