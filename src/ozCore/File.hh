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
 * @file ozCore/File.hh
 *
 * `File` class.
 */

#pragma once

#include "Buffer.hh"

namespace oz
{

/**
 * Class for basic file and directory operations.
 *
 * This class provides two back-ends: the native file system back-end and virtual file system (VFS)
 * back-end implemented via PhysicsFS.
 *
 * Paths inside VFS are always absolute and should not begin with '/', they use '/' as a path
 * separator and are prefixed by "@".
 */
class File
{
  public:

    /**
     * %File type classification.
     */
    enum Type
    {
      MISSING,
      DIRECTORY,
      REGULAR
    };

    /**
     * NaCl file system type.
     */
    enum NaClFileSystem
    {
      TEMPORARY,
      PERSISTENT
    };

  private:

    String filePath; ///< %File path.
    Type   fileType; ///< %File type.
    int    fileSize; ///< %File size (>= 0 if `fileType == REGULAR`, -1 otherwise).
    long64 fileTime; ///< Modification or creation time, what is newer.
    char*  data;     ///< Mapped memory.

  private:

    /**
     * Internal constructor.
     */
    File( const String& path, Type type, int size, long64 time );

  public:

    /**
     * Create an instance for a given path.
     *
     * `stat()` is automatically called on construction unless path empty.
     */
    File( const char* path = "" );

    /**
     * Create an instance for a given path.
     *
     * `stat()` is automatically called on construction unless path empty.
     */
    File( const String& path );

    /**
     * Destructor.
     */
    ~File();

    /**
     * Copy constructor.
     *
     * Mapped memory is not copied.
     */
    File( const File& file );

    /**
     * Move constructor, transfers mapped memory.
     */
    File( File&& file );

    /**
     * Copy operator.
     *
     * Mapped memory is not copied.
     */
    File& operator = ( const File& file );

    /**
     * Move operator, transfers mapped memory.
     */
    File& operator = ( File&& file );

    /**
     * Access file to update its type, size and modification time.
     *
     * @return true iff file exists.
     */
    bool stat();

    /**
     * True iff file path is empty (i.e. an empty string or "@").
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return filePath.fileIsEmpty();
    }

    /**
     * %File type.
     */
    OZ_ALWAYS_INLINE
    Type type() const
    {
      return fileType;
    }

    /**
     * True iff VFS file path.
     */
    OZ_ALWAYS_INLINE
    bool isVirtual() const
    {
      return filePath.fileIsVirtual();
    }

    /**
     * %File size in bytes if regular file, -1 otherwise.
     */
    OZ_ALWAYS_INLINE
    int size() const
    {
      return fileSize;
    }

    /**
     * Modification or creation (Unix) time, what is newer.
     */
    OZ_ALWAYS_INLINE
    long64 time() const
    {
      return fileTime;
    }

    /**
     * %File path.
     */
    OZ_ALWAYS_INLINE
    const String& path() const
    {
      return filePath;
    }

    /**
     * Extract directory from the path (substring before the last `/`).
     */
    String directory() const
    {
      return filePath.fileDirectory();
    }

    /**
     * Extract file name from the path (substring after the last `/`).
     */
    String name() const
    {
      return filePath.fileName();
    }

    /**
     * Extract base file name from the path (substring after the last `/` till the last dot
     * following it).
     */
    String baseName() const
    {
      return filePath.fileBaseName();
    }

    /**
     * Extract file extension from the path (substring after the last dot in file name or "" if no
     * extension).
     */
    String extension() const
    {
      return filePath.fileExtension();
    }

    /**
     * True iff file name has a given extension.
     *
     * Empty string matches both no extension and files names ending with dot.
     */
    bool hasExtension( const char* ext ) const
    {
      return filePath.fileHasExtension( ext );
    }

    /**
     * %Path to the archive or mount-point of a VFS, "" for native files.
     */
    String realDirectory() const;

    /**
     * %Path to file in native file system.
     *
     * This function does not work correctly for VFS files from archives and that whose directory
     * was not mounted to the root of VFS. This function is identical to `path()` for files with
     * native file system path.
     */
    String realPath() const;

