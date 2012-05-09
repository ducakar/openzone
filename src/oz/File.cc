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
# include "System.hh"

# include <cstring>
# include <ppapi/c/pp_file_info.h>
# include <ppapi/c/ppb_file_io.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <ppapi/cpp/file_io.h>
# include <ppapi/cpp/file_ref.h>
# include <ppapi/cpp/file_system.h>
#elif defined( _WIN32 )
# include "windefs.h"
# include <cstring>
# include <windows.h>
#else
# include <cstring>
# include <dirent.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <unistd.h>
#endif

namespace oz
{

#ifdef __native_client__

struct Barrier
{
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  volatile int    counter;
};

#define BARRIER_INCREMENT( barrier ) \
  pthread_mutex_lock( &barrier.mutex ); \
  ++barrier.counter; \
  pthread_mutex_unlock( &barrier.mutex ); \
  pthread_cond_signal( &barrier.cond );

#define BARRIER_WAIT( barrier ) \
  pthread_mutex_lock( &barrier.mutex ); \
  while( barrier.counter == 0 ) { \
    pthread_cond_wait( &barrier.cond, &barrier.mutex ); \
  } \
  --barrier.counter; \
  pthread_mutex_unlock( &barrier.mutex );

#define DEFINE_CALLBACK( name, code ) \
  struct _Callback##name \
  { \
    static void _main##name( void* _data, int _result ) \
    { \
      FileDesc* _fd = static_cast<FileDesc*>( _data ); \
      static_cast<void>( _fd ); \
      static_cast<void>( _result ); \
      code \
    } \
  };

#define CALLBACK_OBJECT( name, arg ) \
  pp::CompletionCallback( _Callback##name::_main##name, arg )

#define MAIN_CALL( code ) \
  { \
    DEFINE_CALLBACK( Main, code BARRIER_INCREMENT( _fd->mainBarrier ) ) \
    System::core->CallOnMainThread( 0, CALLBACK_OBJECT( Main, descriptor ) ); \
    BARRIER_WAIT( descriptor->mainBarrier ) \
  }

struct FileDesc
{
  Barrier     mainBarrier;
  Barrier     auxBarrier;
  File*       file;
  pp::FileIO* fio;
  PP_FileInfo info;
  char*       buffer;
  int         size;
  int         offset;

  explicit FileDesc( File* file_ ) :
    file( file_ )
  {
    pthread_mutex_init( &mainBarrier.mutex, null );
    pthread_cond_init( &mainBarrier.cond, null );
    mainBarrier.counter = 0;

    pthread_mutex_init( &auxBarrier.mutex, null );
    pthread_cond_init( &auxBarrier.cond, null );
    auxBarrier.counter = 0;
  }

  ~FileDesc()
  {
    pthread_cond_destroy( &auxBarrier.cond );
    pthread_mutex_destroy( &auxBarrier.mutex );

    pthread_cond_destroy( &mainBarrier.cond );
    pthread_mutex_destroy( &mainBarrier.mutex );
  }
};

// Some FileDesc members are also useful for static functions.
static FileDesc staticDesc( null );

static pp::FileSystem* filesystem = null;

DEFINE_CALLBACK( WaitMain, {
  BARRIER_INCREMENT( _fd->mainBarrier )
} )

DEFINE_CALLBACK( WaitAux, {
  BARRIER_INCREMENT( _fd->auxBarrier )
} )

#endif // __native_client__

inline bool operator < ( const File& a, const File& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

File::File() :
  fileType( MISSING ), fileSize( -1 ), data( null )
{
#ifdef __native_client__
  descriptor = new FileDesc( this );
#endif
}

File::~File()
{
  unmap();

#ifdef __native_client__
  delete descriptor;
#endif
}

File::File( const File& file ) :
  filePath( file.filePath ), fileType( file.fileType ), fileSize( file.fileSize ), data( null )
{
#ifdef __native_client__
  descriptor = new FileDesc( this );
#endif
}

File::File( File&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileSize( file.fileSize ), data( file.data )
{
#ifdef __native_client__
  descriptor = new FileDesc( this );
#endif

  file.filePath = "";
  file.fileType = MISSING;
  file.fileSize = -1;
  file.data     = null;
}

File& File::operator = ( const File& file )
{
  if( &file == this ) {
    return *this;
  }

  unmap();

  filePath = file.filePath;
  fileType = file.fileType;
  fileSize = file.fileSize;
  data     = null;

  return *this;
}

File& File::operator = ( File&& file )
{
  if( &file == this ) {
    return *this;
  }

  unmap();

  filePath = static_cast<String&&>( file.filePath );
  fileType = file.fileType;
  fileSize = file.fileSize;
  data     = file.data;

  file.filePath = "";
  file.fileType = MISSING;
  file.fileSize = -1;
  file.data     = null;

  return *this;
}

File::File( const char* path ) :
  filePath( path ), fileType( MISSING ), fileSize( -1 ), data( null )
{
#ifdef __native_client__
  descriptor = new FileDesc( this );
#endif
}

void File::setPath( const char* path )
{
  unmap();

  filePath = path;
  fileType = MISSING;
  fileSize = -1;
}

bool File::stat()
{
  // If file is mapped it had to be successfuly stat'd before. Futhermore fileSize must not change
  // while file is mapped as it is needed by read() function if mapped and unmap() on POSIX systems.
  if( data != null ) {
    return true;
  }

#if defined( __native_client__ )

  fileType = MISSING;
  fileSize = -1;

  DEFINE_CALLBACK( Result, {
    if( _result == PP_OK ) {
      if( _fd->info.type == PP_FILETYPE_REGULAR ) {
        _fd->file->fileType = REGULAR;
        _fd->file->fileSize = int( _fd->info.size );
      }
      else if( _fd->info.type == PP_FILETYPE_DIRECTORY ) {
        _fd->file->fileType = DIRECTORY;
      }
    }
    BARRIER_INCREMENT( _fd->auxBarrier )
  } )
  DEFINE_CALLBACK( Query, {
    if( _result == PP_OK ) {
      _fd->fio->Query( &_fd->info, CALLBACK_OBJECT( Result, _fd ) );
    }
    else {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  MAIN_CALL( {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ),
                              0, CALLBACK_OBJECT( Query, _fd ) );
    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  BARRIER_WAIT( descriptor->auxBarrier )

  MAIN_CALL( {
    delete _fd->fio;
  } )

#elif defined( _WIN32 )

  DWORD attributes = GetFileAttributes( filePath );

  if( attributes == INVALID_FILE_ATTRIBUTES ) {
    fileType = MISSING;
    fileSize = -1;
  }
  else if( attributes & FILE_ATTRIBUTE_DIRECTORY ) {
    fileType = DIRECTORY;
    fileSize = -1;
  }
  else {
    fileType = REGULAR;
    fileSize = -1;

    HANDLE handle = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, null );
    if( handle != null ) {
      fileSize = int( GetFileSize( handle, null ) );

      CloseHandle( handle );
    }
  }

#else

  struct ::stat info;

  if( ::stat( filePath, &info ) != 0 ) {
    fileType = MISSING;
    fileSize = -1;
  }
  else if( S_ISDIR( info.st_mode ) ) {
    fileType = DIRECTORY;
    fileSize = -1;
  }
  else if( !S_ISREG( info.st_mode ) ) {
    fileType = MISSING;
    fileSize = -1;
  }
  else {
    fileType = REGULAR;
    fileSize = int( info.st_size );
  }

#endif

  return fileType != MISSING;
}

File::Type File::type() const
{
  return fileType;
}

int File::size() const
{
  return fileSize;
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
    return true;
  }

#if defined( __native_client__ )

  if( fileSize == -1 ) {
    stat();
  }
  if( fileSize == -1 ) {
    return false;
  }

  data = new char[fileSize];

  descriptor->buffer = data;
  descriptor->size   = fileSize;
  descriptor->offset = 0;

  DEFINE_CALLBACK( Read, {
    if( _result <= 0 ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
    else {
      _fd->offset += _result;

      if( _fd->offset == _fd->size ) {
        BARRIER_INCREMENT( _fd->auxBarrier )
      }
      else {
        int ret = _fd->fio->Read( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                  CALLBACK_OBJECT( Read, _fd ) );

        if( ret != PP_OK_COMPLETIONPENDING ) {
          BARRIER_INCREMENT( _fd->auxBarrier )
        }
      }
    }
  } )
  DEFINE_CALLBACK( BeginRead, {
    int ret = _fd->fio->Read( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( Read, _fd ) );
    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  MAIN_CALL( {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ),
                              PP_FILEOPENFLAG_READ, CALLBACK_OBJECT( BeginRead, _fd ) );

    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  BARRIER_WAIT( descriptor->auxBarrier )

  MAIN_CALL( {
    delete _fd->fio;
  } )

  if( descriptor->offset < fileSize ) {
    delete[] data;
    data = null;
    return false;
  }

  int size = fileSize;

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

  int size = int( GetFileSize( mapping, null ) );
  data = static_cast<char*>( MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) );

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

  int size = int( statInfo.st_size );
  data = static_cast<char*>( mmap( null, size_t( statInfo.st_size ),
                                   PROT_READ, MAP_SHARED, fd, 0 ) );
  close( fd );

  if( data == MAP_FAILED ) {
    data = null;
    return false;
  }

#endif

  fileType = REGULAR;
  fileSize = size;
  return true;
}

void File::unmap()
{
  if( data != null ) {
#if defined( __native_client__ )
    delete[] data;
#elif defined( _WIN32 )
    UnmapViewOfFile( data );
#else
    munmap( data, size_t( fileSize ) );
#endif
    data = null;
  }
}

InputStream File::inputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return InputStream( data, data + fileSize, order );
}

Buffer File::read()
{
  Buffer buffer;

  if( data != null ) {
    buffer.alloc( fileSize );
    memcpy( buffer.begin(), data, size_t( fileSize ) );
    return buffer;
  }

#if defined( __native_client__ )

  if( fileSize == -1 ) {
    stat();
  }
  if( fileSize == -1 ) {
    return buffer;
  }

  buffer.alloc( fileSize );

  descriptor->buffer = buffer.begin();
  descriptor->size   = fileSize;
  descriptor->offset = 0;

  DEFINE_CALLBACK( Read, {
    if( _result <= 0 ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
    else {
      _fd->offset += _result;

      if( _fd->offset == _fd->size ) {
        BARRIER_INCREMENT( _fd->auxBarrier )
      }
      else {
        int ret = _fd->fio->Read( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                  CALLBACK_OBJECT( Read, _fd ) );

        if( ret != PP_OK_COMPLETIONPENDING ) {
          BARRIER_INCREMENT( _fd->auxBarrier )
        }
      }
    }
  } )
  DEFINE_CALLBACK( BeginRead, {
    int ret = _fd->fio->Read( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( Read, _fd ) );

    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  MAIN_CALL( {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ),
                              PP_FILEOPENFLAG_READ, CALLBACK_OBJECT( BeginRead, _fd ) );

    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  BARRIER_WAIT( descriptor->auxBarrier )

  MAIN_CALL( {
    delete _fd->fio;
  } )

  if( descriptor->offset < fileSize ) {
    buffer.dealloc();
    return buffer;
  }

  int size = fileSize;

#elif defined( _WIN32 )

  HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, null );
  if( file == null ) {
    return buffer;
  }

  int size = int( GetFileSize( file, null ) );
  buffer.alloc( size );

  DWORD read;
  BOOL result = ReadFile( file, buffer.begin(), DWORD( size ), &read, null );
  CloseHandle( file );

  if( result == 0 || int( read ) != size ) {
    buffer.dealloc();
    return buffer;
  }

#else

  int fd = open( filePath, O_RDONLY );
  if( fd == -1 ) {
    return buffer;
  }

  struct stat statInfo;
  if( fstat( fd, &statInfo ) != 0 ) {
    close( fd );
    return buffer;
  }

  int size = int( statInfo.st_size );
  buffer.alloc( size );

  int result = int( ::read( fd, buffer.begin(), size_t( size ) ) );
  close( fd );

  if( result != size ) {
    buffer.dealloc();
    return buffer;
  }

#endif

  fileType = REGULAR;
  fileSize = size;
  return buffer;
}

bool File::write( const char* buffer, int size )
{
  if( data != null ) {
    return false;
  }

#if defined( __native_client__ )

  descriptor->buffer = const_cast<char*>( buffer );
  descriptor->size   = size;
  descriptor->offset = 0;

  DEFINE_CALLBACK( Write, {
    if( _result <= 0 ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
    else {
      _fd->offset += _result;

      if( _fd->offset == _fd->size ) {
        BARRIER_INCREMENT( _fd->auxBarrier )
      }
      else {
       int ret =  _fd->fio->Write( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                   CALLBACK_OBJECT( Write, _fd ) );

        if( ret != PP_OK_COMPLETIONPENDING ) {
          BARRIER_INCREMENT( _fd->auxBarrier )
        }
      }
    }
  } )
  DEFINE_CALLBACK( BeginWrite, {
    int ret = _fd->fio->Write( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( Write, _fd ) );

    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  MAIN_CALL( {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ),
                              PP_FILEOPENFLAG_WRITE | PP_FILEOPENFLAG_CREATE | PP_FILEOPENFLAG_TRUNCATE,
                              CALLBACK_OBJECT( BeginWrite, _fd ) );

    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
    }
  } )

  BARRIER_WAIT( descriptor->auxBarrier )

  MAIN_CALL( {
    delete _fd->fio;
  } )

  if( descriptor->offset < size ) {
    return false;
  }

#elif defined( _WIN32 )

  HANDLE file = CreateFile( filePath, GENERIC_WRITE, 0, null, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, null );
  if( file == null ) {
    return false;
  }

  DWORD written;
  BOOL result = WriteFile( file, buffer, DWORD( size ), &written, null );
  CloseHandle( file );

  if( result == 0 || int( written ) != size ) {
    return false;
  }

#else

  int fd = open( filePath, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
  if( fd == -1 ) {
    return false;
  }

  int result = int( ::write( fd, buffer, size_t( size ) ) );
  close( fd );

  if( result != size ) {
    return false;
  }

#endif

  fileType = REGULAR;
  fileSize = size;
  return true;
}

bool File::write( const Buffer* buffer )
{
  return write( buffer->begin(), buffer->length() );
}

String File::cwd()
{
#if defined( __native_client__ )

  return "";

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

  return false;

#elif defined( _WIN32 )

  return SetCurrentDirectory( path ) != 0;

#else

  return ::chdir( path ) == 0;

#endif
}

DArray<File> File::ls()
{
  DArray<File> array;

  if( fileType == MISSING ) {
    stat();
  }
  if( fileType != DIRECTORY ) {
    return array;
  }

#if defined( __native_client__ )
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

  return false;

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

  return false;

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

void File::init( FilesystemType type, int size )
{
#ifdef __native_client__

  if( System::instance == null ) {
    throw Exception( "NaClModule::instance must be set to NaCl module pointer in order to "
                     "initialise NaCl filesystem" );
  }

  free();

  // We abuse staticDesc.size and staticDesc.offset variables to pass filesystem type and size to
  // callback.
  staticDesc.size   = size;
  staticDesc.offset = type == PERSISTENT ? PP_FILESYSTEMTYPE_LOCALPERSISTENT :
                                           PP_FILESYSTEMTYPE_LOCALTEMPORARY;

  FileDesc* descriptor = &staticDesc;
  MAIN_CALL( {
    filesystem = new pp::FileSystem( System::instance, PP_FileSystemType( _fd->offset ) );

    int ret = filesystem->Open( _fd->size, CALLBACK_OBJECT( WaitAux, _fd ) );
    if( ret != PP_OK_COMPLETIONPENDING ) {
      BARRIER_INCREMENT( _fd->auxBarrier )
      throw Exception( "Local filesystem open failed" );
    }
  } )
  BARRIER_WAIT( staticDesc.auxBarrier )

#else

  static_cast<void>( type );
  static_cast<void>( size );

#endif
}

void File::free()
{
#ifdef __native_client__

  if( filesystem != null ) {
    FileDesc* descriptor = &staticDesc;
    MAIN_CALL( {
      delete filesystem;
      filesystem = null;
    } )
  }

#endif
}

}
