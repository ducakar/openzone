/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

struct Foo
{
  Foo() { oz::Log() << "Foo()"; }
  ~Foo() { oz::Log() << "~Foo()"; }

  Foo(const Foo&) { oz::Log() << "Foo(const Foo&)"; }
  Foo(Foo&&) noexcept { oz::Log() << "Foo(Foo&&)"; }

  Foo& operator=(const Foo&) { oz::Log() << "Foo& operator=(const Foo&)"; return *this; }
  Foo& operator=(Foo&&) noexcept { oz::Log() << "Foo& operator=(Foo&&)"; return *this; }
  bool operator==(const Foo&) { return true; }
  bool operator<(const Foo&) { return false; }
};
