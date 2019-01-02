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

void test_common()
{
  Log() << "+ common";

  Foo a = 1;
  Foo b = 2;
  Foo c = 2;
  Foo d = 2;
  Foo e = 3;

  Foo::allowCopy = false;

  swap(a, b);
  OZ_CHECK(a == 2 && b == 1);

  swap(a, b);
  OZ_CHECK(a == 1 && b == 2);

  Foo::allowCopy = true;

  OZ_CHECK(&min(a, b) == &a);
  OZ_CHECK(&min(b, a) == &a);
  OZ_CHECK(&min(b, c) == &b);

  OZ_CHECK(&max(a, b) == &b);
  OZ_CHECK(&max(b, a) == &b);
  OZ_CHECK(&max(b, c) == &b);

  OZ_CHECK(&clamp(c, a, e) == &c);
  OZ_CHECK(&clamp(c, b, e) == &c);
  OZ_CHECK(&clamp(c, a, d) == &c);
  OZ_CHECK(&clamp(a, b, e) == &b);
  OZ_CHECK(&clamp(a, b, d) == &b);
  OZ_CHECK(&clamp(e, b, d) == &d);
}
