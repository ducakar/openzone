/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Buffer.hh
 *
 * `Buffer` class.
 */

#pragma once

#include "OutputStream.hh"

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

  char* data = nullptr; ///< Storage.
  int   size = 0;       ///< Data length in bytes.

public:

  /**
   * Create an empty buffer of size 0.
   */
  Buffer() = default;

  /**
   * Create an uninitialised buffer of given size.
   */
  explicit Buffer(int size);

  /**
   * Create a buffer of size `size` and copy `data` into it.
   */
  explicit Buffer(const char* data, int size);

  /**
   * Create a buffer containing a given string (without the terminating null character).
   */
  explicit Buffer(const String& s);

  /**
   * Destructor.
   */
  ~Buffer();

  /**
   * Copy constructor, copies data.
   */
  Buffer(const Buffer& b);

  /**
   * Move constructor, moves data storage.
   */
  Buffer(Buffer&& b);

  /**
   * Copy operator, copies data.
   *
   * Existing storage is reused if it suffices.
   */
  Buffer& operator = (const Buffer& b);

  /**
   * Move operator, moves data storage.
   */
  Buffer& operator = (Buffer&& b);

  /**
   * True iff buffer sizes and contents are equal.
   */
  bool operator == (const Buffer& b) const;

  /**
   * False iff buffer sizes and contents are equal.
   */
  bool operator != (const Buffer& b) const;

  /**
   * Constant reference to `i`-th byte.
   */
  OZ_ALWAYS_INLINE
  const char& operator [] (int i) const
  {
    hard_assert(uint(i) < uint(size));

    return data[i];
  }

  /**
   * Reference to `i`-th byte.
   */
  OZ_ALWAYS_INLINE
  char& operator [] (int i)
  {
    hard_assert(uint(i) < uint(size));

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
   * Create an `InputStream` object for reading binary data from this buffer.
   */
  InputStream inputStream(Endian::Order order = Endian::NATIVE) const;

  /**
   * Create a fixed-size `OutputStream` object for writing binary data into this buffer.
   */
  OutputStream outputStream(Endian::Order order = Endian::NATIVE);

  /**
   * Create a string from the buffer contents. Terminating null byte is always appended.
   */
  String toString() const;

  /**
   * Compress using deflate (ZIP/GZip) algorithm.
   *
   * An empty buffer is returned on an error.
   *
   * @param level 0 - none, 1 to 9 - fastest to best, -1 - default level.
   */
  Buffer compress(int level = -1) const;

  /**
   * Decompress data compressed with deflate (ZIP/GZip) algorithm.
   *
   * An empty buffer is returned on an error.
   */
  Buffer decompress() const;

  /**
   * Resize the buffer.
   */
  void resize(int newSize);

};

}
