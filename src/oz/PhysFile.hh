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
 * @file oz/PhysFile.hh
 *
 * PhysFile class.
 */

#pragma once

#include "File.hh"

namespace oz
{

/**
 * Class for file represenatation and basic operation in Virtual Filesystem (VFS).
 *
 * It uses PhysicsFS as an underlaying library, on NaCl it uses a custom implementation.
 *
 * @ingroup oz
 */
class PhysFile
{
  private:

    String     filePath; ///< %File path.
    File::Type fileType; ///< %File type.
    char*      data;     ///< Mapped memory.
    int        size;     ///< Mapped memory size.

  public:

    /**
     * Default constructor initialises file to VFS root.
     *
     * Path is set to "" and file type to <tt>DIRECTORY</tt>.
     */
    PhysFile();

    /**
     * Destructor.
     */
    ~PhysFile();

    /**
     * Copy constructor.
     *
     * Mapped memory is not copied.
     */
    PhysFile( const PhysFile& );

    /**
     * Move constructor, transfers mapped mempoy.
     */
    PhysFile( PhysFile&& file );

    /**
     * Copy operator.
     *
     * Mapped memory is not copied.
     */
    PhysFile& operator = ( const PhysFile& );

    /**
     * Move operator, transfers mapped memory.
     */
    PhysFile& operator = ( PhysFile&& file );

    /**
     * Create an instance for the given path and detect file type.
     */
    explicit PhysFile( const char* path );

    /**
     * Set a new file path.
     *
     * Besides changing path, <tt>unmap()</tt> is called and file type is detected for the new path.
     */
    void setPath( const char* path );

    /**
     * Get (cached) file type.
     *
     * %File type is detection is performed on construction or <tt>setPath()</tt>.
     */
    File::Type type() const;

    /**
     * Get file size.
     *
     * %File size in bytes or -1 if file doesn't exist or is a directory.
     */
    int getSize() const;

    /**
     * %File path in virtual file system.
     */
    String path() const;

    /**
     * %Path in real filesystem to file's archive or top folder that is mouted to VFS.
     */
    String realDir() const;

    /**
     * Mount point under which file's archive or top directory is mounted.
     */
    String mountPoint() const;

    /**
     * %File name.
     */
    String name() const;

    /**
     * Extension (part of base name after the last dot) or "" if no dot in base name.
     */
    String extension() const;

    /**
     * Name without the extension (and the dot).
     */
    String baseName() const;

    /**
     * True iff the extension (without dot) is equal to the given string.
     *
     * Empty string matches both no extension and files ending with dot.
     */
    bool hasExtension( const char* ext ) const;

    /**
     * True iff file is mapped to memory.
     */
    bool isMapped() const;

    /**
     * %Map file into memory.
     *
     * One can use <tt>inputStream()</tt> afterwards to read the contents.
     */
    bool map();

    /**
     * Unmap mapped file.
     */
    void unmap();

    /**
     * Get <tt>InputStream</tt> for currently mapped file.
     */
    InputStream inputStream( Endian::Order order = Endian::NATIVE ) const;

    /**
     * Read file into a buffer.
     */
    Buffer read() const;

    /**
     * Write a buffer to the file.
     */
    bool write( const char* buffer, int size ) const;

    /**
     * Write buffer contents into a file.
     */
    bool write( const Buffer* buffer ) const;

    /**
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped.
     * On error, and empty array is returned.
     */
    DArray<PhysFile> ls() const;

    /**
     * Make a new directory.
     */
    static bool mkdir( const char* path );

    /**
     * Delete a file or an empty directory.
     */
    static bool rm( const char* path );

    /**
     * Mount read-only directoy or archive to VFS.
     *
     * @param path archive or directory in real file system directory to mount.
     * @param mountPoint mount point in VFS, "" or <tt>null</tt> equals root of VFS.
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
     * Initialise VFS.
     *
     * On NaCl <tt>System::instance()</tt> must be set prior to initialisation of VFS.
     */
    static void init();

    /**
     * Deinitialise VFS.
     */
    static void free();

};

}