    /**
     * True iff file is mapped into memory.
     */
    OZ_ALWAYS_INLINE
    bool isMapped() const
    {
      return data != nullptr;
    }

    /**
     * Read at most `*size` bytes from file and update `*size` to the number of bytes read.
     *
     * @return true iff read operation succeeded (it is not necessary the whole file was read).
     */
    bool read( char* buffer, int* size ) const;

    /**
     * Read file and write it to a given stream's current position.
     *
     * If there's not enough space on stream, the rest of the file contents is skipped.
     *
     * @return true iff read operation succeeded (it is not necessary the whole file was read).
     */
    bool read( OutputStream* ostream ) const;

    /**
     * Read file into a buffer.
     *
     * @return true iff read operation succeeded (it is not necessary the whole file was read).
     */
    Buffer read() const;

    /**
     * Read file as a string.
     *
     * Terminating null character is always assured.
     */
    String readString() const;

    /**
     * Write buffer contents to the file.
     *
     * @note
     * @li This function does not update file size and modification time. `stat()` must be invoked
     *     manually for this.
     * @li Write operation is not possible while the file is mapped.
     */
    bool write( const char* data, int size ) const;

    /**
     * Write buffer contents into the file.
     *
     * @note
     * @li This function does not update file size and modification time. `stat()` must be invoked
     *     manually for this.
     * @li Write operation is not possible while the file is mapped.
     */
    bool write( const Buffer& buffer ) const;

    /**
     * Write string into the file (omitting the terminating null character).
     *
     * @note
     * @li This function does not update file size and modification time. `stat()` must be invoked
     *     manually for this.
     * @li Write operation is not possible while the file is mapped.
     */
    bool writeString( const String& s ) const;

    /**
     * %Map file into memory for reading.
     *
     * If the back-end doesn't support mapping files to memory (currently NaCl and VFS), this
     * function merely copies file contents into an internal buffer.
     */
    bool map();

    /**
     * Unmap mapped file.
     */
    void unmap();

    /**
     * Get `InputStream` for currently mapped file.
     */
    InputStream inputStream( Endian::Order order = Endian::NATIVE ) const;

    /**
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped.
     * On error, an empty array is returned.
     *
     * Directory listing is not supported on NaCl, so this function always returns an empty list.
     */
    DArray<File> ls() const;

    /**
     * Return the current directory in native file system.
     *
     * Empty string is returned on failure. This function always fails on NaCl.
     */
    static String cwd();

    /**
     * Change current directory in native file system.
     *
     * Always fails on NaCl.
     */
    static bool chdir( const char* path );

    /**
     * Make a new directory.
     */
    static bool mkdir( const char* path );

    /**
     * Delete a file or an empty directory.
     */
    static bool rm( const char* path );

    /**
     * Mount read-only directory or archive into VFS.
     *
     * @param path archive or directory in native file system.
     * @param mountPoint mount point in VFS, `nullptr` or "" equals root of VFS.
     * @param append true to append to the end instead to the beginning of the search path.
     */
    static bool mount( const char* path, const char* mountPoint, bool append = false );

    /**
     * Mount writeable local resource directory to the root of VFS.
     *
     * Only physical files under this directory can be changed in VFS. Any new files or directories
     * created in VFS will be physically written here.
     *
     * @note
     * If more than one directory is mounted via `mountLocal()` the last one will be used for
     * writing.
     *
     * @param path path to directory in native file system.
     * @param append true to append to the end instead to the beginning of the search path.
     */
    static bool mountLocal( const char* path, bool append = false );

    /**
     * Initialise NaCl native file system and VFS.
     *
     * On platforms other than NaCl, native file system does not need to be (de)initialised.
     *
     * @param naclFileSystem NaCl file system type, either `TEMPORARY` or `PERSISTENT`.
     * @param naclSize NaCl file system size.
     *
     * @note
     * On NaCl, `System::instance` must be set prior to initialising any kind of file system.
     * Persistent NaCl file system must be initialised from JavaScript before NaCl module is loaded.
     * On other platforms `naclFileSystem` and `naclSize` parameters are ignored.
     */
    static void init( NaClFileSystem naclFileSystem = TEMPORARY, int naclSize = 0 );

    /**
     * Deinitialise VFS.
     */
    static void destroy();

};

}
