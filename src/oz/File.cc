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

#include "System.hh"

#if defined( __native_client__ )
# include <ppapi/c/pp_file_info.h>
# include <ppapi/c/ppb_file_io.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <ppapi/cpp/file_io.h>
# include <ppapi/cpp/file_ref.h>
# include <ppapi/cpp/file_system.h>
# include <cstring>
#elif defined( _WIN32 )
# include "windefs.h"
# include <windows.h>
# include <cstring>
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

struct Semaphore
{
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  volatile int    counter;
};

#define SEMAPHORE_POST() \
  pthread_mutex_lock( &_fd->semaphore.mutex ); \
  ++_fd->semaphore.counter; \
  pthread_mutex_unlock( &_fd->semaphore.mutex ); \
  pthread_cond_signal( &_fd->semaphore.cond )

#define SEMAPHORE_WAIT() \
  pthread_mutex_lock( &descriptor->semaphore.mutex ); \
  while( descriptor->semaphore.counter == 0 ) { \
    pthread_cond_wait( &descriptor->semaphore.cond, &descriptor->semaphore.mutex ); \
  } \
  --descriptor->semaphore.counter; \
  pthread_mutex_unlock( &descriptor->semaphore.mutex )

#define DEFINE_CALLBACK( name, code ) \
  struct _Callback##name \
  { \
    static void _main##name( void* _data, int _result ) \
    { \
      Descriptor* _fd = static_cast<Descriptor*>( _data ); \
      static_cast<void>( _fd ); \
      static_cast<void>( _result ); \
      code \
    } \
  }

#define CALLBACK_OBJECT( name, arg ) \
  pp::CompletionCallback( _Callback##name::_main##name, arg )

#define MAIN_CALL( name ) \
  System::core->CallOnMainThread( 0, CALLBACK_OBJECT( name, descriptor ) )

struct File::Descriptor
{
  Semaphore    semaphore;
  File*        file;
  pp::FileRef* fref;
  pp::FileIO*  fio;
  PP_FileInfo  info;
  char*        buffer;
  int          size;
  int          offset;

  explicit Descriptor( File* file_ ) :
    file( file_ )
  {
    pthread_mutex_init( &semaphore.mutex, null );
    pthread_cond_init( &semaphore.cond, null );
    semaphore.counter = 0;
  }

  ~Descriptor()
  {
    pthread_cond_destroy( &semaphore.cond );
    pthread_mutex_destroy( &semaphore.mutex );
  }
};

// Some Descriptor members are also useful for static functions.
static File::Descriptor staticDesc( null );

static pp::FileSystem* filesystem = null;

#endif // __native_client__

inline bool operator < ( const File& a, const File& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

File::File() :
  fileType( MISSING ), fileSize( -1 ), fileTime( 0 ), data( null )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
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
  filePath( file.filePath ), fileType( file.fileType ), fileSize( file.fileSize ),
  fileTime( file.fileTime ), data( null )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif
}

File::File( File&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileSize( file.fileSize ), data( file.data )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif

  file.filePath = "";
  file.fileType = MISSING;
  file.fileSize = -1;
  file.fileTime = 0;
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
  fileTime = file.fileTime;
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
  fileTime = file.fileTime;
  data     = file.data;

  file.filePath = "";
  file.fileType = MISSING;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = null;

  return *this;
}

File::File( const char* path ) :
  filePath( path ), fileType( MISSING ), fileSize( -1 ), fileTime( 0 ), data( null )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif
}

void File::setPath( const char* path )
{
  unmap();

  filePath = path;
  fileType = MISSING;
  fileSize = -1;
  fileTime = 0;
}

