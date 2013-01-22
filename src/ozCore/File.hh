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
 * File class.
 */

#pragma once

#include "Buffer.hh"

namespace oz
{

/**
 * Class for basic file and directory operations.
 *
 * This class provides two back-ends: the native filesystem back-end and virtual filesystem (VFS)
 * back-end implemented via PhysicsFS.
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
     * Filesystem type.
     */
    enum Filesystem
    {
      NATIVE,
      VIRTUAL
    };

    /**
     * NaCl filesystem type.
     */
    enum NaClFilesystem
    {
      TEMPORARY,
      PERSISTENT
    };

  private:

    struct Descriptor;

    String      filePath;   ///< %File path.
    Type        fileType;   ///< %File type.
    Filesystem  fileFS;     ///< %Filesystem type.
    int         fileSize;   ///< %File size (>= 0 if `fileType == REGULAR`, -1 otherwise).
    long64      fileTime;   ///< Modification or creation time, what is newer.
    char*       data;       ///< Mapped memory.
#ifdef __native_client__
    Descriptor* descriptor; ///< Structure for control and data exchange with NaCl callbacks.
#endif

  public:

    /**
     * Create an instance for the given filesystem and path.
     *
     * `stat()` is automatically called on construction unless path is an empty string.
     */
    explicit File( Filesystem filesystem = NATIVE, const char* path = "" );

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
     * @return true iff call succeeds, i.e. file exists.
     */
    bool stat();

    /**
     * %File type.
     */
    OZ_ALWAYS_INLINE
    Type type() const
    {
      return fileType;
    }

    /**
     * %Filesystem type.
     */
    OZ_ALWAYS_INLINE
    Filesystem filesystem() const
    {
      return fileFS;
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
     * %File name.
     */
    String name() const
    {
      return filePath.fileName();
    }

    /**
     * Name without the extension (and the dot).
     */
    String baseName() const
    {
      return filePath.fileBaseName();
    }

    /**
     * Extension (part of base name after the last dot) or "" if no dot in base name.
     */
    String extension() const
    {
      return filePath.fileExtension();
    }

    /**
     * True iff the extension (without dot) is equal to the given string.
     *
     * Empty string matches both no extension and files ending with dot.
     */
    bool hasExtension( const char* ext ) const
    {
      return filePath.fileHasExtension( ext );
    }

    /**
     * %Path to the archive or mountpoint of a VFS, "" for native files.
     */
    String realDir() const;

    /**
     * True iff file is mapped into memory.
     */
    OZ_ALWAYS_INLINE
    bool isMapped() const
    {
      return data != nullptr;
    }

    /**
     * %Map native file into memory for reading, always fails for VFS files.
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
     * Read file into a buffer.
     */
    Buffer read() const;

    /**
     * Write buffer contents to the file.
     *
     * It also sets file type on `REGULAR` and updates file size if it succeeds.
     * Write operation is not possible while file is mapped.
     */
    bool write( const char* data, int size ) const;

    /**
     * Write buffer contents into a file.
     *
     * It also sets file type on `REGULAR` and updates file size if it succeeds.
     * Write operation is not possible while file is mapped.
     */
    bool write( const Buffer& buffer ) const;

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
     * Return the current directory in native filesystem.
     *
     * Empty string is returned on failure. This function always fails on NaCl.
     */
    static String cwd();

    /**
     * Change current directory in native filesystem.
     *
     * Always fails on NaCl.
     */
    static bool chdir( const char* path );

    /**
     * Make a new directory.
     */
    static bool mkdir( const char* path, Filesystem filesystem = NATIVE );

    /**
     * Delete a file or an empty directory.
     */
    static bool rm( const char* path, Filesystem filesystem = NATIVE );

    /**
     * Mount read-only directory or archive into VFS.
     *
     * @param path archive or directory in real file system directory to mount.
     * @param mountPoint mount point in VFS, `nullptr` or "" equals root of VFS.
     * @param append true to add to the end instead to the beginning of the search path.
     */
    static bool mount( const char* path, const char* mountPoint, bool append );

    /**
     * Mount read/write local resource directory to root of VFS.
     *
     * This function does not fork for NaCl.
     *
     * @param path path to directory in real file system.
     */
    static bool mountLocal( const char* path );

    /**
     * Initialise filesystem.
     *
     * @note
     * On NaCl, `System::instance` must be set prior to initialising a filesystem (either `NATIVE`
     * or `VFS`). Persistent NaCl filesystem must be initialised from JavaScript before NaCl module
     * is loaded.
     *
     * @param filesystem filesystem type, either `NATIVE` or `VFS`.
     * @param naclFilesystem NaCl filesystem type, either `TEMPORARY` or `PERSISTENT`.
     * @param naclSize NaCl filesystem size.
     */
    static void init( Filesystem filesystem = NATIVE, NaClFilesystem naclFilesystem = TEMPORARY,
                      int naclSize = 0 );

    /**
     * Deinitialise filesystem.
     */
    static void destroy( Filesystem filesystem = NATIVE );

};

}
