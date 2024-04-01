/*
 * ozCore - OpenZone Core Library.
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

/**
 * @file ozCore/Arrays.hh
 *
 * Iterators and utility functions for arrays and array-like containers.
 */

#pragma once

#include "Range.hh"

namespace oz
{

/**
 * Wrapper class for array iterators and utility functions.
 */
class Arrays
{
public:

  /**
   * Array range with constant access to elements.
   */
  template <typename Elem>
  using CRange = oz::Range<const Elem*, const Elem*>;

  /**
   * Array range with non-constant access to elements.
   */
  template <typename Elem>
  using Range = oz::Range<Elem*, Elem*>;

private:

  /**
   * Helper function for `sort()`.
   *
   * @note
   * `Elem` type must have `operator<(const Elem&)` defined.
   *
   * Quicksort algorithm is used which takes last element in a partition as a pivot so sorting a
   * sorted or nearly sorted array will take O(n^2) time instead of O(n log n) as in average case.
   * When a partition has at most 11 elements, selection sort is used.
   *
   * @param first pointer to first element in the array to be sorted.
   * @param last pointer to last element in the array.
   */
  template <typename Elem, class LessFunc = Less<Elem>>
  static void quicksort(Elem* first, Elem* last)
  {
    // 8 seem as the optimal thresholds for switching to selection sort (Core i5 4460).
    if (last - first > 8) {
      // Quicksort (the last element is the pivot).
      Elem* top        = first;
      Elem* bottom     = last - 1;
      Elem* pivot      = first + (last - first) / 2;
      Elem  pivotValue = *pivot;

      swap<Elem>(*pivot, *last);

      while (true) {
        while (!LessFunc()(pivotValue, *top) && top <= bottom) {
          ++top;
        }
        while (LessFunc()(pivotValue, *bottom) && top < bottom) {
          --bottom;
        }

        if (top >= bottom) {
          break;
        }

        swap<Elem>(*top, *bottom);
      }

      if (top != last) {
        swap<Elem>(*top, *last);
        quicksort<Elem, LessFunc>(top + 1, last);
      }
      quicksort<Elem, LessFunc>(first, top - 1);
    }
    else {
      // Selection sort.
      for (Elem* i = first; i < last;) {
        Elem* pivot   = i;
        Elem* minElem = i;
        ++i;

        for (Elem* j = i; j <= last; ++j) {
          if (LessFunc()(*j, *minElem)) {
            minElem = j;
          }
        }
        if (minElem != pivot) {
          swap<Elem>(*minElem, *pivot);
        }
      }
    }
  }

public:

  /**
   * Static class.
   */
  Arrays() = delete;

  /**
   * True iff respective elements are equal.
   */
  template <typename Elem>
  static bool equals(const Elem* arrayA, int size, const Elem* arrayB)
  {
    const Elem* endA = arrayA + size;

    while (arrayA < endA) {
      if (!(*arrayA++ == *arrayB++)) {
        return false;
      }
    }
    return true;
  }

  /**
   * Length of a static array.
   */
  template <typename Elem, int SIZE>
  OZ_ALWAYS_INLINE
  static constexpr int size(const Elem (&)[SIZE]) noexcept
  {
    return SIZE;
  }

  /**
   * Index of the first occurrence of the value or -1 if not found.
   */
  template <typename Elem, typename Value>
  static int index(const Elem* array, int size, const Value& value)
  {
    const Elem* begin = array;
    const Elem* end   = array + size;

    for (; begin < end; ++begin) {
      if (*begin == value) {
        return int(begin - array);
      }
    }
    return -1;
  }

  /**
   * Index of the last occurrence of the value or -1 if not found.
   */
  template <typename Elem, typename Value>
  static int lastIndex(const Elem* array, int size, const Value& value)
  {
    const Elem* end = array + size - 1;

    for (; end >= array; --end) {
      if (*end == value) {
        break;
      }
    }
    return int(end - array);
  }

  /**
   * True iff a given value is found in an array.
   */
  template <typename Elem, typename Value>
  static bool contains(const Elem* array, int size, const Value& value)
  {
    const Elem* end = array + size;

    for (; array < end; ++array) {
      if (*array == value) {
        return true;
      }
    }
    return false;
  }

