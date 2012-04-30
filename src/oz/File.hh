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
 * @file oz/File.hh
 *
 * File class.
 */

#pragma once

#include "BufferStream.hh"
#include "Buffer.hh"

namespace oz
{

/**
 * Class for basic file and directory operations.
 *
 * @ingroup oz
 */
class File
{
  public:

    /**
     * %File type classification.
     */
    enum Type
    {
      NONE,
      DIRECTORY,
      REGULAR,
      OTHER,
      MISSING
    };

  private:

    String filePath; ///< %File path.
    Type   fileType; ///< %File type.
    char*  data;     ///< Mapped memory.
    int    size;     ///< Mapped memory size.

  public:

    /**
     * Create an empty instance.
     *
     * Path is set to "" and file type to <tt>NONE</tt>.
     */
    File();

    /**
     * Destructor.
     */
    ~File();

    /**
     * Copy constructor.
     *
     * For mapped files, original stays mapped, copy is not.
     */
    File( const File& file );

    /**
     * Move constructor, transfers mapped region "ownership".
     */
    File( File&& file );

    /**
     * Copy operator.
     *
     * For mapped files, original stays mapped, copy is not.
     */
    File& operator = ( const File& file );

    /**
     * Move operator, transfers mapped region "ownership".
     */
    File& operator = ( File&& file );

    /**
     * Create an instance for the given path.
     */
    explicit File( const char* path );

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
    Type type();

    /**
     * Stat file to get its size.
     *
     * %File size in bytes or -1 if stat fails.
     */
    int getSize() const;

    /**
     * %File path.
     */
    String path() const;

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
    bool write( const char* buffer, int count ) const;

    /**
     * Write buffer contents into a file.
     */
    bool write( const Buffer* buffer ) const;

    /**
     * Return current directory.
     */
    static String cwd();

    /**
     * Change current directory.
     */
    static bool chdir( const char* path );

    /**
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped.
     * On error, empty array is returned.
     */
    DArray<File> ls();

    /**
     * Make a new directory.
     */
    static bool mkdir( const char* path );

    /**
     * Delete a file or an empty directory.
     */
    static bool rm( const char* path );

};

}
