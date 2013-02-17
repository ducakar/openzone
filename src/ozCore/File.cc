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
 * @file ozCore/File.cc
 */

#include "File.hh"

#if defined( __native_client__ )
# include <ppapi/c/pp_file_info.h>
# include <ppapi/c/ppb_file_io.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <ppapi/cpp/file_io.h>
# include <ppapi/cpp/file_ref.h>
# include <ppapi/cpp/file_system.h>
# include <pthread.h>
#elif defined( _WIN32 )
# include <windows.h>
#else
# include <dirent.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <unistd.h>
#endif

#include <physfs.h>

#if PHYSFS_VER_MAJOR < 2
# error PhysicsFS version must be at least 2.0.
#elif PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0
# define PHYSFS_readBytes( handle, buffer, len )  PHYSFS_read( handle, buffer, 1, uint( len ) )
# define PHYSFS_writeBytes( handle, buffer, len ) PHYSFS_write( handle, buffer, 1, uint( len ) )
#endif

#ifdef __native_client__
extern "C"
int PHYSFS_NACL_init( PP_Instance instance, PPB_GetInterface getInterface,
                      PP_FileSystemType fileSystemType, PHYSFS_sint64 size );
#endif

namespace oz
{

#ifdef __native_client__

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
  ppCore->CallOnMainThread( 0, CALLBACK_OBJECT( name, descriptor ) )

struct File::Descriptor
{
  struct Semaphore
  {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    volatile int    counter;
  };

  // Some Descriptor members are also useful for static functions.
  static Descriptor staticDesc;

  Semaphore         semaphore;
  File*             file;
  pp::FileRef*      fref;
  pp::FileIO*       fio;
  PP_FileInfo       info;
  char*             buffer;
  int               size;
  int               offset;

  OZ_HIDDEN
  explicit Descriptor( File* file_ ) :
    file( file_ )
  {
    pthread_mutex_init( &semaphore.mutex, nullptr );
    pthread_cond_init( &semaphore.cond, nullptr );
    semaphore.counter = 0;
  }

  OZ_HIDDEN
  ~Descriptor()
  {
    pthread_cond_destroy( &semaphore.cond );
    pthread_mutex_destroy( &semaphore.mutex );
  }
};

OZ_HIDDEN
File::Descriptor File::Descriptor::staticDesc( nullptr );

static pp::Core*       ppCore       = nullptr;
static pp::FileSystem* ppFileSystem = nullptr;

#endif // __native_client__

static bool operator < ( const File& a, const File& b )
{
  return String::compare( a.path(), b.path() ) < 0;
}

File::File( FileSystem fileSystem, const char* path ) :
  filePath( path ), fileType( MISSING ), fileFS( fileSystem ), fileSize( -1 ), fileTime( 0 ),
  data( nullptr )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif

  // Avoid stat'ing obviously non-existent files.
  if( filePath.isEmpty() ) {
    return;
  }

  stat();
}

File::~File()
{
  unmap();

#ifdef __native_client__
  delete descriptor;
#endif
}

File::File( const File& file ) :
  filePath( file.filePath ), fileType( file.fileType ), fileFS( file.fileFS ),
  fileSize( file.fileSize ), fileTime( file.fileTime ), data( nullptr )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif
}

File::File( File&& file ) :
  filePath( static_cast<String&&>( file.filePath ) ), fileType( file.fileType ),
  fileFS( file.fileFS ), fileSize( file.fileSize ), fileTime( file.fileTime ), data( file.data )
{
#ifdef __native_client__
  descriptor = new Descriptor( this );
#endif

  file.filePath = "";
  file.fileType = MISSING;
  file.fileFS   = NATIVE;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = nullptr;
}

File& File::operator = ( const File& file )
{
  if( &file == this ) {
    return *this;
  }

  unmap();

  filePath = file.filePath;
  fileType = file.fileType;
  fileFS   = file.fileFS;
  fileSize = file.fileSize;
  fileTime = file.fileTime;

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
  fileFS   = file.fileFS;
  fileSize = file.fileSize;
  fileTime = file.fileTime;
  data     = file.data;

  file.filePath = "";
  file.fileType = MISSING;
  file.fileFS   = NATIVE;
  file.fileSize = -1;
  file.fileTime = 0;
  file.data     = nullptr;

  return *this;
}

