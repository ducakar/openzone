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

void test_iterables()
{
  Log() << "+ iterables";

  Chain<Foo>::CRange        rcc;
  Chain<Foo>::Range         rc;
  DChain<Foo>::CRange       rdcc;
  DChain<Foo>::Range        rdc;
  List<Foo>::CRange         rcl;
  List<Foo>::Range          rl;
  SList<Foo, 1>::CRange     rcsl;
  SList<Foo, 1>::Range      rsl;
  Set<Foo>::CRange          rcs;
  Set<Foo>::Range           rs;
  Map<Foo, Foo>::CRange     rcm;
  Map<Foo, Foo>::Range      rm;
  HashSet<Foo>::CRange      rchs;
  HashSet<Foo>::Range       rhs;
  HashMap<Foo, Foo>::CRange rchm;
  HashMap<Foo, Foo>::Range  rhm;

  static_cast<void>(rcc);
  static_cast<void>(rc);
  static_cast<void>(rdcc);
  static_cast<void>(rdc);
  static_cast<void>(rcl);
  static_cast<void>(rl);
  static_cast<void>(rcsl);
  static_cast<void>(rsl);
  static_cast<void>(rcs);
  static_cast<void>(rs);
  static_cast<void>(rcm);
  static_cast<void>(rm);
  static_cast<void>(rchs);
  static_cast<void>(rhs);
  static_cast<void>(rchm);
  static_cast<void>(rhm);

  DChain<Foo> l;
  List<Foo> v;

  l.add(new Foo(2));
  l.add(new Foo(3));
  l.add(new Foo(2));
  l.add(new Foo(1));

  v.add(0);
  v.add(0);
  v.add(0);
  v.add(0);

  OZ_CHECK(!iEquals(crange(l), crange(v)))
  OZ_CHECK(!iEquals(crange(v), crange(l)))

  l.free();
  v.clear();
  v.trim();

  OZ_CHECK(iEquals(crange(l), crange(v)))
}
