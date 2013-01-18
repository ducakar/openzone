/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/PFile.cc
 */

#include "PFile.hh"

#include <cerrno>
#include <physfs.h>

#ifdef __native_client__
# include <ppapi/c/ppb.h>
# include <ppapi/cpp/file_system.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/module.h>

extern "C"
int PHYSFS_NACL_init( PP_Instance instance, PPB_GetInterface getInterface,
                      PP_FileSystemType filesystemType, PHYSFS_sint64 size );
#endif

#if PHYSFS_VER_MAJOR < 2
# error PhysicsFS version must be at least 2.0.
#elif PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0
# define PHYSFS_readBytes( handle, buffer, len )  PHYSFS_read( handle, buffer, 1, uint( len ) )
# define PHYSFS_writeBytes( handle, buffer, len ) PHYSFS_write( handle, buffer, 1, uint( len ) )
#endif

namespace oz
{

static bool operator < ( const PFile& a, const PFile& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

PFile::PFile( const char* path ) :
  filePath( path ), fileType( File::MISSING ), fileSize( -1 ), fileTime( 0 )
{
  // Avoid stat'ing obviously non-existent files.
  if( filePath.isEmpty() ) {
    return;
  }

#if PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0

  if( !PHYSFS_exists( filePath ) ) {
    fileType = File::MISSING;
    fileSize = -1;
    fileTime = 0;
  }
  else if( PHYSFS_isDirectory( filePath ) ) {
    fileType = File::DIRECTORY;
    fileSize = -1;
    fileTime = PHYSFS_getLastModTime( filePath );
  }
  else {
    PHYSFS_File* file = PHYSFS_openRead( filePath );

    if( file == nullptr ) {
      fileType = File::MISSING;
      fileSize = -1;
      fileTime = 0;
    }
    else {
      fileType = File::REGULAR;
      fileSize = int( PHYSFS_fileLength( file ) );
      fileTime = PHYSFS_getLastModTime( filePath );

      PHYSFS_close( file );
    }
  }

#else

  PHYSFS_Stat info;

  if( !PHYSFS_stat( filePath, &info ) ) {
    fileType = File::MISSING;
    fileSize = -1;
    fileTime = 0;
  }
  else if( info.filetype == PHYSFS_FILETYPE_DIRECTORY ) {
    fileType = File::DIRECTORY;
    fileSize = -1;
    fileTime = max( info.createtime, info.modtime );
  }
  else if( info.filetype == PHYSFS_FILETYPE_REGULAR ) {
    fileType = File::REGULAR;
    fileSize = int( info.filesize );
    fileTime = max( info.createtime, info.modtime );
  }
  else {
    fileType = File::MISSING;
    fileSize = -1;
    fileTime = 0;
  }

#endif
}

PFile::PFile( const PFile& file ) :
  filePath( file.filePath ), fileType( file.fileType ), fileSize( file.fileSize ),
  fileTime( file.fileTime )
{}

PFile::PFile( PFile&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileSize( file.fileSize ), fileTime( file.fileTime )
{
  file.filePath = "";
  file.fileType = File::DIRECTORY;
  file.fileSize = -1;
  file.fileTime = 0;
}

PFile& PFile::operator = ( const PFile& file )
{
  if( &file == this ) {
    return *this;
  }

  filePath = file.filePath;
  fileType = file.fileType;
  fileSize = file.fileSize;
  fileTime = file.fileTime;

  return *this;
}

PFile& PFile::operator = ( PFile&& file )
{
  if( &file == this ) {
    return *this;
  }

  filePath = static_cast<String&&>( file.filePath );
  fileType = file.fileType;
  fileSize = file.fileSize;
  fileTime = file.fileTime;

  file.filePath = "";
  file.fileType = File::MISSING;
  file.fileSize = -1;
  file.fileTime = 0;

  return *this;
}

String PFile::realDir() const
{
  const char* realDir = PHYSFS_getRealDir( filePath );
  return realDir == nullptr ? "" : realDir;
}

String PFile::mountPoint() const
{
  const char* mountPoint = PHYSFS_getMountPoint( filePath );
  return mountPoint == nullptr ? "" : mountPoint;
}

Buffer PFile::read() const
{
  Buffer buffer;

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == nullptr ) {
    return buffer;
  }

  int size = int( PHYSFS_fileLength( file ) );
  buffer.allocate( size );

  int result = int( PHYSFS_readBytes( file, buffer.begin(), ulong64( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    buffer.deallocate();
  }
  return buffer;
}

bool PFile::write( const char* data, int size ) const
{
  PHYSFS_File* file = PHYSFS_openWrite( filePath );
  if( file == nullptr ) {
    return false;
  }

  int result = int( PHYSFS_writeBytes( file, data, ulong64( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    return false;
  }
  return true;
}

bool PFile::write( const Buffer& buffer ) const
{
  return write( buffer.begin(), buffer.length() );
}

DArray<PFile> PFile::ls() const
{
  DArray<PFile> array;

  if( fileType != File::DIRECTORY ) {
    return array;
  }

  char** list = PHYSFS_enumerateFiles( filePath );
  if( list == nullptr ) {
    return array;
  }

  // Count entries first.
  int count = 0;
  char** entity = list;
  while( *entity != nullptr ) {
    if( ( *entity )[0] != '.' ) {
      ++count;
    }
    ++entity;
  }

  if( count == 0 ) {
    PHYSFS_freeList( list );
    return array;
  }

  array.resize( count );

  String prefix = filePath.isEmpty() || filePath.equals( "/" ) ? "" : filePath + "/";

  entity = list;
  for( int i = 0; i < count; ++entity ) {
    if( ( *entity )[0] != '.' ) {
      array[i] = PFile( prefix + *entity );
      ++i;
    }
  }

  PHYSFS_freeList( list );

  array.sort();
  return array;
}

bool PFile::mkdir( const char* path )
{
  return PHYSFS_mkdir( path ) != 0;
}

bool PFile::rm( const char* path )
{
  return PHYSFS_delete( path );
}

bool PFile::mount( const char* path, const char* mountPoint, bool append )
{
  return PHYSFS_mount( path, mountPoint, append ) != 0;
}

bool PFile::mountLocal( const char* path )
{
  if( PHYSFS_setWriteDir( path ) == 0 ) {
    return false;
  }
  if( PHYSFS_mount( path, nullptr, false ) == 0 ) {
    PHYSFS_setWriteDir( nullptr );
    return false;
  }
  return true;
}

void PFile::init( File::FilesystemType type, int size )
{
#ifdef __native_client__

  pp::Module* module = pp::Module::Get();

  if( System::instance == nullptr ) {
    OZ_ERROR( "System::instance must be set prior to PhysicsFS initialisation" );
  }
  if( module->core()->IsMainThread() ) {
    OZ_ERROR( "PhysicsFS cannot be initialisation in the main thread" );
  }

  PHYSFS_NACL_init( System::instance->pp_instance(), module->get_browser_interface(),
                    type == File::PERSISTENT ? PP_FILESYSTEMTYPE_LOCALPERSISTENT :
                                               PP_FILESYSTEMTYPE_LOCALTEMPORARY,
                    size );

#else

  static_cast<void>( type );
  static_cast<void>( size );

#endif

  if( PHYSFS_init( nullptr ) == 0 ) {
    OZ_ERROR( "PhysicsFS initialisation failed: %s", PHYSFS_getLastError() );
  }
}

void PFile::destroy()
{
  PHYSFS_deinit();
}

}
