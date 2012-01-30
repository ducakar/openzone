/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/File.cc
 */

#include "File.hh"

#ifdef _WIN32
# include "windefs.h"
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

inline bool operator < ( const File& a, const File& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

File::File() :
  type( NONE ), data( null )
{}

File::~File()
{
  if( data != null ) {
    unmap();
  }
}

File::File( const File& file ) :
  filePath( file.filePath ), type( file.type ), data( null ), size( 0 )
{}

File::File( File&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), type( file.type ),
  data( file.data ), size( file.size )
{
  file.type = NONE;
  file.data = null;
}

File& File::operator = ( const File& file )
{
  if( &file == this ) {
    return *this;
  }

  filePath = file.filePath;
  type     = file.type;
  data     = null;
  size     = 0;

  return *this;
}

File& File::operator = ( File&& file )
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

File::File( const char* path ) :
  filePath( path ), type( NONE ), data( null )
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

String File::path() const
{
  return filePath;
}

String File::name() const
{
  int slash = filePath.lastIndex( '/' );

  return slash == -1 ? filePath : filePath.substring( slash + 1 );
}

String File::extension() const
{
  int slash = filePath.lastIndex( '/' );
  int dot   = filePath.lastIndex( '.' );

  return slash < dot ? filePath.substring( dot + 1 ) : String();
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
  const char* slash = filePath.findLast( '/' );
  const char* dot   = filePath.findLast( '.' );

  if( slash < dot ) {
    return String::equals( dot + 1, ext );
  }
  else {
    return String::isEmpty( ext );
  }
}

bool File::map()
{
  if( data != null ) {
    unmap();
  }

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
  if( data != null ) {
#ifdef _WIN32
    UnmapViewOfFile( data );
#else
    munmap( data, size_t( size ) );
#endif
    data = null;
  }
}

InputStream File::inputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return InputStream( data, data + size, order );
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

bool File::write( const Buffer* buffer ) const
{
  return write( buffer->begin(), buffer->length() );
}

bool File::write( const InputStream* istream ) const
{
  return write( istream->begin(), istream->capacity() );
}

bool File::write( const OutputStream* ostream ) const
{
  return write( ostream->begin(), ostream->length() );
}

bool File::write( const BufferStream* bstream ) const
{
  return write( bstream->begin(), bstream->length() );
}

bool File::unlink( const char* path )
{
#ifdef _WIN32
  return DeleteFile( path ) != 0;
#else
  return ::unlink( path ) == 0;
#endif
}

String File::cwd()
{
  char buffer[256];
#ifdef _WIN32
  bool hasFailed = GetCurrentDirectory( 256, buffer ) == 0;
#else
  bool hasFailed = getcwd( buffer, 256 ) == null;
#endif
  return hasFailed ? "." : buffer;
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

bool File::rmdir( const char* path )
{
#ifdef _WIN32
  return RemoveDirectory( path ) != 0;
#else
  return ::rmdir( path ) == 0;
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

  // Count entries first.
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

  // Count entries first.
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
