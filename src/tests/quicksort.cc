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

#include <cstdlib>
#include <cstdio>

using namespace oz;

template <typename Type, int STACK_SIZE>
inline void aSort(Type* array, int count)
{
  Type*  stack[STACK_SIZE];
  Type** sp = stack;
  Type*  first = array;
  Type*  last = array + count - 1;

  *(++sp) = first;
  *(++sp) = last;

  do {
    last = *(--sp);
    first = *(--sp);

    if (first < last) {
      if (last - first > 10) {
        int pivotValue = *last;
        Type* top = first;
        Type* bottom = last - 1;

        do {
          while (top <= bottom && *top <= pivotValue) {
            ++top;
          }
          while (top < bottom && *bottom > pivotValue) {
            --bottom;
          }
          if (top >= bottom) {
            break;
          }
          swap(*top, *bottom);
        }
        while (true);

        swap(*top, *last);

        *(++sp) = first;
        *(++sp) = top - 1;
        *(++sp) = top + 1;
        *(++sp) = last;
      }
      else {
        // selection sort
        for (Type* i = first; i < last;) {
          Type* pivot = i;
          Type* min = i;
          ++i;

          for (Type* j = i; j <= last; ++j) {
            if (*j < *min) {
              min = j;
            }
          }
          swap(*pivot, *min);
        }
      }
    }
  }
  while (sp != stack);
}

template <typename Type>
inline void arSort(Type* first, Type* last)
{
  if (first < last) {
    if (last - first > 1) {
      int pivotValue = *last;
      Type* top = first;
      Type* bottom = last - 1;

      do {
        while (top <= bottom && *top <= pivotValue) {
          ++top;
        }
        while (top < bottom && *bottom > pivotValue) {
          --bottom;
        }
        if (top >= bottom) {
          break;
        }
        swap(*top, *bottom);
      }
      while (true);

      swap(*top, *last);

      arSort(first, top - 1);
      arSort(top + 1, last);
    }
    else if (*first > *last) {
      swap(*first, *last);
    }
  }
}

template <typename Type>
inline void oaSort(Type* array, int begin, int end)
{
  int first = begin;
  int last = end - 1;

  if (first < last) {
    if (first + 1 == last) {
      if (array[first] > array[last]) {
        swap(array[first], array[last]);
      }
    }
    else {
      int pivotValue = array[last];
      int top = first;
      int bottom = last - 1;

      do {
        while (top <= bottom && array[top] <= pivotValue) {
          ++top;
        }
        while (top < bottom && array[bottom] > pivotValue) {
          --bottom;
        }
        if (top < bottom) {
          swap(array[top], array[bottom]);
        }
        else {
          break;
        }
      }
      while (true);

      swap(array[top], array[last]);
      oaSort(array, begin, top);
      oaSort(array, top + 1, end);
    }
  }
}

#define MAX 10000
#define TESTS 2000

int main()
{
  System::init();
  Math::seed(42);

  SList<int, MAX> list(MAX);

  Instant t0 = Instant<STEADY>::now();

  for (int i = 0; i < TESTS; ++i) {
    for (int& j : list) {
      j = Math::rand(MAX);
    }

    Arrays::sort(list.begin(), MAX);
    //aSort<int, 100>(list.begin(), MAX);
    //arSort(list.begin(), list.begin() + MAX - 1);
    //oaSort<int>(list.begin(), 0, MAX);
  }

  Log() << (Instant<STEADY>::now() - t0).ms() << " ms";
  return 0;
}
