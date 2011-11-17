/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/File.cpp
 */

#include "File.hpp"

#ifdef OZ_MINGW
# include <cstdio>
#else
# include <fcntl.h>
# include <sys/mman.h>
# include <unistd.h>
#endif
#include <dirent.h>
#include <sys/stat.h>

// prevent old-style cast warning for GCC
#ifdef __GNUC__
# undef MAP_FAILED
# define MAP_FAILED reinterpret_cast<void*>( -1 )
#endif

namespace oz
{

bool File::write( const char* buffer, int count ) const
{
#ifdef OZ_MINGW

  FILE* fs = fopen( filePath, "wb" );
  if( fs == null ) {
    return false;
  }

  int result = int( ::fwrite( buffer, size_t( count ), 1, fs ) );
  fclose( fs );

  return result == 1;

#else

  int fd = open( filePath, O_WRONLY | O_CREAT, 0644 );
  if( fd == -1 ) {
    return false;
  }

  int bytesWritten = int( ::write( fd, buffer, size_t( count ) ) );
  close( fd );

  return bytesWritten == count;

#endif
}

File::File() : type( NONE ), data( null )
{}

File::~File()
{
  soft_assert( data == null );

  if( data != null ) {
    unmap();
  }
}

File::File( const char* path ) : filePath( path ), type( NONE ), data( null )
{}

void File::setPath( const char* path )
{
  soft_assert( data == null );

  if( data != null ) {
    unmap();
  }

  filePath = path;
  type     = NONE;
  data     = null;
}

File::Type File::getType()
{
  if( type == NONE ) {
    struct stat info;
    if( stat( filePath, &info ) != 0 ) {
      type = MISSING;
    }
    else if( S_ISREG( info.st_mode ) ) {
      type = REGULAR;
    }
    else if( S_ISDIR( info.st_mode ) ) {
      type = DIRECTORY;
    }
    else {
      type = OTHER;
    }
  }
  return type;
}

const char* File::path() const
{
  return filePath;
}

const char* File::name() const
{
  const char* slash = String::findLast( filePath, '/' );

  return slash == null ? filePath.cstr() : slash + 1;
}

const char* File::extension() const
{
  const char* slash = String::findLast( filePath, '/' );
  const char* dot   = String::findLast( filePath, '.' );

  return slash < dot ? dot + 1 : null;
}

String File::baseName() const
{
  int slash = filePath.lastIndex( '/' );
  int dot   = filePath.lastIndex( '.' );

  if( slash < dot ) {
    return filePath.substring( slash + 1, dot );
  }
  else {
    return filePath.substring( slash + 1 );
  }
}

bool File::hasExtension( const char* ext ) const
{
  hard_assert( ext != null );

  const char* slash = String::findLast( filePath, '/' );
  const char* dot   = String::findLast( filePath, '.' );

  if( slash < dot ) {
    return String::equals( dot + 1, ext );
  }
  else {
    return ext[0] == '\0';
  }
}

bool File::map()
{
  hard_assert( data == null );

#ifdef OZ_MINGW

  struct stat statInfo;
  if( stat( filePath, &statInfo ) != 0 ) {
    return false;
  }

  FILE* fs = fopen( filePath, "rb" );
  if( fs == null ) {
    return false;
  }

  size = int( statInfo.st_size );
  data = new char[size];

  int result = int( ::fread( data, size_t( size ), 1, fs ) );
  fclose( fs );

  if( result != 1 ) {
    delete[] data;
    data = null;
    return false;
  }

#else

  int fd = open( filePath, O_RDONLY );
  if( fd == -1 ) {
    return false;
  }

  struct stat statInfo;
  if( fstat( fd, &statInfo ) != 0 ) {
    close( fd );
    return false;
  }

  size = int( statInfo.st_size );
  data = reinterpret_cast<char*>( mmap( null, size_t( size ), PROT_READ, MAP_SHARED, fd, 0 ) );
  close( fd );

  if( data == MAP_FAILED ) {
    data = null;
    return false;
  }

#endif

  return true;
}

void File::unmap()
{
  hard_assert( data != null );

#ifdef OZ_MINGW
  delete[] data;
#else
  munmap( data, size_t( size ) );
#endif
  data = null;
}

InputStream File::inputStream() const
{
  hard_assert( data != null );

  return InputStream( data, data + size );
}

Buffer File::read() const
{
  Buffer buffer;

#ifdef OZ_MINGW

  struct stat statInfo;
  if( stat( filePath, &statInfo ) != 0 ) {
    return buffer;
  }

  FILE* fs = fopen( filePath, "rb" );
  if( fs == null ) {
    return buffer;
  }

  int fileSize = int( statInfo.st_size );
  buffer.alloc( fileSize );

  int result = int( ::fread( buffer.begin(), size_t( fileSize ), 1, fs ) );
  fclose( fs );

  if( result != 1 ) {
    buffer.dealloc();
  }

#else

  int fd = open( filePath, O_RDONLY );
  if( fd == -1 ) {
    return buffer;
  }

  struct stat fileStat;
  if( fstat( fd, &fileStat ) != 0 ) {
    close( fd );
    return buffer;
  }

  int fileSize = int( fileStat.st_size );
  buffer.alloc( fileSize );

  int bytesRead = int( ::read( fd, buffer.begin(), size_t( fileSize ) ) );
  close( fd );

  if( bytesRead != fileSize ) {
    buffer.dealloc();
  }

#endif

  return buffer;
}

bool File::write( const Buffer* buffer ) const
{
  return write( buffer->begin(), buffer->length() );
}

bool File::write( const OutputStream* ostream ) const
{
  return write( ostream->begin(), ostream->length() );
}

bool File::write( const BufferStream* bstream ) const
{
  return write( bstream->begin(), bstream->length() );
}

bool File::chdir( const char* path )
{
  return ::chdir( path ) == 0;
}

bool File::mkdir( const char* path, uint mode )
{
#ifdef OZ_MINGW
  static_cast<void>( mode );

  return ::mkdir( path ) == 0;
#else
  return ::mkdir( path, mode ) == 0;
#endif
}

bool File::ls( DArray<File>* array )
{
  hard_assert( array != null && array->isEmpty() );

  if( getType() != DIRECTORY ) {
    return false;
  }

  DIR* directory = opendir( filePath );
  if( directory == null ) {
    return false;
  }

  struct dirent* entity = readdir( directory );

  // count entries first
  int count = 0;
  while( entity != null ) {
    if( entity->d_name[0] != '.' ) {
      ++count;
    }
    entity = readdir( directory );
  }

  if( count == 0 ) {
    closedir( directory );
    return true;
  }

  array->alloc( count );

  rewinddir( directory );

  for( int i = 0; i < count; ) {
    entity = readdir( directory );

    if( entity == null ) {
      array->dealloc();
      closedir( directory );
      return false;
    }

    if( entity->d_name[0] != '.' ) {
      ( *array )[i].filePath = ( filePath + "/" ) + entity->d_name;
#ifdef OZ_MINGW
      ( *array )[i].type = NONE;
#else
      if( entity->d_type == DT_REG ) {
        ( *array )[i].type = REGULAR;
      }
      else
        if( entity->d_type == DT_DIR ) {
          ( *array )[i].type = DIRECTORY;
        }
        else {
          ( *array )[i].type = OTHER;
        }
#endif
      ++i;
    }
  }

  closedir( directory );
  return true;
}

}
