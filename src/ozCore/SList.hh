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
 * @file ozCore/SList.hh
 *
 * `SList` class template.
 */

#pragma once

#include "Arrays.hh"

namespace oz
{

/**
 * Array list with static storage (fixed capacity).
 *
 * Default copy/move constructors and operators are used, so the source list is not emptied after
 * move.
 *
 * In contrast with `std::vector` all allocated elements are constructed all the time. This yields
 * slightly better performance and simplifies implementation. When an element is removed its
 * destruction is still guaranteed.
 *
 * @sa `oz::List`
 */
template <typename Elem, int SIZE>
class SList
{
  static_assert(SIZE > 0, "oz::SList size must be at least 1");

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef Arrays::CIterator<Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef Arrays::Iterator<Elem> Iterator;

private:

  Elem data[SIZE] = {}; ///< Element storage.
  int  count      = 0;  ///< Number of elements.

public:

  /**
   * Create an empty list with capacity SIZE.
   */
  SList() = default;

  /**
   * Create a list with a given initial length.
   *
   * Primitive types are not initialised to zero.
   */
  explicit SList(int count_) :
    count(count_)
  {
    OZ_ASSERT(count <= SIZE);
  }

  /**
   * Initialise from a C++ array.
   */
  explicit SList(const Elem* array, int count_) :
    SList(count_)
  {
    Arrays::copy<Elem>(array, count, data);
  }

