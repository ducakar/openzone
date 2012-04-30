/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/File.cc
 */

#include "File.hh"

#if defined( __native_client__ )
# include "Exception.hh"
#elif defined( _WIN32 )
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
  fileType( NONE ), data( null ), size( 0 )
{}

File::~File()
{
  if( data != null ) {
    unmap();
  }
}

File::File( const File& file ) :
  filePath( file.filePath ), fileType( file.fileType ), data( null ), size( 0 )
{}

File::File( File&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  data( file.data ), size( file.size )
{
  file.fileType = NONE;
  file.data     = null;
  file.size     = 0;
}

File& File::operator = ( const File& file )
{
  if( &file == this ) {
    return *this;
  }

  filePath = file.filePath;
  fileType = file.fileType;
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
  fileType = file.fileType;
  data     = file.data;
  size     = file.size;

  file.fileType = NONE;
  file.data     = null;
  file.size     = 0;

  return *this;
}

File::File( const char* path ) :
  filePath( path ), data( null ), size( 0 )
{
#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  DWORD attributes = GetFileAttributes( filePath );

  if( attributes == INVALID_FILE_ATTRIBUTES ) {
    fileType = MISSING;
  }
  else if( attributes & FILE_ATTRIBUTE_DIRECTORY ) {
    fileType = DIRECTORY;
  }
  else {
    fileType = REGULAR;
  }

#else

  struct stat info;

  if( stat( filePath, &info ) != 0 ) {
    fileType = MISSING;
  }
  else if( S_ISDIR( info.st_mode ) ) {
    fileType = DIRECTORY;
  }
  else if( S_ISREG( info.st_mode ) ) {
    fileType = REGULAR;
  }
  else {
    fileType = OTHER;
  }

#endif
}

void File::setPath( const char* path )
{
  if( data != null ) {
    unmap();
  }

  filePath = path;
  data     = null;
  size     = 0;

  if( String::isEmpty( path ) ) {
    fileType = NONE;
  }
  else {
#if defined( __native_client__ )

    throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

    DWORD attributes = GetFileAttributes( filePath );

    if( attributes == INVALID_FILE_ATTRIBUTES ) {
      fileType = MISSING;
    }
    else if( attributes & FILE_ATTRIBUTE_DIRECTORY ) {
      fileType = DIRECTORY;
    }
    else {
      fileType = REGULAR;
    }

#else

    struct stat info;

    if( stat( filePath, &info ) != 0 ) {
      fileType = MISSING;
    }
    else if( S_ISDIR( info.st_mode ) ) {
      fileType = DIRECTORY;
    }
    else if( S_ISREG( info.st_mode ) ) {
      fileType = REGULAR;
    }
    else {
      fileType = OTHER;
    }

#endif
  }
}

File::Type File::type()
{
  return fileType;
}

int File::getSize() const
{
#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  HANDLE handle = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, null );
  if( handle != null ) {
    int size = int( GetFileSize( handle, null ) );
    CloseHandle( handle );
    return size;
  }

#else

  struct stat info;
  if( stat( filePath, &info ) == 0 ) {
    return int( info.st_size );
  }

#endif

  return -1;
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

bool File::isMapped() const
{
  return data != null;
}

bool File::map()
{
  if( data != null ) {
    unmap();
  }

#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

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
  data = static_cast<char*>( MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) );
  CloseHandle( mapping );
  CloseHandle( file );

  if( data == null ) {
    size = 0;
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
  data = static_cast<char*>( mmap( null, size_t( statInfo.st_size ),
                                   PROT_READ, MAP_SHARED, fd, 0 ) );
  close( fd );
  if( data == MAP_FAILED ) {
    data = null;
    size = 0;
    return false;
  }

#endif

  return true;
}

void File::unmap()
{
  if( data != null ) {
#if defined( __native_client__ )
    throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );
#elif defined( _WIN32 )
    UnmapViewOfFile( data );
#else
    munmap( data, size_t( size ) );
#endif
    data = null;
    size = 0;
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

#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

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
#if defined( __native_client__ )

  static_cast<void>( buffer );
  static_cast<void>( count );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

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

String File::cwd()
{
#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  char buffer[256];
  bool hasFailed = GetCurrentDirectory( 256, buffer ) == 0;
  return hasFailed ? "." : buffer;

#else

  char buffer[256];
  bool hasFailed = getcwd( buffer, 256 ) == null;
  return hasFailed ? "." : buffer;

#endif
}

bool File::chdir( const char* path )
{
#if defined( __native_client__ )

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  return SetCurrentDirectory( path ) != 0;

#else

  return ::chdir( path ) == 0;

#endif
}

DArray<File> File::ls()
{
  DArray<File> array;

  if( fileType != DIRECTORY ) {
    return array;
  }

#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

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

  String prefix = filePath.isEmpty() || filePath.equals( "/" ) ? "" : filePath + "/";

  for( int i = 0; i < count; ) {
    entity = readdir( directory );

    if( entity == null ) {
      closedir( directory );
      array.dealloc();
      return array;
    }

    if( entity->d_name[0] != '.' ) {
      array[i].filePath = prefix + entity->d_name;
      ++i;
    }
  }

  closedir( directory );

#endif

  array.sort();
  return array;
}

bool File::mkdir( const char* path )
{
#if defined( __native_client__ )

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  return CreateDirectory( path, null ) != 0;

#else

  return ::mkdir( path, 0755 ) == 0;

#endif
}

bool File::rm( const char* path )
{
#if defined( __native_client__ )

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( _WIN32 )

  if( File( path ).fileType == DIRECTORY ) {
    return RemoveDirectory( path ) != 0;
  }
  else {
    return DeleteFile( path ) != 0;
  }

#else

  if( File( path ).fileType == DIRECTORY ) {
    return ::rmdir( path ) == 0;
  }
  else {
    return ::unlink( path ) == 0;
  }

#endif
}

}
