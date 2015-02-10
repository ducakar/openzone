/*
 * liboz - OpenZone Core Library.
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
 * @file unittest/unittest.cc
 */

#include "unittest.hh"

#include <cstdlib>

using namespace oz;

bool hasPassed = true;

bool Foo::allowCopy     = true;
bool Foo::allowMove     = true;
bool Foo::allowEqualsOp = true;
bool Foo::allowLessOp   = true;

#ifdef __native_client__
int naclMain(int, char**)
#else
int main()
#endif
{
  Log() << "Unittest has begun";

  System::init();
  Math::seed(42);

  test_common();
  test_iterables();
  test_arrays();

#ifdef OZ_ALLOCATOR
  test_Alloc();
#endif

  test_String();

  Log() << (hasPassed ? "Unittest PASSED" : "Unittest FAILED");
  return EXIT_SUCCESS;
}

OZ_NACL_ENTRY_POINT()
