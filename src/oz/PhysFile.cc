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
 * @file oz/PhysFile.cc
 */

#include "PhysFile.hh"

#include "windefs.h"

#include <physfs.h>

namespace oz
{

inline bool operator < ( const PhysFile& a, const PhysFile& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

PhysFile::PhysFile() : type( NONE ), data( null )
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
  file.type = NONE;
  file.data = null;
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

  file.type = NONE;
  file.data = null;

  return *this;
}

PhysFile::PhysFile( const char* path ) : filePath( path ), type( NONE ), data( null )
{}

void PhysFile::setPath( const char* path )
{
  delete[] data;

  filePath = path;
  type     = NONE;
  data     = null;
}

PhysFile::Type PhysFile::getType()
{
  if( type == NONE ) {
    if( !PHYSFS_exists( filePath ) ) {
      type = MISSING;
    }
    else if( PHYSFS_isDirectory( filePath ) ) {
      type = DIRECTORY;
    }
    else {
      type = REGULAR;
    }
  }
  return type;
}

const String& PhysFile::path() const
{
  return filePath;
}

String PhysFile::realPath() const
{
  const char* mountPoint = PHYSFS_getRealDir( filePath );
  return mountPoint == null ? filePath : mountPoint + ( "/" + filePath );
}

const char* PhysFile::name() const
{
  const char* slash = String::findLast( filePath, '/' );

  return slash == null ? filePath.cstr() : slash + 1;
}

const char* PhysFile::extension() const
{
  const char* slash = String::findLast( filePath, '/' );
  const char* dot   = String::findLast( filePath, '.' );

  return slash < dot ? dot + 1 : null;
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

bool PhysFile::map()
{
  hard_assert( data == null );

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

    return false;
  }

  return true;
}

void PhysFile::unmap()
{
  hard_assert( data != null );

  delete[] data;
  data = null;
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

  if( getType() != DIRECTORY ) {
    return array;
  }

  char** list = PHYSFS_enumerateFiles( filePath );
  if( list == null ) {
    return array;
  }

  // count entries first
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

}
