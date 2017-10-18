/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

  Chain<Foo>::CRangeType        icl;
  Chain<Foo>::RangeType         il;
  DChain<Foo>::CRangeType       icdl;
  DChain<Foo>::RangeType        idl;
  List<Foo>::CRangeType         icv;
  List<Foo>::RangeType          iv;
  SList<Foo, 1>::CRangeType     icsv;
  SList<Foo, 1>::RangeType      isv;
  Set<Foo>::CRangeType          ics;
  Set<Foo>::RangeType           is;
  Map<Foo, Foo>::CRangeType     icm;
  Map<Foo, Foo>::RangeType      im;
  HashSet<Foo>::CRangeType      ichs;
  HashSet<Foo>::RangeType       ihs;
  HashMap<Foo, Foo>::CRangeType ichm;
  HashMap<Foo, Foo>::RangeType  ihtm;

  List<Foo*>::RangeType         invalid;

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

  OZ_CHECK(!iEquals(crange(l), crange(v)));
  OZ_CHECK(!iEquals(crange(v), crange(l)));

  l.free();
  v.clear();
  v.trim();

  OZ_CHECK(iEquals(crange(l), crange(v)));
}
