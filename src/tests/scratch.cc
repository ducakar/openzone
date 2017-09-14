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

namespace oz
{
namespace detail
{

template <typename Head, typename ...Tail>
union VarImpl
{
  char             head[sizeof(Head)];
  VarImpl<Tail...> tail;
};

template <typename Head>
union VarImpl<Head>
{
  char head[sizeof(Head)];
};

template <typename Elem, typename Head, typename ...Tail>
struct VarAccessor
{
  static constexpr const ubyte TYPE_INDEX = 1 + VarAccessor<Elem, Tail...>::TYPE_INDEX;

  static const Elem& as(const VarImpl<Head, Tail...>& var)
  {
    return VarAccessor<Elem, Tail...>::as(var.tail);
  }

  static Elem& as(VarImpl<Head, Tail...>& var)
  {
    return VarAccessor<Elem, Tail...>::as(var.tail);
  }
};

template <typename Head, typename ...Tail>
struct VarAccessor<Head, Head, Tail...>
{
  static const ubyte TYPE_INDEX = 0;

  static const Head& as(const VarImpl<Head, Tail...>& var)
  {
    return *reinterpret_cast<const Head*>(var.head);
  }

  static Head& as(VarImpl<Head, Tail...>& var)
  {
    return *reinterpret_cast<Head*>(var.head);
  }
};

}

template <typename Head, typename ...Tail>
class Var;

template <typename Head, typename ...Tail>
class Var
{
private:

  detail::VarImpl<Head, Tail...> data_;
  ubyte                          index_;

public:

  Var() = default;

  template <typename Elem>
  Var(Elem&& e)
    : data_{}, index_(0)
  {
    ::new(this) Elem(static_cast<Elem&&>(e));
  }

  template <typename Elem>
  Elem& operator=(Elem&& e)
  {
    as<Elem>() = static_cast<Elem&&>(e);
  }

  template <typename Elem>
  int index() const
  {
    return detail::VarAccessor<Elem, Head, Tail...>::TYPE_INDEX;
  }

  template <typename Elem>
  const Elem& as() const
  {
    return detail::VarAccessor<Elem, Head, Tail...>::as(data_);
  }

  template <typename Elem>
  Elem& as()
  {
    return detail::VarAccessor<Elem, Head, Tail...>::as(data_);
  }
};

}

using namespace oz;

struct Foo
{
  Foo()
  {
    Log() << "Foo()";
  }

  ~Foo()
  {
    Log() << "~Foo()";
  }

  Foo(const Foo&)
  {
    Log() << "Foo(const Foo&)";
  }

  Foo(Foo&&)
  {
    Log() << "Foo(Foo&&)";
  }

  Foo& operator=(const Foo&)
  {
    Log() << "operator=(const Foo&)";
    return *this;
  }

  Foo& operator=(Foo&&)
  {
    Log() << "operator=(Foo&&)";
    return *this;
  }

  char data[50];
};

struct Bar
{
};

static Var<int, char> v(42);
static auto i = v.as<int>();

int main()
{
  System::init();
  Log() << v.index<int>();
  Log() << v.index<char>();
  return 0;
}
