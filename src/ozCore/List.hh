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
 * @file ozCore/List.hh
 *
 * `List` class template.
 */

#pragma once

#include "arrays.hh"
#include "System.hh"

namespace oz
{

/**
 * Array list.
 *
 * In contrast with `std::vector` all allocated elements are constructed all the time. This yields
 * slightly better performance and simplifies implementation. However, on element removal its
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::SList`
 */
template <typename Elem>
class List
{
protected:

  /// Granularity for automatic storage allocations.
  static const int GRANULARITY = 8;

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef detail::ArrayIterator<const Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef detail::ArrayIterator<Elem> Iterator;

protected:

  Elem* data  = nullptr; ///< Array of elements.
  int   count = 0;       ///< Number of elements.
  int   size  = 0;       ///< Capacity, number of elements in storage.

protected:

  /**
   * Ensure a given capacity.
   *
   * Capacity is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` able to hold the requested number of elements.
   */
  void ensureCapacity(int capacity)
  {
    if (capacity < 0) {
      OZ_ERROR("oz::List: Capacity overflow");
    }
    else if (size < capacity) {
      size *= 2;
      size  = size < capacity ? (capacity + GRANULARITY - 1) & ~(GRANULARITY - 1) : size;

      if (size <= 0) {
        OZ_ERROR("oz::List: Capacity overflow");
      }

      data = aReallocate<Elem>(data, count, size);
    }
  }

public:

  /**
   * Create an empty list.
   */
  List() = default;

  /**
   * Create a list with a given initial length and capacity.
   *
   * Primitive types are not initialised to zero.
   */
  explicit List(int count_) :
    data(aReallocate<Elem>(nullptr, 0, count_)), count(count_), size(count_)
  {}

  /**
   * Initialise from a C++ array.
   */
  explicit List(const Elem* array, int count_) :
    data(aReallocate<Elem>(nullptr, 0, count_)), count(count_), size(count_)
  {
    aCopy<Elem>(array, count, data);
  }

  /**
   * Initialise from an initialiser list.
   */
  List(InitialiserList<Elem> l) :
    data(new Elem[l.size()]), count(int(l.size())), size(int(l.size()))
  {
    aCopy<Elem>(l.begin(), int(l.size()), data);
  }

  /**
   * Destructor.
   */
  ~List()
  {
    delete[] data;
  }

  /**
   * Copy constructor, copies elements.
   */
  List(const List& l) :
    data(aReallocate<Elem>(nullptr, 0, l.count)), count(l.count), size(l.size)
  {
    aCopy<Elem>(l.data, l.count, data);
  }

  /**
   * Move constructor, moves element storage.
   */
  List(List&& l) :
    data(l.data), count(l.count), size(l.size)
  {
    l.data  = nullptr;
    l.count = 0;
    l.size  = 0;
  }

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  List& operator = (const List& l)
  {
    if (&l != this) {
      if (size < l.count) {
        delete[] data;

        data = new Elem[l.size];
        size = l.size;
      }

      aCopy<Elem>(l.data, l.count, data);
      count = l.count;
    }
    return *this;
  }

