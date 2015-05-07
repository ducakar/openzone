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

#include "String.hh"

namespace oz
{

/**
 * Memory buffer.
 *
 * It can be used as a generic memory buffer or to store contents read from a file.
 */
class Buffer : public List<char>
{
public:

  using List<char>::List;

  /**
   * Create an empty buffer of size 0.
   */
  Buffer() = default;

  /**
   * Create a buffer containing a given string (without the terminating null character).
   */
  explicit Buffer(const String& s);

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

};

}
