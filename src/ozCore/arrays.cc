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
 * @file ozCore/arrays.cc
 */

#include "arrays.hh"

namespace oz
{

OZ_WEAK
size_t strlcpy(char* dest, const char* s, size_t size)
{
  if (size <= 0) {
    return 0;
  }

  const char* end = s + size - 1;
  size_t count = 0;

  while (s != end && *s != '\0') {
    *dest = *s;

    ++s;
    ++dest;
    ++count;
  }
  *dest = '\0';

  while (*s != '\0') {
    ++s;
    ++count;
  }
  return count;
}

}
