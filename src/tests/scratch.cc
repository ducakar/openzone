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
#include <ozEngine/ozEngine.hh>

using namespace oz;

struct S
{
  char s[8];
};

int main()
{
  System::init();

  List<S> l(10000000);

  for (S& s : l) {
    for (int i = 0; i < 8; ++i) {
      s.s[i] = char(97 + Math::rand(26));
    }
    s.s[7] = '\0';
  }

  List<int> h(l.length());

  uint t0;

  t0 = Time::clock();
  for (int i = 0; i < h.length(); ++i) {
    h[i] = Hash<const char*>()(l[i].s);
  }
  Log() << (Time::clock() - t0);
  return 0;
}