bool File::stat()
{
  if( fileFS == VIRTUAL ) {
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
  else {
#if defined( __native_client__ )

    fileType = MISSING;
    fileSize = -1;
    fileTime = 0;

    if( filePath.equals( "/" ) ) {
      fileType = DIRECTORY;
      return true;
    }

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
          _fd->file->fileSize = -1;
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

      int ret = _fd->fio->Open( pp::FileRef( *ppFileSystem, _fd->file->filePath ), 0,
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
      ULARGE_INTEGER creationTime = {
        {
          info.ftCreationTime.dwLowDateTime,
          info.ftCreationTime.dwHighDateTime
        }
      };
      ULARGE_INTEGER modificationTime = {
        {
          info.ftLastWriteTime.dwLowDateTime,
          info.ftLastWriteTime.dwHighDateTime
        }
      };

      fileTime = max( creationTime.QuadPart, modificationTime.QuadPart ) / 10000;
      fileSize = -1;

      if( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
        fileType = DIRECTORY;
      }
      else {
        fileType = REGULAR;

        HANDLE handle = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr );

        if( handle != nullptr ) {
          fileSize = int( GetFileSize( handle, nullptr ) );

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
  }

  return fileType != MISSING;
}

String File::realDir() const
{
  if( fileFS == VIRTUAL ) {
    const char* realDir = PHYSFS_getRealDir( filePath );
    return realDir == nullptr ? "" : realDir;
  }
  else {
    return "";
  }
}

bool File::map()
{
  if( fileSize < 0 ) {
    return false;
  }
  if( data != nullptr ) {
    return true;
  }

  if( fileFS == VIRTUAL ) {
    int size = fileSize;

    data = new char[size];
    return read( data, &size );
  }
  else {
#if defined( __native_client__ )

    int size = fileSize;

    data = new char[fileSize];
    return read( data, &size );

#elif defined( _WIN32 )

    HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr );
    if( file == nullptr ) {
      return false;
    }

    HANDLE mapping = CreateFileMapping( file, nullptr, PAGE_READONLY, 0, 0, nullptr );
    if( mapping == nullptr ) {
      CloseHandle( file );
      return false;
    }

    data = static_cast<char*>( MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 ) );

    CloseHandle( mapping );
    CloseHandle( file );

    return data != nullptr;

#else

    int fd = open( filePath, O_RDONLY );
    if( fd < 0 ) {
      return false;
    }

    data = static_cast<char*>( mmap( nullptr, size_t( fileSize ), PROT_READ, MAP_SHARED, fd, 0 ) );
    data = data == MAP_FAILED ? nullptr : data;
    close( fd );

    return data != nullptr;

#endif
  }
}

void File::unmap()
{
  if( data == nullptr ) {
    return;
  }

  if( fileFS == VIRTUAL ) {
    delete[] data;
  }
  else {
#if defined( __native_client__ )
    delete[] data;
#elif defined( _WIN32 )
    UnmapViewOfFile( data );
#else
    munmap( data, size_t( fileSize ) );
#endif
  }
  data = nullptr;
}

InputStream File::inputStream( Endian::Order order ) const
{
  hard_assert( data != nullptr );

  return InputStream( data, data + fileSize, order );
}

bool File::read( char* buffer, int* size ) const
{
  if( fileSize <= 0 ) {
    *size = 0;
    return fileSize == 0;
  }

  if( fileFS == VIRTUAL ) {
    PHYSFS_File* file = PHYSFS_openRead( filePath );
    if( file == nullptr ) {
      *size = 0;
      return false;
    }

    int result = int( PHYSFS_readBytes( file, buffer, ulong64( *size ) ) );
    PHYSFS_close( file );

    *size = result;
  }
  else {
    if( data != nullptr ) {
      *size = min( *size, fileSize );
      mCopy( buffer, data, size_t( *size ) );
      return true;
    }

#if defined( __native_client__ )

    descriptor->buffer = buffer;
    descriptor->size   = min( *size, fileSize );
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

      int ret = _fd->fio->Open( pp::FileRef( *ppFileSystem, _fd->file->filePath ),
                                PP_FILEOPENFLAG_READ, CALLBACK_OBJECT( beginRead, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }

      delete _fd->fio;
      SEMAPHORE_POST();
    } );

    MAIN_CALL( open );
    SEMAPHORE_WAIT();

    if( descriptor->offset != descriptor->size ) {
      *size = 0;
      return false;
    }

    *size = descriptor->offset;

#elif defined( _WIN32 )

    HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr );
    if( file == nullptr ) {
      *size = 0;
      return false;
    }

    DWORD read;
    BOOL result = ReadFile( file, buffer, DWORD( size ), &read, nullptr );
    CloseHandle( file );

    if( result == 0 ) {
      *size = 0;
      return false;
    }

    *size = int( read );

#else

    int fd = open( filePath, O_RDONLY );
    if( fd < 0 ) {
      *size = 0;
      return false;
    }

    int result = int( ::read( fd, buffer, size_t( *size ) ) );
    close( fd );

    *size = result;

#endif
  }

  return true;
}

