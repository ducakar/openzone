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
 * @file oz/PhysFile.cc
 */

#include "PhysFile.hh"

#include "windefs.h"
#include <cerrno>

// TODO implement NaCl

#ifdef __native_client__
# include <ppapi/cpp/file_system.h>
# include <ppapi/cpp/file_ref.h>
# include <ppapi/cpp/file_io.h>
#else
# include <physfs.h>
#endif

namespace oz
{

inline bool operator < ( const PhysFile& a, const PhysFile& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

PhysFile::PhysFile() :
  fileType( File::DIRECTORY ), data( null ), size( 0 )
{}

PhysFile::~PhysFile()
{
  delete[] data;
}

PhysFile::PhysFile( const PhysFile& file ) :
  filePath( file.filePath ), fileType( file.fileType ), data( null ), size( 0 )
{}

PhysFile::PhysFile( PhysFile&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  data( file.data ), size( file.size )
{
  file.fileType = File::DIRECTORY;
  file.data     = null;
  file.size     = 0;
}

PhysFile& PhysFile::operator = ( const PhysFile& file )
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

PhysFile& PhysFile::operator = ( PhysFile&& file )
{
  if( &file == this ) {
    return *this;
  }

  delete[] data;

  filePath = static_cast<String&&>( file.filePath );
  fileType = file.fileType;
  data     = file.data;
  size     = file.size;

  file.fileType = File::DIRECTORY;
  file.data     = null;
  file.size     = 0;

  return *this;
}

PhysFile::PhysFile( const char* path ) :
  filePath( path ), data( null ), size( 0 )
{
#ifdef __native_client__

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  if( !PHYSFS_exists( filePath ) ) {
    fileType = File::MISSING;
  }
  else if( PHYSFS_isDirectory( filePath ) ) {
    fileType = File::DIRECTORY;
  }
  else {
    fileType = File::REGULAR;
  }

#endif
}

void PhysFile::setPath( const char* path )
{
  delete[] data;

  filePath = path;
  data     = null;
  size     = 0;

#ifdef __native_client__

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  if( !PHYSFS_exists( filePath ) ) {
    fileType = File::MISSING;
  }
  else if( PHYSFS_isDirectory( filePath ) ) {
    fileType = File::DIRECTORY;
  }
  else {
    fileType = File::REGULAR;
  }

#endif
}

File::Type PhysFile::type() const
{
  return fileType;
}

int PhysFile::getSize() const
{
#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  PHYSFS_File* file = PHYSFS_openRead( filePath );

  if( file != null ) {
    int size = int( PHYSFS_fileLength( file ) );
    PHYSFS_close( file );
    return size;
  }
  return -1;

#endif
}

String PhysFile::path() const
{
  return filePath;
}

String PhysFile::realDir() const
{
#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  const char* realDir = PHYSFS_getRealDir( filePath );
  return realDir == null ? "" : realDir;

#endif
}

String PhysFile::mountPoint() const
{
#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  const char* mountPoint = PHYSFS_getMountPoint( filePath );
  return mountPoint == null ? "" : mountPoint;

#endif
}

String PhysFile::name() const
{
  int slash = filePath.lastIndex( '/' );

  return slash == -1 ? filePath : filePath.substring( slash + 1 );
}

String PhysFile::extension() const
{
  int slash = filePath.lastIndex( '/' );
  int dot   = filePath.lastIndex( '.' );

  return slash < dot ? filePath.substring( dot + 1 ) : String();
}

String PhysFile::baseName() const
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

bool PhysFile::hasExtension( const char* ext ) const
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

bool PhysFile::isMapped() const
{
  return data != null;
}

bool PhysFile::map()
{
  if( data != null ) {
    unmap();
  }

#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == null ) {
    return false;
  }

  size = int( PHYSFS_fileLength( file ) );
  if( size <= 0 ) {
    return false;
  }

  data = new char[size];

  int result = int( PHYSFS_read( file, data, 1, uint( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    delete[] data;

    data = null;
    size = 0;
    return false;
  }

#endif

  return true;
}

void PhysFile::unmap()
{
  if( data != null ) {
    delete[] data;

    data = null;
    size = 0;
  }
}

InputStream PhysFile::inputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return InputStream( data, data + size, order );
}

Buffer PhysFile::read() const
{
  Buffer buffer;

#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == null ) {
    return buffer;
  }

  int fileSize = int( PHYSFS_fileLength( file ) );
  if( fileSize <= 0 ) {
    return buffer;
  }

  buffer.alloc( fileSize );

  int result = int( PHYSFS_read( file, buffer.begin(), 1, uint( fileSize ) ) );
  PHYSFS_close( file );

  if( result != fileSize ) {
    buffer.dealloc();
  }

#endif

  return buffer;
}

bool PhysFile::write( const char* buffer, int size ) const
{
#ifdef __native_client__

  static_cast<void>( buffer );
  static_cast<void>( size );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  PHYSFS_File* file = PHYSFS_openWrite( filePath );
  if( file == null ) {
    return buffer;
  }

  int result = int( PHYSFS_write( file, buffer, 1, uint( size ) ) );
  PHYSFS_close( file );

  return result == size;

#endif
}

bool PhysFile::write( const Buffer* buffer ) const
{
  return write( buffer->begin(), buffer->length() );
}

DArray<PhysFile> PhysFile::ls() const
{
  DArray<PhysFile> array;

  if( fileType != File::DIRECTORY ) {
    return array;
  }

#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  char** list = PHYSFS_enumerateFiles( filePath );
  if( list == null ) {
    return array;
  }

  // Count entries first.
  int count = 0;
  char** entity = list;
  while( *entity != null ) {
    if( ( *entity )[0] != '.' ) {
      ++count;
    }
    ++entity;
  }

  if( count == 0 ) {
    PHYSFS_freeList( list );
    return array;
  }

  array.alloc( count );

  String prefix = filePath.isEmpty() || filePath.equals( "/" ) ? "" : filePath + "/";

  entity = list;
  for( int i = 0; i < count; ++entity ) {
    if( ( *entity )[0] != '.' ) {
      array[i].setPath( prefix + *entity );
      ++i;
    }
  }

  PHYSFS_freeList( list );

#endif

  array.sort();
  return array;
}

bool PhysFile::mkdir( const char* path )
{
#ifdef __native_client__

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  return PHYSFS_mkdir( path ) != 0;

#endif
}

bool PhysFile::rm( const char* path )
{
#ifdef __native_client__

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  return PHYSFS_delete( path );

#endif
}

bool PhysFile::mount( const char* path, const char* mountPoint, bool append )
{
#ifdef __native_client__

  static_cast<void>( path );
  static_cast<void>( mountPoint );
  static_cast<void>( append );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  return PHYSFS_mount( path, mountPoint, append ) != 0;

#endif
}

bool PhysFile::mountLocal( const char* path )
{
#ifdef __native_client__

  static_cast<void>( path );

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  if( PHYSFS_setWriteDir( path ) == 0 ) {
    return false;
  }
  if( PHYSFS_mount( path, null, false ) == 0 ) {
    PHYSFS_setWriteDir( null );
    return false;
  }
  return true;

#endif
}

void PhysFile::init()
{
#if defined( __native_client__ )

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#elif defined( __ANDROID__ ) || defined( _WIN32 )

  if( PHYSFS_init( null ) == 0 ) {
    throw Exception( "PHYSFS initialisation failed" );
  }

#else

  if( PHYSFS_init( program_invocation_name ) == 0 ) {
    throw Exception( "PHYSFS initialisation failed" );
  }

#endif
}

void PhysFile::free()
{
#ifdef __native_client__

  throw Exception( "Not implemented: %s", __PRETTY_FUNCTION__ );

#else

  PHYSFS_deinit();

#endif
}

}