bool File::stat()
{
  // If file is mapped it had to be successfuly stat'd before. Futhermore fileSize must not change
  // while file is mapped as it is needed by read() function if mapped and unmap() on POSIX systems.
  if( data != null ) {
    return true;
  }

#if defined( __native_client__ )

  if( filePath.equals( "/" ) ) {
    fileType = DIRECTORY;
    fileSize = 0;
    fileTime = 0;
    return true;
  }

  fileType = MISSING;
  fileSize = -1;
  fileTime = 0;

  DEFINE_CALLBACK( queryResult, {
    if( _result == PP_OK ) {
      if( _fd->info.type == PP_FILETYPE_REGULAR ) {
        _fd->file->fileType = REGULAR;
        _fd->file->fileSize = int( _fd->info.size );
        _fd->file->fileTime = long64( max( _fd->info.creation_time,
                                           _fd->info.last_modified_time ) );
      }
      else if( _fd->info.type == PP_FILETYPE_DIRECTORY ) {
        _fd->file->fileType = DIRECTORY;
        _fd->file->fileSize = 0;
        _fd->file->fileTime = long64( max( _fd->info.creation_time,
                                           _fd->info.last_modified_time ) );
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( query, {
    if( _result == PP_OK ) {
      int ret = _fd->fio->Query( &_fd->info, CALLBACK_OBJECT( queryResult, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( open, {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ), 0,
                              CALLBACK_OBJECT( query, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( open );
  SEMAPHORE_WAIT();

#elif defined( _WIN32 )

  WIN32_FILE_ATTRIBUTE_DATA info;

  if( GetFileAttributesEx( filePath, GetFileExInfoStandard, &info ) == 0 ) {
    fileType = MISSING;
    fileSize = -1;
    fileTime = 0;
  }
  else {
    ULARGE_INTEGER creationTime = { {
      info.ftCreationTime.dwLowDateTime,
      info.ftCreationTime.dwHighDateTime
    } };
    ULARGE_INTEGER modificationTime = { {
      info.ftLastWriteTime.dwLowDateTime,
      info.ftLastWriteTime.dwHighDateTime
    } };

    fileTime = max( creationTime.QuadPart, modificationTime.QuadPart ) / 10000;
    fileSize = -1;

    if( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
      fileType = DIRECTORY;
    }
    else {
      fileType = REGULAR;

      HANDLE handle = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, null );

      if( handle != null ) {
        fileSize = int( GetFileSize( handle, null ) );

        if( fileSize == int( INVALID_FILE_SIZE ) ) {
          fileType = MISSING;
          fileSize = -1;
          fileTime = 0;
        }

        CloseHandle( handle );
      }
    }
  }

#else

  struct stat info;

  if( ::stat( filePath, &info ) != 0 ) {
    fileType = MISSING;
    fileSize = -1;
    fileTime = 0;
  }
  else if( S_ISDIR( info.st_mode ) ) {
    fileType = DIRECTORY;
    fileSize = -1;
    fileTime = long64( max( info.st_ctime, info.st_mtime ) );
  }
  else if( S_ISREG( info.st_mode ) ) {
    fileType = REGULAR;
    fileSize = int( info.st_size );
    fileTime = long64( max( info.st_ctime, info.st_mtime ) );
  }
  else {
    // Ignore files other that directories and regular files.
    fileType = MISSING;
    fileSize = -1;
    fileTime = 0;
  }

#endif

  return fileType != MISSING;
}

File::Type File::type() const
{
  return fileType;
}

long64 File::time() const
{
  return fileTime;
}

int File::size() const
{
  return fileSize;
}

const String& File::path() const
{
  return filePath;
}

String File::name() const
{
  int slash = filePath.lastIndex( '/' );

  return slash < 0 ? filePath : filePath.substring( slash + 1 );
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

  if( fileSize < 0 ) {
    stat();
  }
  if( fileSize < 0 ) {
    return false;
  }

  data = new char[fileSize];

  descriptor->buffer = data;
  descriptor->size   = fileSize;
  descriptor->offset = 0;

  DEFINE_CALLBACK( read, {
    if( _result > 0 ) {
      _fd->offset += _result;

      if( _fd->offset != _fd->size ) {
        int ret = _fd->fio->Read( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                  CALLBACK_OBJECT( read, _fd ) );
        if( ret == PP_OK_COMPLETIONPENDING ) {
          return;
        }
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( beginRead, {
    if( _result == PP_OK ) {
      int ret = _fd->fio->Read( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( read, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( open, {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ), PP_FILEOPENFLAG_READ,
                              CALLBACK_OBJECT( beginRead, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( open );
  SEMAPHORE_WAIT();

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

  int size = int( GetFileSize( file, null ) );
  if( size == int( INVALID_FILE_SIZE ) ) {
    CloseHandle( file );
    return false;
  }

  HANDLE mapping = CreateFileMapping( file, null, PAGE_READONLY, 0, 0, null );
  if( mapping == null ) {
    CloseHandle( file );
    return false;
  }

  data = static_cast<char*>( MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) );

  CloseHandle( mapping );
  CloseHandle( file );

  if( data == null ) {
    return false;
  }

#else

  int fd = open( filePath, O_RDONLY );
  if( fd < 0 ) {
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

  if( fileSize < 0 ) {
    stat();
  }
  if( fileSize < 0 ) {
    return buffer;
  }

  buffer.alloc( fileSize );

  descriptor->buffer = buffer.begin();
  descriptor->size   = fileSize;
  descriptor->offset = 0;

  DEFINE_CALLBACK( read, {
    if( _result > 0 ) {
      _fd->offset += _result;

      if( _fd->offset != _fd->size ) {
        int ret = _fd->fio->Read( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                  CALLBACK_OBJECT( read, _fd ) );
        if( ret == PP_OK_COMPLETIONPENDING ) {
          return;
        }
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( beginRead, {
    if( _result == PP_OK ) {
      int ret = _fd->fio->Read( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( read, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( open, {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ), PP_FILEOPENFLAG_READ,
                              CALLBACK_OBJECT( beginRead, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( open );
  SEMAPHORE_WAIT();

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
  if( size <= 0 || size == int( INVALID_FILE_SIZE ) ) {
    return buffer;
  }

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
  if( fd < 0 ) {
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

  DEFINE_CALLBACK( write, {
    if( _result > 0 ) {
      _fd->offset += _result;

      if( _fd->offset != _fd->size ) {
        int ret = _fd->fio->Write( _fd->offset, &_fd->buffer[_fd->offset], _fd->size - _fd->offset,
                                   CALLBACK_OBJECT( write, _fd ) );
        if( ret == PP_OK_COMPLETIONPENDING ) {
          return;
        }
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( beginWrite, {
    if( _result == PP_OK ) {
      int ret = _fd->fio->Write( 0, _fd->buffer, _fd->size, CALLBACK_OBJECT( write, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( open, {
    _fd->fio = new pp::FileIO( System::instance );

    int ret = _fd->fio->Open( pp::FileRef( *filesystem, _fd->file->filePath ),
                              PP_FILEOPENFLAG_WRITE | PP_FILEOPENFLAG_CREATE | PP_FILEOPENFLAG_TRUNCATE,
                              CALLBACK_OBJECT( beginWrite, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fio;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( open );
  SEMAPHORE_WAIT();

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
  if( fd < 0 ) {
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

  array.resize( count );

  int i = 0;

  if( entity.cFileName[i] != '.' ) {
    array[i].filePath = filePath + "/" + entity.cFileName;
    ++i;
  }

  while( i < count ) {
    if( FindNextFile( dir, &entity ) == 0 ) {
      CloseHandle( dir );
      array.clear();
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

  array.resize( count );

  String prefix = filePath.isEmpty() || filePath.equals( "/" ) ? "" : filePath + "/";

  for( int i = 0; i < count; ) {
    entity = readdir( directory );

    if( entity == null ) {
      closedir( directory );
      array.clear();
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

  Descriptor localDescriptor( null );
  Descriptor* descriptor = &localDescriptor;

  // Abuse buffer for file path and size for result.
  descriptor->buffer = const_cast<char*>( path );
  descriptor->size = false;

  DEFINE_CALLBACK( mkdirResult, {
    if( _result == PP_OK ) {
      _fd->size = true;
    }

    delete _fd->fref;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( mkdir, {
    _fd->fref = new pp::FileRef( *filesystem, _fd->buffer );

    int ret = _fd->fref->MakeDirectory( CALLBACK_OBJECT( mkdirResult, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fref;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( mkdir );
  SEMAPHORE_WAIT();

  return descriptor->size != 0;

#elif defined( _WIN32 )

  return CreateDirectory( path, null ) != 0;

#else

  return ::mkdir( path, 0755 ) == 0;

#endif
}

bool File::rm( const char* path )
{
#if defined( __native_client__ )

  Descriptor localDescriptor( null );
  Descriptor* descriptor = &localDescriptor;

  // Abuse buffer for file path and size for result.
  descriptor->buffer = const_cast<char*>( path );
  descriptor->size = false;

  DEFINE_CALLBACK( rmResult, {
    if( _result == PP_OK ) {
      _fd->size = true;
    }

    delete _fd->fref;
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( rm, {
    _fd->fref = new pp::FileRef( *filesystem, _fd->buffer );

    int ret = _fd->fref->Delete( CALLBACK_OBJECT( rmResult, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete _fd->fref;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( rm );
  SEMAPHORE_WAIT();

  return descriptor->size != 0;

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

  Descriptor* descriptor = &staticDesc;

  DEFINE_CALLBACK( initResult, {
    if( _result != PP_OK ) {
      delete filesystem;
      filesystem = null;
    }
    SEMAPHORE_POST();
  } );
  DEFINE_CALLBACK( init, {
    filesystem = new pp::FileSystem( System::instance, PP_FileSystemType( _fd->offset ) );

    int ret = filesystem->Open( _fd->size, CALLBACK_OBJECT( initResult, _fd ) );
    if( ret == PP_OK_COMPLETIONPENDING ) {
      return;
    }

    delete filesystem;
    filesystem = null;
    SEMAPHORE_POST();
  } );

  MAIN_CALL( init );
  SEMAPHORE_WAIT();

  if( filesystem == null ) {
    throw Exception( "Local filesystem open failed" );
  }

#else

  static_cast<void>( type );
  static_cast<void>( size );

#endif
}

void File::free()
{
#ifdef __native_client__

  if( filesystem != null ) {
    Descriptor* descriptor = &staticDesc;

    DEFINE_CALLBACK( free, {
      delete filesystem;
      filesystem = null;

      SEMAPHORE_POST();
    } );

    MAIN_CALL( free );
    SEMAPHORE_WAIT();
  }

#endif
}

}
