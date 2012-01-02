/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/File.hh
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
      REGULAR,
      DIRECTORY,
      OTHER,
      MISSING
    };

  private:

    String filePath; ///< %File path.
    Type   type;     ///< Cached file type.
    char*  data;     ///< Mapped memory.
    int    size;     ///< Mapped memory size.

    /**
     * Internal function to a write buffer to the file.
     *
     * @return True on success.
     */
    bool write( const char* buffer, int count ) const;

  public:

    /**
     * Create an empty instance.
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
     * Cached file type is cleared to <tt>NONE</tt> and file is unmapped if it is currently mapped.
     */
    void setPath( const char* path );

    /**
     * Stat file to get its type.
     *
     * %File type is cached until one changes the file path.
     */
    Type getType();

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
     * True iff the extension is equal to the given string.
     *
     * @return True iff extension exists.
     */
    bool hasExtension( const char* ext ) const;

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
     * Write buffer contents into a file.
     */
    bool write( const Buffer* buffer ) const;

    /**
     * Write the first <tt>ostream.length()</tt> bytes to a file.
     */
    bool write( const OutputStream* ostream ) const;

    /**
     * Write the first <tt>bstream.length()</tt> bytes of the stream buffer to a file.
     */
    bool write( const BufferStream* bstream ) const;

    /**
     * Return current directory.
     */
    static String cwd();

    /**
     * Change current directory.
     *
     * @return True on success.
     */
    static bool chdir( const char* path );

    /**
     * Make a new directory.
     *
     * @return True on success.
     */
    static bool mkdir( const char* path, uint mode = 0755 );

    /**
     * Generate a list of files in directory.
     *
     * Hidden files (in Unix means, so everything starting with '.') are skipped.
     * On error, empty array is returned.
     */
    DArray<File> ls();

};

}
