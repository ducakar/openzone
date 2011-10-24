/*
 *  Buffer.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Buffer.hpp
 */

#include "stream.hpp"

namespace oz
{

/**
 * Memory buffer.
 *
 * It can be used as a generic memory buffer or to store contents read from a file.
 */
class Buffer
{
  private:

    char* data;  ///< Storage.
    int   count; ///< Data length in bytes.

  public:

    /**
     * Create an empty buffer.
     */
    Buffer();

    /**
     * Destructor.
     */
    ~Buffer();

    /**
     * Copy constructor, copies data.
     */
    Buffer( const Buffer& b );

    /**
     * Copy operator, copies data.
     *
     * Reuse existing storage if it suffices.
     */
    Buffer& operator = ( const Buffer& b );

    /**
     * Create a buffer of size <tt>size</tt>.
     */
    explicit Buffer( int size );

    /**
     * Create a buffer from contents of the given file.
     *
     * Contents are read from the given file and copied into the buffer. Buffer size matches the
     * file length.
     */
    explicit Buffer( const char* file );

    /**
     * Constant pointer to the beginning of the buffer.
     */
    const char* begin() const
    {
      return data;
    }

    /**
     * Pointer to the beginning of the buffer.
     */
    char* begin()
    {
      return data;
    }

    /**
     * Constant pointer to the end of the buffer.
     */
    const char* end() const
    {
      return data + count;
    }

    /**
     * Pointer to the end of the buffer.
     */
    char* end()
    {
      return data + count;
    }

    /**
     * %Buffer size in bytes.
     */
    int length() const
    {
      return count;
    }

    /**
     * True iff buffer size is 0 (and no resources allocated).
     */
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * For an empty buffer, allocate new storage of <tt>size</tt> bytes.
     */
    void alloc( int size );

    /**
     * Deallocate storage.
     */
    void dealloc();

    /**
     * Create an InputStream object for reading binary data from the buffer.
     */
    InputStream inputStream() const;

    /**
     * Create an OutputStream object for writing binary data into the buffer.
     */
    OutputStream outputStream() const;

    /**
     * Discard any existing data and fill the buffer with contents of the given file.
     *
     * Buffer storage is not freed and newly allocated if the existing storage is enough to store
     * the file's contents.
     */
    bool read( const char* path );

    /**
     * Write buffer contents into a file.
     */
    bool write( const char* path, int size );

};

}