bool File::read( OutputStream* ostream ) const
{
  int   size   = fileSize;
  char* buffer = ostream->forward( fileSize );
  bool  result = read( buffer, &size );

  ostream->set( buffer + size );
  return result;
}

Buffer File::read() const
{
  Buffer buffer;

  if( fileSize <= 0 ) {
    return buffer;
  }

  int size = fileSize;
  buffer.allocate( size );

  read( buffer.begin(), &size );

  if( size != fileSize ) {
    buffer.resize( size );
  }
  return buffer;
}

String File::readString() const
{
  char*  buffer;
  int    size = fileSize;
  String s    = String::create( size, &buffer );

  read( buffer, &size );
  buffer[size] = '\0';
  return s;
}

bool File::write( const char* buffer, int size ) const
{
  if( fileFS == VIRTUAL ) {
    PHYSFS_File* file = PHYSFS_openWrite( filePath );
    if( file == nullptr ) {
      return false;
    }

    int result = int( PHYSFS_writeBytes( file, data, ulong64( size ) ) );
    PHYSFS_close( file );

    if( result != size ) {
      return false;
    }
  }
  else {
    if( data != nullptr ) {
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
          int ret = _fd->fio->Write( _fd->offset, &_fd->buffer[_fd->offset],
                                     _fd->size - _fd->offset, CALLBACK_OBJECT( write, _fd ) );
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

      int ret = _fd->fio->Open( pp::FileRef( *ppFileSystem, _fd->file->filePath ),
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

    if( descriptor->offset != size ) {
      return false;
    }

#elif defined( _WIN32 )

    HANDLE file = CreateFile( filePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr );
    if( file == nullptr ) {
      return false;
    }

    DWORD written;
    BOOL result = WriteFile( file, buffer, DWORD( size ), &written, nullptr );
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
  }

  return true;
}

bool File::write( InputStream* istream ) const
{
  return write( istream->pos(), istream->available() );
}

bool File::write( const Buffer& buffer ) const
{
  return write( buffer.begin(), buffer.length() );
}

bool File::writeString( const String& s ) const
{
  return write( s.cstr(), s.length() );
}

DArray<File> File::ls() const
{
  DArray<File> array;

  if( fileType != DIRECTORY ) {
    return array;
  }

  if( fileFS == VIRTUAL ) {
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
        array[i] = File( fileFS, prefix + *entity );
        ++i;
      }
    }

    PHYSFS_freeList( list );
  }
  else {
#if defined( __native_client__ )

    // TODO: Implement when pp::DirectoryReader gets into stable PPAPI.

#elif defined( _WIN32 )

    WIN32_FIND_DATA entity;

    HANDLE dir = FindFirstFile( filePath + "\\*.*", &entity );
    if( dir == nullptr ) {
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
    if( dir == nullptr ) {
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
        array[i] = File( fileFS, filePath + "/" + entity.cFileName );
        ++i;
      }
    }

    CloseHandle( dir );

#else

    DIR* directory = opendir( filePath );
    if( directory == nullptr ) {
      return array;
    }

    struct dirent* entity = readdir( directory );

    // Count entries first.
    int count = 0;
    while( entity != nullptr ) {
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

      if( entity == nullptr ) {
        closedir( directory );
        array.clear();
        return array;
      }

      if( entity->d_name[0] != '.' ) {
        array[i] = File( fileFS, prefix + entity->d_name );
        ++i;
      }
    }

    closedir( directory );

#endif
  }

  array.sort();
  return array;
}

