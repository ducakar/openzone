/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
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
    InputStream inputStream() const;

    /**
     * Create an OutputStream object for writing binary data into the buffer.
     */
    OutputStream outputStream() const;

};

}
