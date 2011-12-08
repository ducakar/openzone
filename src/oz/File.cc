/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * @file oz/File.cc
 */

#include "File.hh"

#include <cstring>

#ifdef _WIN32
# include <windows.h>
#else
# include <dirent.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/mman.h>
#endif

namespace oz
{

static bool operator < ( const File& a, const File& b )
{
  return strcmp( a.path(), b.path() ) < 0;
}

bool File::write( const char* buffer, int count ) const
{
#ifdef _WIN32

  HANDLE file = CreateFile( filePath, GENERIC_WRITE, 0, null, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, null );
  if( file == null ) {
    return false;
  }

  DWORD written;
  BOOL result = WriteFile( file, buffer, DWORD( count ), &written, null );
  CloseHandle( file );

  return result != 0 && int( written ) == count;

#else

  int fd = open( filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
  if( fd == -1 ) {
    return false;
  }

  int result = int( ::write( fd, buffer, size_t( count ) ) );
  close( fd );

  return result == count;

#endif
}

File::File() : type( NONE ), data( null )
{}

File::~File()
{
  if( data != null ) {
    unmap();
  }
}

File::File( File&& file ) :
    filePath( static_cast<String&&>( file.filePath ) ), type( file.type ),
    data( file.data ), size( file.size )
{
  file.type = NONE;
  file.data = null;
}

File& File::operator=( File&& file )
{
  if( &file == this ) {
    return *this;
  }

  if( data != null ) {
    unmap();
  }

  filePath = static_cast<String&&>( file.filePath );
  type     = file.type;
  data     = file.data;
  size     = file.size;

  file.type = NONE;
  file.data = null;

  return *this;
}

File::File( const char* path ) : filePath( path ), type( NONE ), data( null )
{}

void File::setPath( const char* path )
{
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
#ifdef _WIN32
    DWORD attributes = GetFileAttributes( filePath );

    if( attributes == INVALID_FILE_ATTRIBUTES ) {
      type = MISSING;
    }
    else if( attributes & FILE_ATTRIBUTE_DIRECTORY ) {
      type = DIRECTORY;
    }
    else {
      type = REGULAR;
    }
#else
    struct stat info;

    if( stat( filePath, &info ) != 0 ) {
      type = MISSING;
    }
    else if( S_ISDIR( info.st_mode ) ) {
      type = DIRECTORY;
    }
    else if( S_ISREG( info.st_mode ) ) {
      type = REGULAR;
    }
    else {
      type = OTHER;
    }
#endif
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

#ifdef _WIN32

  HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, null );
  if( file == null ) {
    return false;
  }

  HANDLE mapping = CreateFileMapping( file, null, PAGE_READONLY, 0, 0, null );
  if( mapping == null ) {
    CloseHandle( file );
    return false;
  }

  size = int( GetFileSize( mapping, null ) );
  data = reinterpret_cast<char*>( MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) );
  CloseHandle( mapping );
  CloseHandle( file );

  if( data == null ) {
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
  data = reinterpret_cast<char*>( mmap( null, size_t( statInfo.st_size ),
                                        PROT_READ, MAP_SHARED, fd, 0 ) );
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

#ifdef _WIN32
  UnmapViewOfFile( data );
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

#ifdef _WIN32

  HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, null );
  if( file == null ) {
    return buffer;
  }

  int fileSize = int( GetFileSize( file, null ) );
  buffer.alloc( fileSize );

  DWORD read;
  BOOL result = ReadFile( file, buffer.begin(), DWORD( fileSize ), &read, null );
  CloseHandle( file );

  if( result == 0 || int( read ) != fileSize ) {
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

  int result = int( ::read( fd, buffer.begin(), size_t( fileSize ) ) );
  close( fd );

  if( result != fileSize ) {
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
#ifdef _WIN32
  return SetCurrentDirectory( path ) != 0;
#else
  return ::chdir( path ) == 0;
#endif
}

bool File::mkdir( const char* path, uint mode )
{
#ifdef _WIN32
  static_cast<void>( mode );

  return CreateDirectory( path, null ) != 0;
#else
  return ::mkdir( path, mode ) == 0;
#endif
}

DArray<File> File::ls()
{
  DArray<File> array;

  if( getType() != DIRECTORY ) {
    return array;
  }

#ifdef _WIN32

  WIN32_FIND_DATA entity;

  HANDLE dir = FindFirstFile( filePath + "\\*.*", &entity );
  if( dir == null ) {
    return array;
  }

  // count entries first
  int count = 0;
  while( FindNextFile( dir, &entity ) ) {
    if( entity.cFileName[0] != '.' ) {
      ++count;
    }
  }

  CloseHandle( dir );

  if( count == 0 ) {
    return array;
  }

  dir = FindFirstFile( filePath + "\\*.*", &entity );
  if( dir == null ) {
    return array;
  }
  array.alloc( count );

  int i = 0;

  if( entity.cFileName[i] != '.' ) {
    array[i].filePath = filePath + "/" + entity.cFileName;
    ++i;
  }

  while( i < count ) {
    if( FindNextFile( dir, &entity ) == 0 ) {
      CloseHandle( dir );
      array.dealloc();
      return array;
    }

    if( entity.cFileName[0] != '.' ) {
      array[i].filePath = filePath + "/" + entity.cFileName;
      ++i;
    }
  }

  CloseHandle( dir );

#else

  DIR* directory = opendir( filePath );
  if( directory == null ) {
    return array;
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
    return array;
  }

  rewinddir( directory );
  array.alloc( count );

  for( int i = 0; i < count; ) {
    entity = readdir( directory );

    if( entity == null ) {
      closedir( directory );
      array.dealloc();
      return array;
    }

    if( entity->d_name[0] != '.' ) {
      array[i].filePath = filePath + "/" + entity->d_name;
      ++i;
    }
  }

  closedir( directory );

#endif

  array.sort();
  return array;
}

}
