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
 * @file unittest/iterables.cc
 */

#include "unittest.hh"

using namespace oz;

void test_iterables()
{
  Log() << "+ iterables";

  Chain<Foo>::CIterator        icl;
  Chain<Foo>::Iterator         il;
  DChain<Foo>::CIterator       icdl;
  DChain<Foo>::Iterator        idl;
  List<Foo>::CIterator         icv;
  List<Foo>::Iterator          iv;
  SList<Foo, 1>::CIterator     icsv;
  SList<Foo, 1>::Iterator      isv;
  Set<Foo>::CIterator          ics;
  Set<Foo>::Iterator           is;
  Map<Foo, Foo>::CIterator     icm;
  Map<Foo, Foo>::Iterator      im;
  HashSet<Foo>::CIterator      ichs;
  HashSet<Foo>::Iterator       ihs;
  HashMap<Foo, Foo>::CIterator ichm;
  HashMap<Foo, Foo>::Iterator  ihtm;

  List<Foo*>::Iterator         invalid;

  DChain<Foo> l;
  List<Foo> v;

  l.add(new Foo(2));
  l.add(new Foo(3));
  l.add(new Foo(2));
  l.add(new Foo(1));

  v.add(1);
  v.add(2);
  v.add(3);
  v.add(2);

  OZ_CHECK_CONTENTS(v, 1, 2, 3, 2);
  OZ_CHECK(iEquals(l.citer(), v.citer()));
  OZ_CHECK(iEquals(citer(l), citer(l)));

  v.add({});
  OZ_CHECK(!iEquals(l.citer(), citer(v)));

  v.popLast();
  OZ_CHECK(iEquals(citer(l), citer(v)));

  List<Foo*> pv;
  pv.add(new Foo(1));
  pv.add(new Foo(2));

  l.free();

  OZ_CHECK(!iEquals(l.citer(), v.citer()));
  OZ_CHECK(!iEquals(v.citer(), l.citer()));

  v.clear();
  v.trim();

  OZ_CHECK(iEquals(l.citer(), v.citer()));
}
