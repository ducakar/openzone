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
#include <physfs.h>

namespace oz
{

inline bool operator < ( const PhysFile& a, const PhysFile& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

PhysFile::PhysFile() :
  type( File::NONE ), data( null ), size( 0 )
{}

PhysFile::~PhysFile()
{
  delete[] data;
}

PhysFile::PhysFile( const PhysFile& file ) :
  filePath( file.filePath ), type( file.type ), data( null ), size( 0 )
{}

PhysFile::PhysFile( PhysFile&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), type( file.type ),
  data( file.data ), size( file.size )
{
  file.type = File::NONE;
  file.data = null;
  file.size = 0;
}

PhysFile& PhysFile::operator = ( const PhysFile& file )
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

PhysFile& PhysFile::operator = ( PhysFile&& file )
{
  if( &file == this ) {
    return *this;
  }

  delete[] data;

  filePath = static_cast<String&&>( file.filePath );
  type     = file.type;
  data     = file.data;
  size     = file.size;

  file.type = File::NONE;
  file.data = null;
  file.size = 0;

  return *this;
}

PhysFile::PhysFile( const char* path ) :
  filePath( path ), type( File::NONE ), data( null ), size( 0 )
{}

void PhysFile::setPath( const char* path )
{
  delete[] data;

  filePath = path;
  type     = File::NONE;
  data     = null;
  size     = 0;
}

File::Type PhysFile::getType()
{
  if( type == File::NONE ) {
    if( !PHYSFS_exists( filePath ) ) {
      type = File::MISSING;
    }
    else if( PHYSFS_isDirectory( filePath ) ) {
      type = File::DIRECTORY;
    }
    else {
      type = File::REGULAR;
    }
  }
  return type;
}

String PhysFile::path() const
{
  return filePath;
}

String PhysFile::realPath() const
{
  const char* mountPoint = PHYSFS_getRealDir( filePath );
  return mountPoint == null ? filePath : mountPoint + ( "/" + filePath );
}

String PhysFile::mountPoint() const
{
  const char* mountPoint = PHYSFS_getRealDir( filePath );
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

void PhysFile::clear()
{
  if( data != null ) {
    unmap();
  }

  filePath = "";
  type = File::NONE;
}

bool PhysFile::map()
{
  if( data != null ) {
    unmap();
  }

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

  return buffer;
}

DArray<PhysFile> PhysFile::ls()
{
  DArray<PhysFile> array;

  if( getType() != File::DIRECTORY ) {
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

  entity = list;
  for( int i = 0; i < count; ) {
    if( *entity == null ) {
      PHYSFS_freeList( list );
      array.dealloc();
      return array;
    }

    if( ( *entity )[0] != '.' ) {
      array[i].filePath = filePath + "/" + *entity;
      ++i;
    }
    ++entity;
  }

  PHYSFS_freeList( list );

  array.sort();
  return array;
}

bool PhysFile::mount( const char* source, const char* mountPoint, bool append )
{
  return PHYSFS_mount( source, mountPoint, append ) != 0;
}

bool PhysFile::init()
{
#if defined( __ANDROID__ ) || defined( _WIN32 )
  return PHYSFS_init( null ) != 0;
#else
  return PHYSFS_init( program_invocation_name ) != 0;
#endif
}

bool PhysFile::free()
{
  return PHYSFS_deinit() != 0;
}

}
