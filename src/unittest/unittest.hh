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
 * @file unittest/unittest.hh
 */

#include <ozCore/ozCore.hh>

#define OZ_CHECK(cond) \
  if (!(cond)) { \
    oz::System::error(__PRETTY_FUNCTION__, __FILE__, __LINE__, 0, "Check '%s' failed", #cond); \
  }

#define OZ_CHECK_CONTENTS(container, ...) \
  { \
    auto i = citerator(container); \
    typedef decltype(i) CIterator; \
    CIterator::ElemType array[] = { __VA_ARGS__ }; \
    OZ_CHECK(iEquals(i, citerator(array))); \
  }

template <class CIteratorA, class CIteratorB>
inline bool iEquals(CIteratorA iterA, CIteratorB iterB)
{
  hard_assert(static_cast<void*>(&iterA) != static_cast<void*>(&iterB));

  for (; iterA.isValid() && iterB.isValid() && *iterA == *iterB; ++iterA, ++iterB);
  return !iterA.isValid() && !iterB.isValid();
}

extern bool hasPassed;

struct Foo
{
  static bool allowCopy;
  static bool allowMove;
  static bool allowEqualsOp;
  static bool allowLessOp;

  int value;

  Foo* prev[1];
  Foo* next[1];

  Foo() :
    value(-1)
  {}

  Foo(const Foo& f) :
    value(f.value)
  {
    OZ_CHECK(allowCopy);
  }

  Foo(Foo&& f) :
    value(f.value)
  {
    OZ_CHECK(allowMove);

    f.value = -1;
  }

  Foo(int i) :
    value(i)
  {}

  Foo& operator = (const Foo& f)
  {
    OZ_CHECK(allowCopy);

    if (this != &f) {
      value = f.value;
    }
    return *this;
  }

  Foo& operator = (Foo&& f)
  {
    OZ_CHECK(allowMove);

    if (this != &f) {
      value = f.value;
      f.value = -1;
    }
    return *this;
  }

  Foo& operator = (int i)
  {
    value = i;
    return *this;
  }

  bool operator == (const Foo& f) const
  {
    OZ_CHECK(allowEqualsOp);

    return value == f.value;
  }

  bool operator == (int i) const
  {
    OZ_CHECK(allowEqualsOp);

    return value == i;
  }

  friend bool operator == (int i, const Foo& f)
  {
    OZ_CHECK(allowEqualsOp);

    return i == f.value;
  }

  bool operator < (const Foo& f) const
  {
    OZ_CHECK(allowLessOp);

    return value < f.value;
  }

  bool operator < (int i) const
  {
    OZ_CHECK(allowLessOp);

    return value < i;
  }

  friend bool operator < (int i, const Foo& f)
  {
    OZ_CHECK(allowLessOp);

    return i < f.value;
  }

  operator int () const
  {
    return value;
  }
};

void test_common();
void test_iterables();
void test_arrays();

void test_Alloc();

int main();
