/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include "unittest.hh"

using namespace oz;

void test_Alloc()
{
  Log() << "+ Alloc";

  size_t oAmount    = Alloc::amount;
  size_t oSumAmount = Alloc::sumAmount;
  int    oCount     = Alloc::count;
  int    oSumCount  = Alloc::sumCount;

  Foo* array = new Foo[10]();
  OZ_CHECK(Alloc::amount >= oAmount + 10 * sizeof(Foo))
  OZ_CHECK(Alloc::sumAmount >= oSumAmount + 10 * sizeof(Foo))
  OZ_CHECK(Alloc::count == oCount + 1)
  OZ_CHECK(Alloc::sumCount == oSumCount + 1)

  array = Arrays::reallocate(array, 5, 8);
  OZ_CHECK(Alloc::amount >= oAmount + 8 * sizeof(Foo))
  OZ_CHECK(Alloc::sumAmount >= oSumAmount + 18 * sizeof(Foo))
  OZ_CHECK(Alloc::count == oCount + 1)
  OZ_CHECK(Alloc::sumCount == oSumCount + 2)

  delete[] array;
  OZ_CHECK(Alloc::amount >= oAmount)
  OZ_CHECK(Alloc::sumAmount >= 18 * sizeof(Foo))
  OZ_CHECK(Alloc::count == oCount + 0)
  OZ_CHECK(Alloc::sumCount == oSumCount + 2)

  OZ_CHECK(Alloc::alignDown(0) == 0)
  OZ_CHECK(Alloc::alignDown(1) == 0)
  OZ_CHECK(Alloc::alignDown(OZ_ALIGNMENT - 1) == 0)
  OZ_CHECK(Alloc::alignDown(OZ_ALIGNMENT) == OZ_ALIGNMENT)

  OZ_CHECK(Alloc::alignUp(0) == 0)
  OZ_CHECK(Alloc::alignUp(1) == OZ_ALIGNMENT)
  OZ_CHECK(Alloc::alignUp(OZ_ALIGNMENT - 1) == OZ_ALIGNMENT)
  OZ_CHECK(Alloc::alignUp(OZ_ALIGNMENT) == OZ_ALIGNMENT)

  char* zeroptr = nullptr;
  char* oneptr  = zeroptr + OZ_ALIGNMENT;

  OZ_CHECK(Alloc::alignDown(zeroptr) == zeroptr)
  OZ_CHECK(Alloc::alignDown(zeroptr + 1) == zeroptr)
  OZ_CHECK(Alloc::alignDown(oneptr - 1) == zeroptr)
  OZ_CHECK(Alloc::alignDown(oneptr) == oneptr)

  OZ_CHECK(Alloc::alignUp(zeroptr) == zeroptr)
  OZ_CHECK(Alloc::alignUp(zeroptr + 1) == oneptr)
  OZ_CHECK(Alloc::alignUp(oneptr - 1) == oneptr)
  OZ_CHECK(Alloc::alignUp(oneptr) == oneptr)
}