String File::cwd()
{
#if defined( __native_client__ )

  return "";

#elif defined( _WIN32 )

  char buffer[256];
  bool hasFailed = GetCurrentDirectory( 256, buffer ) == 0;
  return hasFailed ? "" : buffer;

#else

  char buffer[256];
  bool hasFailed = getcwd( buffer, 256 ) == nullptr;
  return hasFailed ? "" : buffer;

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

bool File::mkdir( const char* path, FileSystem fileSystem )
{
  if( fileSystem == VIRTUAL ) {
    return PHYSFS_mkdir( path ) != 0;
  }
  else {
#if defined( __native_client__ )

    Descriptor localDescriptor( nullptr );
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
      _fd->fref = new pp::FileRef( *ppFileSystem, _fd->buffer );

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

    return CreateDirectory( path, nullptr ) != 0;

#else

    return ::mkdir( path, 0755 ) == 0;

#endif
  }
}

bool File::rm( const char* path, FileSystem fileSystem )
{
  if( fileSystem == VIRTUAL ) {
    return PHYSFS_delete( path );
  }
  else {
#if defined( __native_client__ )

    Descriptor localDescriptor( nullptr );
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
      _fd->fref = new pp::FileRef( *ppFileSystem, _fd->buffer );

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

    if( File( File::NATIVE, path ).fileType == DIRECTORY ) {
      return RemoveDirectory( path ) != 0;
    }
    else {
      return DeleteFile( path ) != 0;
    }

#else

    if( File( File::NATIVE, path ).fileType == DIRECTORY ) {
      return rmdir( path ) == 0;
    }
    else {
      return unlink( path ) == 0;
    }

#endif
  }
}

bool File::mount( const char* path, const char* mountPoint, bool append )
{
  return PHYSFS_mount( path, mountPoint, append ) != 0;
}

bool File::mountLocal( const char* path )
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

void File::init( FileSystem fileSystem, NaClFileSystem naclFileSystem, int naclSize )
{
  static_cast<void>( naclFileSystem );
  static_cast<void>( naclSize );

  if( fileSystem == VIRTUAL ) {
#ifdef __native_client__

    pp::Module* module = pp::Module::Get();

    if( System::instance == nullptr ) {
      OZ_ERROR( "System::instance must be set prior to PhysicsFS initialisation" );
    }
    if( module->core()->IsMainThread() ) {
      OZ_ERROR( "PhysicsFS cannot be initialisation in the main thread" );
    }

    PHYSFS_NACL_init( System::instance->pp_instance(), module->get_browser_interface(),
                      naclFileSystem == File::PERSISTENT ? PP_FILESYSTEMTYPE_LOCALPERSISTENT :
                                                           PP_FILESYSTEMTYPE_LOCALTEMPORARY,
                      naclSize );

#endif

    if( PHYSFS_init( nullptr ) == 0 ) {
      OZ_ERROR( "PhysicsFS initialisation failed: %s", PHYSFS_getLastError() );
    }
  }
  else {
#ifdef __native_client__

    ppCore = pp::Module::Get()->core();

    if( System::instance == nullptr ) {
      OZ_ERROR( "NaClModule::instance must be set to NaCl module pointer in order to initialise"
                " NaCl file system" );
    }

    destroy();

    Descriptor* descriptor = &Descriptor::staticDesc;

    // We abuse staticDesc.size and staticDesc.offset variables to pass file system type and size to
    // callback.
    descriptor->size   = naclSize;
    descriptor->offset = naclFileSystem == PERSISTENT ? PP_FILESYSTEMTYPE_LOCALPERSISTENT :
                                                        PP_FILESYSTEMTYPE_LOCALTEMPORARY;

    DEFINE_CALLBACK( initResult, {
      if( _result != PP_OK ) {
        delete ppFileSystem;
        ppFileSystem = nullptr;
      }
      SEMAPHORE_POST();
    } );
    DEFINE_CALLBACK( init, {
      ppFileSystem = new pp::FileSystem( System::instance, PP_FileSystemType( _fd->offset ) );

      int ret = ppFileSystem->Open( _fd->size, CALLBACK_OBJECT( initResult, _fd ) );
      if( ret == PP_OK_COMPLETIONPENDING ) {
        return;
      }

      delete ppFileSystem;
      ppFileSystem = nullptr;
      SEMAPHORE_POST();
    } );

    MAIN_CALL( init );
    SEMAPHORE_WAIT();

    if( ppFileSystem == nullptr ) {
      OZ_ERROR( "Local file system open failed" );
    }

#endif
  }
}

void File::destroy( FileSystem fileSystem )
{
  if( fileSystem == VIRTUAL ) {
    PHYSFS_deinit();
  }
  else {
#ifdef __native_client__

    if( ppFileSystem != nullptr ) {
      Descriptor* descriptor = &Descriptor::staticDesc;

      DEFINE_CALLBACK( free, {
        delete ppFileSystem;
        ppFileSystem = nullptr;

        SEMAPHORE_POST();
      } );

      MAIN_CALL( free );
      SEMAPHORE_WAIT();
    }

#endif
  }
}

}
