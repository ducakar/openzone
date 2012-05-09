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

#ifdef __native_client__
# include "System.hh"
#endif

#include "windefs.h"
#include <cerrno>
#include <cstring>
#include <physfs.h>
#ifdef __native_client__
# include <ppapi/c/ppb.h>
# include <ppapi/cpp/module.h>
# include <ppapi/cpp/instance.h>
#endif

namespace oz
{

inline bool operator < ( const PhysFile& a, const PhysFile& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

PhysFile::PhysFile() :
  fileType( File::MISSING ), fileSize( -1 ), data( null )
{}

PhysFile::~PhysFile()
{
  delete[] data;
}

PhysFile::PhysFile( const PhysFile& file ) :
  filePath( file.filePath ), fileType( file.fileType ), fileSize( file.fileSize ), data( null )
{}

PhysFile::PhysFile( PhysFile&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileSize( file.fileSize ), data( file.data )
{
  file.filePath = "";
  file.fileType = File::DIRECTORY;
  file.fileSize = -1;
  file.data     = null;
}

PhysFile& PhysFile::operator = ( const PhysFile& file )
{
  if( &file == this ) {
    return *this;
  }

  filePath = file.filePath;
  fileType = file.fileType;
  fileSize = file.fileSize;
  data     = null;

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
  fileSize = file.fileSize;
  data     = file.data;

  file.filePath = "";
  file.fileType = File::MISSING;
  file.fileSize = -1;
  file.data     = null;

  return *this;
}

PhysFile::PhysFile( const char* path ) :
  filePath( path ), fileType( File::MISSING ), fileSize( -1 ), data( null )
{}

void PhysFile::setPath( const char* path )
{
  delete[] data;

  filePath = path;
  fileType = File::MISSING;
  fileSize = -1;
  data     = null;
}

bool PhysFile::stat()
{
  unmap();

  if( !PHYSFS_exists( filePath ) ) {
    fileType = File::MISSING;
    fileSize = -1;
  }
  else if( PHYSFS_isDirectory( filePath ) ) {
    fileType = File::DIRECTORY;
    fileSize = -1;
  }
  else {
    fileType = File::REGULAR;
    fileSize = -1;

    PHYSFS_File* file = PHYSFS_openRead( filePath );

    if( file != null ) {
      fileSize = int( PHYSFS_fileLength( file ) );

      PHYSFS_close( file );
    }
  }

  return fileType != File::MISSING;
}

File::Type PhysFile::type() const
{
  return fileType;
}

int PhysFile::size() const
{
  return fileSize;
}

String PhysFile::path() const
{
  return filePath;
}

String PhysFile::realDir() const
{
  const char* realDir = PHYSFS_getRealDir( filePath );
  return realDir == null ? "" : realDir;
}

String PhysFile::mountPoint() const
{
  const char* mountPoint = PHYSFS_getMountPoint( filePath );
  return mountPoint == null ? "" : mountPoint;
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
    return true;
  }

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == null ) {
    return false;
  }

  int size = int( PHYSFS_fileLength( file ) );
  data = new char[size];

  int result = int( PHYSFS_read( file, data, 1, uint( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    delete[] data;
    data = null;
    return false;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return true;
}

void PhysFile::unmap()
{
  if( data != null ) {
    delete[] data;
    data = null;
  }
}

InputStream PhysFile::inputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return InputStream( data, data + fileSize, order );
}

Buffer PhysFile::read()
{
  Buffer buffer;

  if( data != null ) {
    buffer.alloc( fileSize );
    memcpy( buffer.begin(), data, size_t( fileSize ) );
    return buffer;
  }

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == null ) {
    return buffer;
  }

  int size = int( PHYSFS_fileLength( file ) );
  buffer.alloc( size );

  int result = int( PHYSFS_read( file, buffer.begin(), 1, uint( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    buffer.dealloc();
    return buffer;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return buffer;
}

bool PhysFile::write( const char* buffer, int size )
{
  if( data != null ) {
    return false;
  }

  PHYSFS_File* file = PHYSFS_openWrite( filePath );
  if( file == null ) {
    return buffer;
  }

  int result = int( PHYSFS_write( file, buffer, 1, uint( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    return false;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return true;
}

bool PhysFile::write( const Buffer* buffer )
{
  return write( buffer->begin(), buffer->length() );
}

DArray<PhysFile> PhysFile::ls()
{
  DArray<PhysFile> array;

  stat();

  if( fileType != File::DIRECTORY ) {
    return array;
  }

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

  array.sort();
  return array;
}

bool PhysFile::mkdir( const char* path )
{
  return PHYSFS_mkdir( path ) != 0;
}

bool PhysFile::rm( const char* path )
{
  return PHYSFS_delete( path );
}

bool PhysFile::mount( const char* path, const char* mountPoint, bool append )
{
  return PHYSFS_mount( path, mountPoint, append ) != 0;
}

bool PhysFile::mountLocal( const char* path )
{
  if( PHYSFS_setWriteDir( path ) == 0 ) {
    return false;
  }
  if( PHYSFS_mount( path, null, false ) == 0 ) {
    PHYSFS_setWriteDir( null );
    return false;
  }
  return true;
}

void PhysFile::init()
{
#if defined( __native_client__ )

  if( System::instance == null ) {
    throw Exception( "System::instance must be set to initialise PhysicsFS" );
  }

  PPB_GetInterface getInterface = pp::Module::Get()->get_browser_interface();
  const char* argv0 = *reinterpret_cast<const char* const*>( &getInterface );

  if( PHYSFS_init( argv0 ) == 0 ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }

#elif defined( __ANDROID__ ) || defined( _WIN32 )

  if( PHYSFS_init( null ) == 0 ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }

#else

  if( PHYSFS_init( program_invocation_name ) == 0 ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }

#endif
}

void PhysFile::free()
{
  PHYSFS_deinit();
}

}