  /**
   * Copy array elements from the first to the last.
   */
  template <typename Elem>
  static void copy(const Elem* srcArray, int size, Elem* destArray)
  {
    const Elem* srcEnd = srcArray + size;

    while (srcArray < srcEnd) {
      *destArray++ = *srcArray++;
    }
  }

  /**
   * Move array elements from the last to the first.
   */
  template <typename Elem>
  static void copyBackward(const Elem* srcArray, int size, Elem* destArray)
  {
    const Elem* srcEnd  = srcArray + size;
    Elem*       destEnd = destArray + size;

    while (srcEnd > srcArray) {
      *--destEnd = *--srcEnd;
    }
  }

  /**
   * Move array elements from the first to the last.
   */
  template <typename Elem>
  static void move(Elem* srcArray, int size, Elem* destArray)
  {
    const Elem* srcEnd = srcArray + size;

    while (srcArray < srcEnd) {
      *destArray++ = static_cast<Elem&&>(*srcArray++);
    }
  }

  /**
   * Move array elements from the last to the first.
   */
  template <typename Elem>
  static void moveBackward(Elem* srcArray, int size, Elem* destArray)
  {
    Elem* srcEnd  = srcArray + size;
    Elem* destEnd = destArray + size;

    while (srcEnd > srcArray) {
      *--destEnd = static_cast<Elem&&>(*--srcEnd);
    }
  }

  /**
   * Assign array elements to a given value.
   */
  template <typename Elem, typename Value>
  static void fill(Elem* array, int size, const Value& value)
  {
    const Elem* end = array + size;

    while (array < end) {
      *array++ = value;
    }
  }

  /**
   * Assign each array element to the default value.
   */
  template <typename Elem>
  static void clear(Elem* array, int size)
  {
    const Elem* end = array + size;

    while (array < end) {
      *array++ = Elem();
    }
  }

  /**
   * Delete objects referenced by elements and assign all elements to null.
   *
   * Array elements must be pointers.
   */
  template <typename Elem>
  static void free(Elem* array, int size)
  {
    const Elem* end = array + size;

    while (array < end) {
      delete *array;
      *array++ = Elem();
    }
  }

  /**
   * Reverse the order of array elements.
   */
  template <typename Elem>
  static void reverse(Elem* array, int size)
  {
    Elem* bottom = array;
    Elem* top    = array + size - 1;

    while (bottom < top) {
      swap<Elem>(*bottom++, *top--);
    }
  }

  /**
   * Sort array using `detail::quicksort()`.
   */
  template <typename Elem, class LessFunc = Less<Elem>>
  static void sort(Elem* array, int size)
  {
    int last = size - 1;

    if (last > 0) {
      quicksort<Elem, LessFunc>(array, &array[last]);
    }
  }

  /**
   * Find index in a sorted array such that `array[index - 1] < key && key <= array[index]`.
   *
   * @note
   * `Elem` type must have `bool operator<(const Elem&, const Key&) const` defined.
   *
   * If all elements are lesser return `size` and if all elements are greater return 0.
   *
   * @param array array of elements.
   * @param size number of elements.
   * @param key the key we are looking for.
   * @return Index of the first element >= `key`, `size` otherwise.
   */
  template <typename Elem, typename Key, class LessFunc = Less<Elem>>
  static int bisection(const Elem* array, int size, const Key& key)
  {
    int a = -1;
    int b = size;

    // The algorithm ensures that (a == -1 or array[a] < key) and (b == size or key <= array[b]),
    // so the key may only lie on position a or nowhere.
    while (b - a > 1) {
      int c = (a + b) / 2;

      if (LessFunc()(array[c], key)) {
        a = c;
      }
      else {
        b = c;
      }
    }
    return b;
  }

  /**
   * Resize an array to a different size, moving its elements.
   *
   * Newly allocated elements are explicitly initialised. If `newSize` is 0 the given array is
   * deleted and `nullptr` is returned.
   *
   * @return Newly allocated array.
   */
  template <typename Elem>
  static Elem* resize(Elem* array, int size, int newSize)
  {
    Elem* newArray = nullptr;

    if (newSize != 0) {
      newArray = new Elem[newSize]{};
      move<Elem>(array, min<int>(size, newSize), newArray);
    }

    delete[] array;
    return newArray;
  }

};

}
