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
 * @file oz/Buffer.hh
 *
 * Buffer class.
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
    explicit Buffer( int size );

    /**
     * Constant reference to i-th byte.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      return data[i];
    }

    /**
     * Reference to i-th byte.
     */
    OZ_ALWAYS_INLINE
    char& operator [] ( int i )
    {
      return data[i];
    }

    /**
     * Constant pointer to the beginning of the buffer.
     */
    OZ_ALWAYS_INLINE
    const char* begin() const
    {
      return data;
    }

    /**
     * Pointer to the beginning of the buffer.
     */
    OZ_ALWAYS_INLINE
    char* begin()
    {
      return data;
    }

    /**
     * Constant pointer to the end of the buffer.
     */
    OZ_ALWAYS_INLINE
    const char* end() const
    {
      return data + size;
    }

    /**
     * Pointer to the end of the buffer.
     */
    OZ_ALWAYS_INLINE
    char* end()
    {
      return data + size;
    }

    /**
     * %Buffer size in bytes.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return size;
    }

    /**
     * True iff buffer size is 0 (and no resources allocated).
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return size == 0;
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
     * Create an <tt>InputStream</tt> object for reading binary data from the buffer.
     */
    InputStream inputStream( Endian::Order order = Endian::NATIVE ) const;

    /**
     * Create an <tt>OutputStream</tt> object for writing binary data into the buffer.
     */
    OutputStream outputStream( Endian::Order order = Endian::NATIVE ) const;

};

}
