/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file oz/Buffer.hh
 */

#pragma once

#include "InputStream.hh"
#include "OutputStream.hh"

namespace oz
{

/**
 * Memory buffer.
 *
 * It can be used as a generic memory buffer or to store contents read from a file.
 *
 * @ingroup oz
 */
class Buffer
{
  private:

    char* data; ///< Storage.
    int   size; ///< Data length in bytes.

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
     * Move constructor, moves data storage.
     */
    Buffer( Buffer&& b );

    /**
     * Copy operator, copies data.
     *
     * Reuse existing storage if it suffices.
     */
    Buffer& operator = ( const Buffer& b );

    /**
     * Move operator, moves data storage.
     */
    Buffer& operator = ( Buffer&& b );

    /**
     * Create a buffer of size <tt>size</tt>.
     */
    explicit Buffer( int initSize );

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
      return data + size;
    }

    /**
     * Pointer to the end of the buffer.
     */
    char* end()
    {
      return data + size;
    }

    /**
     * %Buffer size in bytes.
     */
    int length() const
    {
      return size;
    }

    /**
     * True iff buffer size is 0 (and no resources allocated).
     */
    bool isEmpty() const
    {
      return size == 0;
    }

    /**
     * For an empty buffer, allocate new storage of <tt>size</tt> bytes.
     */
    void alloc( int initSize );

    /**
     * Deallocate storage.
     */
    void dealloc();

    /**
     * Create an InputStream object for reading binary data from the buffer.
     */
    InputStream inputStream( Endian::Order order = Endian::NATIVE ) const;

    /**
     * Create an OutputStream object for writing binary data into the buffer.
     */
    OutputStream outputStream( Endian::Order order = Endian::NATIVE ) const;

};

}
