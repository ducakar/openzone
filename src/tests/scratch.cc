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

using namespace oz;

struct Foo
{
  Foo() { Log() << "Foo()"; }
  ~Foo() { Log() << "~Foo()"; }

  Foo(const Foo&) { Log() << "Foo(const Foo&)"; }
  Foo(Foo&&) noexcept { Log() << "Foo(Foo&&)"; }

  Foo& operator=(const Foo&) { Log() << "Foo& operator=(const Foo&)"; return *this; }
  Foo& operator=(Foo&&) noexcept { Log() << "Foo& operator=(Foo&&)"; return *this; }
  bool operator==(const Foo&) { return true; }
  bool operator<(const Foo&) { return false; }

  Foo* prev[1];
  Foo* next[1];
};

class Dur
{
private:

  int64 s_  = 0;
  long  ns_ = 0;

private:

  OZ_ALWAYS_INLINE
  static constexpr Dur wrapped(int64 s, long ns)
  {
    return Dur(s + ns / 1000000000, ns % 1000000000);
  }

public:

  OZ_ALWAYS_INLINE
  constexpr Dur() = default;

  OZ_ALWAYS_INLINE
  explicit constexpr Dur(int64 s, long ns)
    : s_(s), ns_(ns)
  {}

  OZ_ALWAYS_INLINE
  constexpr long ns() const
  {
    return ns_;
  }

  OZ_ALWAYS_INLINE
  constexpr int64 s() const
  {
    return s_;
  }

  OZ_ALWAYS_INLINE
  constexpr float t() const
  {
    OZ_ASSERT(ns_ >= 0);
    return float(s_) + float(ns_) * 1e-9f;
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
    return wrapped(~s_, 1000000000 - ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator+(const Dur& d) const
  {
    return wrapped(s_ + d.s_, ns_ + d.ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator-(const Dur& d) const
  {
    return wrapped(s_ + ~d.s_, ns_ + 1000000000 - d.ns_);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator*(int i) const
  {
    int64 nsProd = int64(ns_) * i + oz::abs(i) * 1000000000;
    int64 sProd  = s_ * i - oz::abs(i);
    return Dur(sProd + nsProd / 1000000000, (nsProd + 1000000000) % 1000000000);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator/(int i) const
  {
#if 0
    int64 sRem   = s_ % i;
    int64 nsQuot = (ns_ + sRem * 1000000000) / i;
    int64 sQuot  = s_ / i;
    return D(sQuot, nsQuot % 1000000000);
#else
    int64 sRem = ~s_ % i;
    int64 nsQuot = (1000000000 - ns_ + sRem * 1000000000) / i;
    int64 sQuot = ~s_ / i;
    return Dur(~sQuot, (1000000000 - nsQuot) % 1000000000);
#endif
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator%(int i) const
  {
    return Dur(0, 0);
  }

  OZ_ALWAYS_INLINE
  constexpr Dur operator%(const Dur& d) const
  {
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
  DChain<Foo> c1;
  c1.add(new Foo{});
  for (auto& i : range(c1.begin(), c1.end())) {
    i.next[0] = nullptr;
    Log() << &i;
  }
  c1.free();
  return 0;
}
