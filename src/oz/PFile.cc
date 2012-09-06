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
 * @file oz/PFile.cc
 */

#include "PFile.hh"

#include <cerrno>
#include <cstring>
#include <physfs.h>

#ifdef __native_client__
# include <ppapi/c/ppb.h>
# include <ppapi/cpp/file_system.h>
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/module.h>
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

PFile::PFile() :
  fileType( File::MISSING ), fileSize( -1 ), fileTime( 0 ), data( nullptr )
{}

PFile::~PFile()
{
  delete[] data;
}

PFile::PFile( const PFile& file ) :
  filePath( file.filePath ), fileType( file.fileType ), fileSize( file.fileSize ),
  fileTime( file.fileTime ), data( nullptr )
{}

PFile::PFile( PFile&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileSize( file.fileSize ), fileTime( file.fileTime ), data( file.data )
{
  file.filePath = "";
  file.fileType = File::DIRECTORY;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = nullptr;
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
  data     = nullptr;

  return *this;
}

PFile& PFile::operator = ( PFile&& file )
{
  if( &file == this ) {
    return *this;
  }

  delete[] data;

  filePath = static_cast<String&&>( file.filePath );
  fileType = file.fileType;
  fileSize = file.fileSize;
  fileTime = file.fileTime;
  data     = file.data;

  file.filePath = "";
  file.fileType = File::MISSING;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = nullptr;

  return *this;
}

PFile::PFile( const char* path ) :
  filePath( path ), fileType( File::MISSING ), fileSize( -1 ), fileTime( 0 ), data( nullptr )
{}

void PFile::setPath( const char* path )
{
  delete[] data;

  filePath = path;
  fileType = File::MISSING;
  fileSize = -1;
  fileTime = 0;
  data     = nullptr;
}

bool PFile::stat()
{
  unmap();

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

  return fileType != File::MISSING;
}

File::Type PFile::type() const
{
  return fileType;
}

long64 PFile::time() const
{
  return fileTime;
}

int PFile::size() const
{
  return fileSize;
}

const String& PFile::path() const
{
  return filePath;
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

String PFile::name() const
{
  int slash = filePath.lastIndex( '/' );

  return slash < 0 ? filePath : filePath.substring( slash + 1 );
}

String PFile::extension() const
{
  int slash = filePath.lastIndex( '/' );
  int dot   = filePath.lastIndex( '.' );

  return slash < dot ? filePath.substring( dot + 1 ) : String();
}

String PFile::baseName() const
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

bool PFile::hasExtension( const char* ext ) const
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

bool PFile::isMapped() const
{
  return data != nullptr;
}

bool PFile::map()
{
  if( data != nullptr ) {
    return true;
  }

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == nullptr ) {
    return false;
  }

  int size = int( PHYSFS_fileLength( file ) );
  data = new char[size];

  int result = int( PHYSFS_readBytes( file, data, ulong64( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    delete[] data;
    data = nullptr;
    return false;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return true;
}

void PFile::unmap()
{
  if( data != nullptr ) {
    delete[] data;
    data = nullptr;
  }
}

InputStream PFile::inputStream( Endian::Order order ) const
{
  hard_assert( data != nullptr );

  return InputStream( data, data + fileSize, order );
}

Buffer PFile::read()
{
  Buffer buffer;

  if( data != nullptr ) {
    buffer.alloc( fileSize );
    memcpy( buffer.begin(), data, size_t( fileSize ) );
    return buffer;
  }

  PHYSFS_File* file = PHYSFS_openRead( filePath );
  if( file == nullptr ) {
    return buffer;
  }

  int size = int( PHYSFS_fileLength( file ) );
  buffer.alloc( size );

  int result = int( PHYSFS_readBytes( file, buffer.begin(), ulong64( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    buffer.dealloc();
    return buffer;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return buffer;
}

bool PFile::write( const char* buffer, int size )
{
  if( data != nullptr ) {
    return false;
  }

  PHYSFS_File* file = PHYSFS_openWrite( filePath );
  if( file == nullptr ) {
    return buffer;
  }

  int result = int( PHYSFS_writeBytes( file, buffer, ulong64( size ) ) );
  PHYSFS_close( file );

  if( result != size ) {
    return false;
  }

  fileType = File::REGULAR;
  fileSize = size;
  return true;
}

bool PFile::write( const Buffer* buffer )
{
  return write( buffer->begin(), buffer->length() );
}

DArray<PFile> PFile::ls()
{
  DArray<PFile> array;

  stat();

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
      array[i].setPath( prefix + *entity );
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
  static_cast<void>( type );
  static_cast<void>( size );

  const char* arg0;

#if defined( __native_client__ )

  if( System::instance == nullptr ) {
    OZ_ERROR( "System::instance must be set to initialise PhysicsFS" );
  }

  struct InstanceInfo
  {
    PPB_GetInterface  getInterface;
    PP_Instance       instance;
    PP_FileSystemType filesystemType;
    size_t            size;
  };

  InstanceInfo instanceInfo = {
    pp::Module::Get()->get_browser_interface(),
    System::instance->pp_instance(),
    type == File::PERSISTENT ? PP_FILESYSTEMTYPE_LOCALPERSISTENT : PP_FILESYSTEMTYPE_LOCALTEMPORARY,
    size
  };

  arg0 = reinterpret_cast<const char*>( &instanceInfo );

#elif defined( __ANDROID__ ) || defined( _WIN32 )

  arg0 = nullptr;

#else

  arg0 = program_invocation_name;

#endif

  if( PHYSFS_init( arg0 ) == 0 ) {
    OZ_ERROR( "PhysicsFS initialisation failed: %s", PHYSFS_getLastError() );
  }
}

void PFile::free()
{
  PHYSFS_deinit();
}

}
