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
 * Wrapper for PhysicsFS or a fake VFS, similar to <tt>File</tt> class.
 *
 * Files can only be accessed for reading through this class. Besides from <tt>File</tt> it also
 * provides some PhysicsFS-specific functions.
 *
 * @ingroup oz
 */
class PhysFile
{
  private:

    String     filePath; ///< %File path.
    File::Type type;     ///< Cached file type.
    char*      data;     ///< Mapped memory.
    int        size;     ///< Mapped memory size.

  public:

    /**
     * Create an empty instance.
     */
    PhysFile();

    /**
     * Destructor.
     */
    ~PhysFile();

    /**
     * Copy constructor.
     *
     * For mapped files, original stays mapped, copy is not.
     */
    PhysFile( const PhysFile& );

    /**
     * Move constructor, transfers mapped region "ownership".
     */
    PhysFile( PhysFile&& file );

    /**
     * Copy operator.
     *
     * For mapped files, original stays mapped, copy is not.
     */
    PhysFile& operator = ( const PhysFile& );

    /**
     * Move operator, transfers mapped region "ownership".
     */
    PhysFile& operator = ( PhysFile&& file );

    /**
     * Create an instance for the given path.
     */
    explicit PhysFile( const char* path );

    /**
     * Set a new file path.
     *
     * Cached file type is cleared to <tt>NONE</tt> and file is unmapped if it is currently mapped.
     */
    void setPath( const char* path );

    /**
     * Get file type.
     *
     * %File type is cached until one changes the file path.
     */
    File::Type getType();

    /**
     * Get file size.
     *
     * %File size in bytes or -1 if file doesn't exist.
     */
    int getSize() const;

    /**
     * %File path in virtual file system.
     */
    String path() const;

    /**
     * %File path in real file system.
     *
     * If file is inside an archive, looks like <tt>"/path/to/archive.zip/file"</tt>.
     */
    String realPath() const;

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
     * Release resources and set default values for internal fields.
     */
    void clear();

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
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped.
     * On error, empty array is returned.
     */
    DArray<PhysFile> ls();

    /**
     * Add PhysicsFS search path.
     *
     * For more detailed information see PhysicsFS manual for <tt>PHYSFS_mount()</tt>.
     *
     * @param source archive or directory in real file system to mount.
     * @param mountPoint mount point in virtual file system, "" or <tt>null</tt> equals root ("/").
     * @param append true to add to the end instead to the beginning of the search path.
     */
    static bool mount( const char* source, const char* mountPoint, bool append );

    /**
     * Initialise PhysicsFS.
     */
    static bool init();

    /**
     * Deinitialise PhysicsFS.
     */
    static bool free();

};

}