  /**
   * Move operator, moves element storage.
   */
  List& operator = (List&& l)
  {
    if (&l != this) {
      delete[] data;

      data  = l.data;
      count = l.count;
      size  = l.size;

      l.data  = nullptr;
      l.count = 0;
      l.size  = 0;
    }
    return *this;
  }

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  List& operator = (InitialiserList<Elem> l)
  {
    if (size < int(l.size())) {
      delete[] data;

      data = new Elem[l.size()];
      size = int(l.size());
    }

    aCopy<Elem>(l.begin(), int(l.size()), data);
    count = int(l.size());

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == (const List& l) const
  {
    return count == l.count && aEquals<Elem>(data, count, l.data);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != (const List& l) const
  {
    return !operator == (l);
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citer() const
  {
    return CIterator(data, data + count);
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iter()
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
   * True iff empty (no storage is allocated).
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return count == 0;
  }

  /**
   * Number of allocated elements.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return size;
  }

  /**
   * Constant reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  const Elem& operator [] (int i) const
  {
    hard_assert(uint(i) < uint(count));

    return data[i];
  }

  /**
   * Reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator [] (int i)
  {
    hard_assert(uint(i) < uint(count));

    return data[i];
  }

  /**
   * Constant reference to the first element.
   */
  OZ_ALWAYS_INLINE
  const Elem& first() const
  {
    hard_assert(count != 0);

    return data[0];
  }

  /**
   * Reference to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem& first()
  {
    hard_assert(count != 0);

    return data[0];
  }

  /**
   * Constant reference to the last element.
   */
  OZ_ALWAYS_INLINE
  const Elem& last() const
  {
    hard_assert(count != 0);

    return data[count - 1];
  }

  /**
   * Reference to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem& last()
  {
    hard_assert(count != 0);

    return data[count - 1];
  }

  /**
   * True iff a given value is found in the array.
   */
  template <typename Elem_ = Elem>
  bool contains(const Elem_& elem) const
  {
    return aContains<Elem, Elem_>(data, count, elem);
  }

  /**
   * Index of the first occurrence of the value or -1 if not found.
   */
  template <typename Elem_ = Elem>
  int index(const Elem_& elem) const
  {
    return aIndex<Elem, Elem_>(data, count, elem);
  }

  /**
   * Index of the last occurrence of the value or -1 if not found.
   */
  template <typename Elem_ = Elem>
  int lastIndex(const Elem_& elem) const
  {
    return aLastIndex<Elem, Elem_>(data, count, elem);
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_ = Elem>
  void add(Elem_&& elem)
  {
    insert(count, static_cast<Elem_&&>(elem));
  }

  /**
   * Add (copy) elements from a given array to the end.
   */
  void addAll(const Elem* array, int arrayCount)
  {
    int newCount = count + arrayCount;

    ensureCapacity(newCount);

    aCopy<Elem>(array, arrayCount, data + count);
    count = newCount;
  }

  /**
   * Add (move) elements from a given array to the end.
   */
  void takeAll(Elem* array, int arrayCount)
  {
    int newCount = count + arrayCount;

    ensureCapacity(newCount);

    aMove<Elem>(array, arrayCount, data + count);
    count = newCount;
  }

  /**
   * Add an element to the end if there is no equal element in the list.
   *
   * @return Position of the inserted or the existing equal element.
   */
  template <typename Elem_ = Elem>
  int include(Elem_&& elem)
  {
    int i = aIndex<Elem, Elem>(data, count, elem);

    if (i >= 0) {
      return i;
    }
    else {
      insert(count, static_cast<Elem_&&>(elem));
      return count - 1;
    }
  }

  /**
   * Insert an element at a given position.
   *
   * All later elements are shifted to make the gap.
   */
  template <typename Elem_ = Elem>
  void insert(int i, Elem_&& elem)
  {
    hard_assert(uint(i) <= uint(count));

    ensureCapacity(count + 1);

    aMoveBackward<Elem>(data + i, count - i, data + i + 1);
    data[i] = static_cast<Elem_&&>(elem);
    ++count;
  }

  /**
   * Remove the element at a given position.
   *
   * All later elements are shifted to fill the gap.
   */
  void erase(int i)
  {
    hard_assert(uint(i) < uint(count));

    --count;

    if (i == count) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Elem();
    }
    else {
      aMove<Elem>(data + i + 1, count - i, data + i);
    }
  }

  /**
   * Remove the element at a given position from an unordered list.
   *
   * The last element is moved to its place.
   */
  void eraseUnordered(int i)
  {
    hard_assert(uint(i) < uint(count));

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
  template <typename Elem_ = Elem>
  int exclude(const Elem_& elem)
  {
    int i = aIndex<Elem, Elem_>(data, count, elem);

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
  template <typename Elem_ = Elem>
  int excludeUnordered(const Elem_& elem)
  {
    int i = aIndex<Elem, Elem_>(data, count, elem);

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
  template <typename Elem_ = Elem>
  void pushFirst(Elem_&& elem)
  {
    insert(0, static_cast<Elem_&&>(elem));
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_ = Elem>
  void pushLast(Elem_&& elem)
  {
    insert(count, static_cast<Elem_&&>(elem));
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
    hard_assert(count != 0);

    Elem elem = static_cast<Elem&&>(data[0]);

    --count;
    aMove<Elem>(data + 1, count, data);
    return elem;
  }

  /**
   * Remove the last element.
   *
   * @return Value of the removed element.
   */
  Elem popLast()
  {
    hard_assert(count != 0);

    --count;
    return static_cast<Elem&&>(data[count]);
  }

  /**
   * Reverse elements.
   */
  void reverse()
  {
    aReverse<Elem>(data, count);
  }

  /**
   * Sort elements with quicksort.
   */
  void sort()
  {
    aSort<Elem>(data, count);
  }

  /**
   * Resize the list and its capacity to the specified number of elements.
   *
   * Primitive types are not initialised to zero.
   */
  void resize(int newCount)
  {
    if (newCount != size) {
      data = aReallocate<Elem>(data, count, newCount);
      size = newCount;
    }
    count = newCount;
  }

  /**
   * Increase capacity (exactly) to the given value if smaller.
   */
  void reserve(int capacity)
  {
    if (size < capacity) {
      data = aReallocate<Elem>(data, count, capacity);
      size = capacity;
    }
  }

  /**
   * Trim capacity to the current number of elements.
   */
  void trim()
  {
    if (count < size) {
      data = aReallocate<Elem>(data, count, count);
      size = count;
    }
  }

  /**
   * Clear the list.
   */
  void clear()
  {
    // Ensure destruction of all elements.
    for (int i = 0; i < count; ++i) {
      data[i] = Elem();
    }
    count = 0;
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    aFree<Elem>(data, count);
    count = 0;
  }

};

}
