/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/arrays.hh
 *
 * Iterators and utility functions for arrays and array-like containers.
 *
 * @sa ozCore/iterables.hh
 */

#pragma once

#include "iterables.hh"

namespace oz
{

namespace detail
{

/**
 * Array iterator.
 *
 * It should not be used directly but appropriate iterator types should be typedef'd to it.
 */
template <typename Elem>
class ArrayIterator : public IteratorBase<Elem>
{
protected:

  using IteratorBase<Elem>::elem;

  Elem* past = nullptr; ///< Pointer that points just past the last element.

public:

  /**
   * Create an invalid iterator.
   */
  ArrayIterator() = default;

  /**
   * Array iterator.
   *
   * @param first first array element.
   * @param past_ successor of the last element.
   */
  OZ_ALWAYS_INLINE
  explicit ArrayIterator(Elem* first, Elem* past_) :
    IteratorBase<Elem>(first), past(past_)
  {}

  /**
   * True as long as iterator has not passed all array elements.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return elem != past;
  }

  /**
   * Advance to the next element.
   */
  OZ_ALWAYS_INLINE
  ArrayIterator& operator ++ ()
  {
    hard_assert(elem != past);

    ++elem;
    return *this;
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin() const
  {
    return elem;
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end() const
  {
    return past;
  }

};

/**
 * Utility function for aSort.
 *
 * @note
 * `Elem` type must have `operator < (const Elem&)` defined.
 *
 * Quicksort algorithm is used which takes last element in a partition as a pivot so sorting a
 * sorted or nearly sorted array will take O(n^2) time instead of O(n log n) as in average case.
 * When a partition has at most 11 elements, selection sort is used.
 *
 * @param first pointer to first element in the array to be sorted.
 * @param last pointer to last element in the array.
 */
template <typename Elem>
static void quicksort(Elem* first, Elem* last)
{
  // 8-14 seem as optimal thresholds for switching to selection sort.
  if (last - first > 11) {
    // Quicksort (last element is pivot).
    Elem* top    = first;
    Elem* bottom = last - 1;

    do {
      for (; !(*last < *top) && top <= bottom; ++top);
      for (; *last < *bottom && top < bottom; --bottom);

      if (top >= bottom) {
        break;
      }

      swap<Elem>(*top, *bottom);
    }
    while (true);

    if (top != last) {
      swap<Elem>(*top, *last);
      quicksort<Elem>(top + 1, last);
    }
    quicksort<Elem>(first, top - 1);
  }
  else {
    // Selection sort.
    for (Elem* i = first; i < last;) {
      Elem* pivot = i;
      Elem* min   = i;
      ++i;

      for (Elem* j = i; j <= last; ++j) {
        if (*j < *min) {
          min = j;
        }
      }
      if (min != pivot) {
        swap<Elem>(*min, *pivot);
      }
    }
  }
}

}

/**
 * Array iterator with constant access to elements.
 */
template <typename Elem>
using CIterator = detail::ArrayIterator<const Elem>;

/**
 * Array iterator with non-constant access to elements.
 */
template <typename Elem>
using Iterator = detail::ArrayIterator<Elem>;

/**
 * Create array iterator with constant element access.
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer(const Elem* array, int count)
{
  return CIterator<Elem>(array, array + count);
}

/**
 * Create array iterator with non-constant element access.
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter(Elem* array, int count)
{
  return Iterator<Elem>(array, array + count);
}

/**
 * Create static array iterator with element constant access.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer(const Elem(& array)[COUNT])
{
  return CIterator<Elem>(array, array + COUNT);
}

/**
 * Create static array iterator with non-constant element access.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter(Elem(& array)[COUNT])
{
  return Iterator<Elem>(array, array + COUNT);
}

/**
 * Create initialiser list iterator with element constant access.
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer(InitialiserList<Elem> l)
{
  return CIterator<Elem>(l.begin(), l.begin() + l.size());
}

/**
 * Equivalent to `memcpy()`.
 */
OZ_ALWAYS_INLINE
inline void* mCopy(void* dest, const void* src, int size)
{
#if defined(NDEBUG) || !defined(__GLIBC__)
  return __builtin_memcpy(dest, src, size);
#else
  return __builtin___memcpy_chk(dest, src, size, __builtin_object_size(dest, 0));
#endif
}

/**
 * Equivalent to `memmove()`.
 */
OZ_ALWAYS_INLINE
inline void* mMove(void* dest, const void* src, int size)
{
#if defined(NDEBUG) || !defined(__GLIBC__)
  return __builtin_memmove(dest, src, size);
#else
  return __builtin___memmove_chk(dest, src, size, __builtin_object_size(dest, 0));
#endif
}

/**
 * Equivalent to `memset()`.
 */
OZ_ALWAYS_INLINE
inline void* mSet(void* dest, int value, int size)
{
#if defined(NDEBUG) || !defined(__GLIBC__)
  return __builtin_memset(dest, value, size);
#else
  return __builtin___memset_chk(dest, value, size, __builtin_object_size(dest, 0));
#endif
}

/**
 * Equivalent to `memcmp()`.
 */
OZ_ALWAYS_INLINE
inline int mCompare(const void* srcA, const void* srcB, int size)
{
  return __builtin_memcmp(srcA, srcB, size);
}

/**
 * Equivalent to `memchr()`.
 */
OZ_ALWAYS_INLINE
inline void* mChar(const void* src, int ch, int size)
{
  return __builtin_memchr(src, ch, size);
}

/**
 * Reallocate storage.
 *
 * Similar to `realloc()` but uses `new`/`delete` operators. First `min(size, newSize)` bytes from
 * source are copied to the newly allocated memory. Similar to `realloc()`, it acts as `new char[]`
 * and `delete[]` if called with `src == nullptr` and `newSize == 0` respectively.
 *
 * @return newly allocated stroage.
 */
char* mReallocate(const void* src, int size, int newSize);

/**
 * `strlcpy()` implementation.
 *
 * Safer and faster alternative to `strncpy()`. It always adds the terminating null char and doesn't
 * pad destination with zeros if `length` is larger than the size of the source string.
 */
size_t strlcpy(char* dest, const char* src, size_t size);

/**
 * True iff respective elements are equal.
 */
template <typename Elem>
inline bool aEquals(const Elem* arrayA, int count, const Elem* arrayB)
{
  for (int i = 0; i < count; ++i) {
    if (!(arrayA[i] == arrayB[i])) {
      return false;
    }
  }
  return true;
}

/**
 * Length of a static array.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline constexpr int aLength(const Elem(&)[COUNT])
{
  return COUNT;
}

/**
 * Index of the first occurrence of the value or -1 if not found.
 */
template <typename Elem, typename Value = Elem>
inline int aIndex(const Elem* array, int count, const Value& value)
{
  for (int i = 0; i < count; ++i) {
    if (array[i] == value) {
      return i;
    }
  }
  return -1;
}

/**
 * Index of the last occurrence of the value or -1 if not found.
 */
template <typename Elem, typename Value = Elem>
inline int aLastIndex(const Elem* array, int count, const Value& value)
{
  for (int i = count - 1; i >= 0; --i) {
    if (array[i] == value) {
      return i;
    }
  }
  return -1;
}

/**
 * True iff a given value is found in an array.
 */
template <typename Elem, typename Value = Elem>
inline bool aContains(const Elem* array, int count, const Value& value)
{
  return aIndex<Elem, Value>(array, count, value) >= 0;
}

/**
 * Copy array elements from the first to the last.
 */
template <typename Elem>
inline void aCopy(const Elem* srcArray, int count, Elem* destArray)
{
  for (int i = 0; i < count; ++i) {
    destArray[i] = srcArray[i];
  }
}

/**
 * Move array elements from the last to the first.
 */
template <typename Elem>
inline void aCopyBackward(const Elem* srcArray, int count, Elem* destArray)
{
  for (int i = count - 1; i >= 0; --i) {
    destArray[i] = srcArray[i];
  }
}

/**
 * Move array elements from the first to the last.
 */
template <typename Elem>
inline void aMove(Elem* srcArray, int count, Elem* destArray)
{
  for (int i = 0; i < count; ++i) {
    destArray[i] = static_cast<Elem&&>(srcArray[i]);
  }
}

/**
 * Move array elements from the last to the first.
 */
template <typename Elem>
inline void aMoveBackward(Elem* srcArray, int count, Elem* destArray)
{
  for (int i = count - 1; i >= 0; --i) {
    destArray[i] = static_cast<Elem&&>(srcArray[i]);
  }
}

/**
 * %Set array elements to a given value.
 */
template <typename Elem, typename Value = Elem>
inline void aFill(Elem* array, int count, const Value& value)
{
  for (int i = 0; i < count; ++i) {
    array[i] = value;
  }
}

/**
 * Delete objects referenced by elements (elements must be pointers).
 */
template <typename Elem>
inline void aFree(const Elem* array, int count)
{
  for (int i = 0; i < count; ++i) {
    delete array[i];
  }
}

/**
 * Reverse the order of array elements.
 */
template <typename Elem>
inline void aReverse(Elem* array, int count)
{
  for (int bottom = 0, top = count - 1; bottom < top; ++bottom, --top) {
    swap<Elem>(array[bottom], array[top]);
  }
}

/**
 * Sort array using `detail::quicksort()`.
 */
template <typename Elem>
inline void aSort(Elem* array, int count)
{
  int last = count - 1;

  if (last > 0) {
    detail::quicksort<Elem>(array, &array[last]);
  }
}

/**
 * Find index in a sorted array such that `array[index] <= key && key < array[index + 1]`.
 *
 * @note
 * `Elem` type must have `bool operator < (const Key&, const Elem&) const` defined.
 *
 * If all elements are lesser return `count - 1` and if all elements are greater return -1.
 *
 * @param array array of elements.
 * @param count number of elements.
 * @param key the key we are looking for.
 * @return Index of the last element not greater than `key`, -1 otherwise.
 */
template <typename Elem, typename Key = Elem>
inline int aBisection(const Elem* array, int count, const Key& key)
{
  int a = -1;
  int b = count;

  // The algorithm ensures that (a == -1 or array[a] <= key) and (b == count or key < array[b]),
  // so the key may only lie on position a or nowhere.
  while (b - a > 1) {
    int c = (a + b) / 2;

    if (key < array[c]) {
      b = c;
    }
    else {
      a = c;
    }
  }
  return a;
}

/**
 * Reallocate an array moving its elements.
 *
 * Allocate a new array of `newCount` elements, move first `min(count, newCount)` elements of the
 * source array to the newly created one and free the source array. Similar to `realloc()`, the
 * given array is deleted and `nullptr` is returned if `newCount` is 0.
 *
 * @return Newly allocated array.
 */
template <typename Elem>
inline Elem* aReallocate(Elem* array, int count, int newCount)
{
  Elem* newArray = nullptr;

  if (newCount != 0) {
    newArray = new Elem[newCount];
    aMove<Elem>(array, min<int>(count, newCount), newArray);
  }
  delete[] array;

  return newArray;
}

}
