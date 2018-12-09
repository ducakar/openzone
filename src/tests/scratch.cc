/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ozCore/ozCore.hh>

#include "Foo.hh"
#include <algorithm>
#include <iostream>
#include <tuple>

using namespace oz;

class Dur
{
public:

  static constexpr long NS_PER_SEC = 1000000000;

private:

  int64 s_  = 0;
  long  ns_ = 0;

private:

  OZ_ALWAYS_INLINE
  static constexpr Dur wrapped(int64 s, long ns)
  {
    return Dur(s + int64(ns / NS_PER_SEC), ns % NS_PER_SEC);
  }

public:

  OZ_ALWAYS_INLINE
  constexpr Dur() = default;

  OZ_ALWAYS_INLINE
  explicit constexpr Dur(int64 s, long ns)
    : s_(s), ns_(ns)
  {
    OZ_ASSERT(ns < NS_PER_SEC);
  }

  OZ_ALWAYS_INLINE
  constexpr int64 s() const
  {
    return s_;
  }

  OZ_ALWAYS_INLINE
  constexpr long ns() const
  {
    return ns_;
  }

  OZ_ALWAYS_INLINE
  constexpr double t() const
  {
    OZ_ASSERT(ns_ >= 0);

    return double(s_) + double(ns_) / NS_PER_SEC;
  }

  OZ_ALWAYS_INLINE
  constexpr bool operator==(const Dur& other) const
  {
    return s_ == other.s_ && ns_ == other.ns_;
  }

  OZ_ALWAYS_INLINE
  constexpr bool operator<(const Dur& other) const
  {
    return s_ < other.s_ || (s_ == other.s_ && ns_ < other.ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator+() const
  {
    return *this;
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator-() const
  {
    return Dur(~s_, NS_PER_SEC - ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator+(const Dur& d) const
  {
    return wrapped(s_ + d.s_, ns_ + d.ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator-(const Dur& d) const
  {
    return wrapped(s_ + ~d.s_, ns_ + NS_PER_SEC - d.ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator*(int i) const
  {
    int64 absI = abs<int>(i);
    int64 s    = i * s_ - absI;
    int64 ns   = absI * NS_PER_SEC + int64(i) * ns_;
    return Dur(s + ns / NS_PER_SEC, ns % NS_PER_SEC);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator/(int i) const
  {
#if 0
    int64 sRem   = s_ % i;
    int64 nsQuot = (ns_ + sRem * NS_PER_SEC) / i;
    int64 sQuot  = s_ / i;
    return D(sQuot, nsQuot % NS_PER_SEC);
#else
    int64 sRem = ~s_ % i;
    int64 nsQuot = (NS_PER_SEC - ns_ + sRem * NS_PER_SEC) / i;
    int64 sQuot = ~s_ / i;
    return Dur(~sQuot, (NS_PER_SEC - nsQuot) % NS_PER_SEC);
#endif
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator/(const Dur&) const
  {
    // TODO
    return Dur(0, 0);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator%(const Dur&) const
  {
    // TODO
    return Dur(0, 0);
  }

};

struct Bar
{
  int value = 0;

  Bar() = default;

  Bar(int value_)
    : value(value_)
  {}

  Bar(const Bar& other)
    : value(other.value)
  {}

  Bar(Bar&& other)
    : Bar()
  {
    swap(*this, other);
  }

  Bar& operator=(const Bar& other)
  {
    Bar temp(other);
    swap(*this, temp);
    return *this;
  }

  Bar& operator=(Bar&& other)
  {
    swap(*this, other);
    return *this;
  }

  friend void swap(Bar& a, Bar& b)
  {
    swap(a.value, b.value);
  }
};

int main()
{
  System::init();
  Dur a(-8, 500000000);
  Dur b(-2, 800000000);

  Log() << (-a * 6).t();
  Log() << (-b * 6).t();

  int a1[] = {1, 2, 3, 4};
  int a2[] = {2, 3, 4, 5};

  oz::swap(a1, a2);
  return 0;
}