  /**
   * Initialise from an initialiser list.
   */
  SList(InitialiserList<Elem> l) :
    SList(l.begin(), int(l.size()))
  {}

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  SList& operator = (InitialiserList<Elem> l)
  {
    clear();
    addAll(l.begin(), int(l.size()));

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == (const SList& l) const
  {
    return count == l.count && Arrays::equals<Elem>(data, count, l.data);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != (const SList& l) const
  {
    return !operator == (l);
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citerator() const
  {
    return CIterator(data, data + count);
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iterator()
  {
    return Iterator(data, data + count);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* begin() const
  {
    return data;
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin()
  {
    return data;
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* end() const
  {
    return data + count;
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end()
  {
    return data + count;
  }

  /**
   * Number of elements.
   */
  OZ_ALWAYS_INLINE
  int length() const
  {
    return count;
  }

  /**
   * True iff empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return count == 0;
  }

  /**
   * Size of storage.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return SIZE;
  }

  /**
   * Constant reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  const Elem& operator [] (int i) const
  {
    OZ_ASSERT(uint(i) < uint(count));

    return data[i];
  }

  /**
   * Reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator [] (int i)
  {
    OZ_ASSERT(uint(i) < uint(count));

    return data[i];
  }

  /**
   * Constant reference to the first element.
   */
  OZ_ALWAYS_INLINE
  const Elem& first() const
  {
    OZ_ASSERT(count != 0);

    return data[0];
  }

  /**
   * Reference to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem& first()
  {
    OZ_ASSERT(count != 0);

    return data[0];
  }

  /**
   * Constant reference to the last element.
   */
  OZ_ALWAYS_INLINE
  const Elem& last() const
  {
    OZ_ASSERT(count != 0);

    return data[count - 1];
  }

  /**
   * Reference to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem& last()
  {
    OZ_ASSERT(count != 0);

    return data[count - 1];
  }

  /**
   * True iff a given value is found in the list.
   */
  template <typename Key>
  bool contains(const Key& key) const
  {
    return Arrays::contains<Elem, Key>(data, count, key);
  }

  /**
   * Index of the first occurrence of the value or -1 if not found.
   */
  template <typename Key>
  int index(const Key& key) const
  {
    return Arrays::index<Elem, Key>(data, count, key);
  }

  /**
   * Index of the last occurrence of the value or -1 if not found.
   */
  template <typename Key>
  int lastIndex(const Key& key) const
  {
    return Arrays::lastIndex<Elem, Key>(data, count, key);
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_>
  Elem& add(Elem_&& elem)
  {
    return insert<Elem_>(count, static_cast<Elem_&&>(elem));
  }

  /**
   * Add (copy) elements from a given array to the end.
   */
  void addAll(const Elem* array, int arrayCount)
  {
    int newCount = count + arrayCount;

    OZ_ASSERT(uint(newCount) <= uint(SIZE));

    Arrays::copy<Elem>(array, arrayCount, data + count);
    count = newCount;
  }

  /**
   * Add (move) elements from a given array to the end.
   */
  void takeAll(Elem* array, int arrayCount)
  {
    int newCount = count + arrayCount;

    OZ_ASSERT(uint(newCount) <= uint(SIZE));

    Arrays::move<Elem>(array, arrayCount, data + count);
    count = newCount;
  }

  /**
   * Add an element to the end if there is no equal element in the list.
   *
   * @return Position of the inserted or the existing equal element.
   */
  template <typename Elem_>
  Elem& include(Elem_&& elem)
  {
    int i = Arrays::index<Elem, Elem>(data, count, elem);

    if (i >= 0) {
      return data[i];
    }
    else {
      return insert<Elem_>(count, static_cast<Elem_&&>(elem));
    }
  }

  /**
   * Insert an element at a given position.
   *
   * All later elements are shifted to make the gap.
   */
  template <typename Elem_>
  Elem& insert(int i, Elem_&& elem)
  {
    OZ_ASSERT(uint(i) <= uint(count));
    OZ_ASSERT(uint(count) < uint(SIZE));

    Arrays::moveBackward<Elem>(data + i, count - i, data + i + 1);
    data[i] = static_cast<Elem_&&>(elem);
    ++count;

    return data[i];
  }

  /**
   * Remove the element at a given position.
   *
   * All later elements are shifted to fill the gap.
   */
  void erase(int i)
  {
    OZ_ASSERT(uint(i) < uint(count));

    --count;

    if (i == count) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Elem();
    }
    else {
      Arrays::move<Elem>(data + i + 1, count - i, data + i);
    }
  }

  /**
   * Remove the element at a given position from an unordered list.
   *
   * The last element is moved to its place.
   */
  void eraseUnordered(int i)
  {
    OZ_ASSERT(uint(i) < uint(count));

    --count;

    if (i == count) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Elem();
    }
    else {
      data[i] = static_cast<Elem&&>(data[count]);
    }
  }

  /**
   * Find and remove the first element with a given value.
   *
   * @return Index of the removed element or -1 if not found.
   */
  template <typename Key>
  int exclude(const Key& key)
  {
    int i = Arrays::index<Elem, Key>(data, count, key);

    if (i >= 0) {
      erase(i);
    }
    return i;
  }

  /**
   * Find and remove the first element with a given value from an unordered list.
   *
   * The last element is moved to its place.
   *
   * @return Index of the removed element or -1 if not found.
   */
  template <typename Key>
  int excludeUnordered(const Key& key)
  {
    int i = Arrays::index<Elem, Key>(data, count, key);

    if (i >= 0) {
      eraseUnordered(i);
    }
    return i;
  }

  /**
   * Add an element to the beginning.
   *
   * All elements are shifted to make a gap.
   */
  template <typename Elem_>
  Elem&  pushFirst(Elem_&& elem)
  {
    return insert<Elem_>(0, static_cast<Elem_&&>(elem));
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_>
  Elem& pushLast(Elem_&& elem)
  {
    return insert<Elem_>(count, static_cast<Elem_&&>(elem));
  }

  /**
   * Remove the first element.
   *
   * All elements are shifted to fill the gap.
   *
   * @return Value of the removed element.
   */
  Elem popFirst()
  {
    if (count == 0) {
      return Elem();
    }
    else {
      Elem elem = static_cast<Elem&&>(data[0]);

      --count;
      Arrays::move<Elem>(data + 1, count, data);
      return elem;
    }
  }

  /**
   * Remove the last element.
   *
   * @return Value of the removed element.
   */
  Elem popLast()
  {
    if (count == 0) {
      return Elem();
    }
    else {
      --count;
      return static_cast<Elem&&>(data[count]);
    }
  }

  /**
   * Reverse elements.
   */
  void reverse()
  {
    Arrays::reverse<Elem>(data, count);
  }

  /**
   * Sort elements with quicksort.
   */
  template <class LessFunc = Less<void>>
  void sort()
  {
    Arrays::sort<Elem, LessFunc>(data, count);
  }

  /**
   * Resize the list to the specified number of elements.
   *
   * Primitive types are not initialised to zero.
   */
  void resize(int newCount)
  {
    OZ_ASSERT(newCount <= SIZE);

    Arrays::clear<Elem>(data + newCount, count - newCount);
    count = newCount;
  }

  /**
   * Clear the list.
   */
  void clear()
  {
    Arrays::clear<Elem>(data, count);
    count = 0;
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    Arrays::free<Elem>(data, count);
    count = 0;
  }

};

}
