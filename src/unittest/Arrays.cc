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

void test_Arrays()
{
  Log() << "+ Arrays";

  Arrays::CRange<Foo> rca;
  Arrays::Range<Foo>  ra;

  static_cast<void>(rca);
  static_cast<void>(ra);

  Foo a[4] = {1, 2, 3, 2};
  Foo b[4];

  Arrays::move(a, 4, b);
  Arrays::move(b, 4, b);
  Arrays::move(a, 0, b);
  OZ_CHECK_CONTENTS(a, -1, -1, -1, -1)
  OZ_CHECK_CONTENTS(b, 1, 2, 3, 2)
  OZ_CHECK(!Arrays::equals(a, 4, b))
  OZ_CHECK(Arrays::equals(a, 0, b))

  Arrays::moveBackward(b, 4, a);
  Arrays::moveBackward(a, 4, a);
  Arrays::moveBackward(b, 0, a);
  Arrays::move(a, 4, b);
  OZ_CHECK_CONTENTS(a, -1, -1, -1, -1)
  OZ_CHECK_CONTENTS(b, 1, 2, 3, 2)
  OZ_CHECK(!Arrays::equals(a, 4, b))
  OZ_CHECK(Arrays::equals(a, 0, b))

  Arrays::fill(a, 4, 0);
  Arrays::fill(a, 0, -1);
  OZ_CHECK_CONTENTS(a, 0, 0, 0, 0)

  Arrays::copy(b, 4, a);
  Arrays::copy(b, 0, a);
  OZ_CHECK(Arrays::equals(a, 4, b))
  OZ_CHECK(iEquals(crange(a), crange(b)))

  Arrays::copyBackward(a, 4, b);
  Arrays::copyBackward(a, 0, b);
  OZ_CHECK(Arrays::equals(a, 4, b))

  Arrays::copy(a, 4, a);
  OZ_CHECK(Arrays::equals(a, 4, b))

  Arrays::copyBackward(a, 4, a);
  OZ_CHECK(Arrays::equals(a, 4, b))

  OZ_CHECK(!Arrays::contains(a, 4, 0))
  OZ_CHECK(!Arrays::contains(b, 4, 0))
  OZ_CHECK(Arrays::contains(a, 4, 1))
  OZ_CHECK(Arrays::contains(b, 4, 1))
  OZ_CHECK(Arrays::contains(a, 4, 2))
  OZ_CHECK(Arrays::contains(b, 4, 2))
  OZ_CHECK(Arrays::contains(a, 4, 3))
  OZ_CHECK(Arrays::contains(b, 4, 3))
  OZ_CHECK(!Arrays::contains(b, 0, 3))

  OZ_CHECK(Arrays::index(a, 4, 0) == -1)
  OZ_CHECK(Arrays::index(a, 4, 1) == 0)
  OZ_CHECK(Arrays::index(a, 4, 2) == 1)
  OZ_CHECK(Arrays::index(a, 4, 3) == 2)
  OZ_CHECK(Arrays::index(a, 0, 3) == -1)

  OZ_CHECK(Arrays::lastIndex(a, 4, 0) == -1)
  OZ_CHECK(Arrays::lastIndex(a, 4, 1) == 0)
  OZ_CHECK(Arrays::lastIndex(a, 4, 2) == 3)
  OZ_CHECK(Arrays::lastIndex(a, 4, 3) == 2)
  OZ_CHECK(Arrays::lastIndex(a, 0, 3) == -1)

  Arrays::reverse(a, 4);
  OZ_CHECK_CONTENTS(a, 2, 3, 2, 1)

  Foo** c = new Foo*[5]();
  for (Foo*& i : range(c, 5)) {
    i = new Foo();
  }
  Arrays::free(c, 5);
  delete[] c;

  OZ_CHECK(Arrays::size(a) == 4)

  Foo* d = new Foo[4]();
  Arrays::copy<Foo>(b, 4, d);
  d = Arrays::resize(d, 4, 10);
  OZ_CHECK(Arrays::equals(b, 4, d))
  delete[] d;

  for (int j = 0; j < 100; ++j) {
    int r[1000];
    for (int& i : r) {
      i = Math::rand(0, 1000);
    }
    Arrays::sort(r, 1000);

    for (int i = -1; i <= 1000; ++i) {
      int index = Arrays::bisection(r, 1000, i);

      if (1 <= i && i < 1000) {
        OZ_CHECK(r[i - 1] <= r[i])
      }

      OZ_CHECK((index == 0 && r[0] >= i) ||
               (index == 1000 && r[999] < i) ||
               (r[index - 1] < i && r[index] >= i))
    }
  }
}
