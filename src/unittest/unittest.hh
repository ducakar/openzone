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

#include <ozCore/ozCore.hh>

#define OZ_CHECK(cond) \
  if (!(cond)) { \
    oz::System::error(__PRETTY_FUNCTION__, __FILE__, __LINE__, 0, "Check '%s' failed", #cond); \
  }

#define OZ_CHECK_CONTENTS(container, ...) \
  { \
    auto i = crange(container); \
    using CRange = decltype(i); \
    CRange::Elem array[] = {__VA_ARGS__}; \
    OZ_CHECK(iEquals(i, crange(array))); \
  }

template <class CRangeA, class CRangeB>
inline bool iEquals(CRangeA rangeA, CRangeB rangeB)
{
  auto& iterA = rangeA.begin();
  auto& endA  = rangeA.end();
  auto& iterB = rangeB.begin();
  auto& endB  = rangeB.end();

  for (; iterA != endA && iterB != endB && *iterA == *iterB; ++iterA, ++iterB);
  return iterA == endA && iterB == endB;
}

extern bool hasPassed;

struct Foo : oz::DChainNode<Foo>
{
  static bool allowCopy;
  static bool allowMove;
  static bool allowEqualsOp;
  static bool allowLessOp;

  int value;

  Foo()
    : value(-1)
  {}

  Foo(const Foo& f)
    : value(f.value)
  {
    OZ_CHECK(allowCopy);
  }

  Foo(Foo&& f)
    : value(f.value)
  {
    OZ_CHECK(allowMove);

    f.value = -1;
  }

  Foo(int i)
    : value(i)
  {}

  Foo& operator=(const Foo& f)
  {
    OZ_CHECK(allowCopy);

    if (this != &f) {
      value = f.value;
    }
    return *this;
  }

  Foo& operator=(Foo&& f)
  {
    OZ_CHECK(allowMove);

    if (this != &f) {
      value = f.value;
      f.value = -1;
    }
    return *this;
  }

  Foo& operator=(int i)
  {
    value = i;
    return *this;
  }

  bool operator==(const Foo& f) const
  {
    OZ_CHECK(allowEqualsOp);

    return value == f.value;
  }

  bool operator==(int i) const
  {
    OZ_CHECK(allowEqualsOp);

    return value == i;
  }

  friend bool operator==(int i, const Foo& f)
  {
    OZ_CHECK(allowEqualsOp);

    return i == f.value;
  }

  bool operator<(const Foo& f) const
  {
    OZ_CHECK(allowLessOp);

    return value < f.value;
  }

  bool operator<(int i) const
  {
    OZ_CHECK(allowLessOp);

    return value < i;
  }

  friend bool operator<(int i, const Foo& f)
  {
    OZ_CHECK(allowLessOp);

    return i < f.value;
  }

  operator int() const
  {
    return value;
  }
};

void test_common();
void test_iterables();
void test_Arrays();

void test_Alloc();

int main();
