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

template <typename Elem, int SIZE>
class Sparse
{
private:

  Elem* data[SIZE] = {};
  int   lastSlot   = 0;

public:

  Elem*& operator[](int i) const
  {
    if (i == -1) {
      return nullptr;
    }

    OZ_ASSERT(uint(i) <= uint(SIZE));

    return static_cast<Elem*>(data[i]);
  }

  int allocate()
  {
    int firstSlot = lastSlot + 1;
    int index     = firstSlot;

    do {
      if (data[index] == nullptr) {
        lastSlot = index;
        return index;
      }
      index = index % SIZE;
    }
    while (index != firstSlot);

    return -1;
  }

  void release(int i)
  {
    OZ_ASSERT(uint(i) <= uint(SIZE));

    data[i] = nullptr;
  }

};

int main()
{
  System::init();
  return 0;
}
