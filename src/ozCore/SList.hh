/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
template <typename Elem, int CAPACITY>
class SList
{
  static_assert(CAPACITY > 0, "oz::SList capacity must be at least 1");

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

  Elem data_[CAPACITY] = {}; ///< Element storage.
  int  size_           = 0;  ///< Number of elements.

public:

  /**
   * Create an empty list with capacity CAPACITY.
   */
  SList() = default;

  /**
   * Create a list with a given initial length.
   */
  explicit SList(int size)
    : size_(size)
  {
    OZ_ASSERT(size_ <= CAPACITY);
  }

  /**
   * Initialise from a C++ array.
   */
  explicit SList(const Elem* array, int size)
    : SList(size)
  {
    Arrays::copy<Elem>(array, size, data_);
  }

  /**
   * Initialise from an initialiser list.
   */
  SList(InitialiserList<Elem> il)
    : SList(il.begin(), int(il.size()))
  {}

  /**
   * Assign from an initialiser list.
   */
  SList& operator=(InitialiserList<Elem> il)
  {
    clear();
    addAll(il.begin(), int(il.size()));

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator==(const SList& other) const
  {
    return size_ == other.size_ && Arrays::equals<Elem>(data_, size_, other.data_);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator!=(const SList& other) const
  {
    return !operator==(other);
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citerator() const
  {
    return CIterator(data_, data_ + size_);
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iterator()
  {
    return Iterator(data_, data_ + size_);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* begin() const
  {
    return data_;
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin()
  {
    return data_;
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* end() const
  {
    return data_ + size_;
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end()
  {
    return data_ + size_;
  }

  /**
   * Number of elements.
   */
  OZ_ALWAYS_INLINE
  int size() const
  {
    return size_;
  }

  /**
   * True iff empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return size_ == 0;
  }

  /**
   * Size of storage.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return CAPACITY;
  }

  /**
   * Constant reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  const Elem& operator[](int i) const
  {
    OZ_ASSERT(uint(i) < uint(size_));

    return data_[i];
  }

  /**
   * Reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator[](int i)
  {
    OZ_ASSERT(uint(i) < uint(size_));

    return data_[i];
  }

  /**
   * Constant reference to the first element.
   */
  OZ_ALWAYS_INLINE
  const Elem& first() const
  {
    OZ_ASSERT(size_ != 0);

    return data_[0];
  }

  /**
   * Reference to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem& first()
  {
    OZ_ASSERT(size_ != 0);

    return data_[0];
  }

  /**
   * Constant reference to the last element.
   */
  OZ_ALWAYS_INLINE
  const Elem& last() const
  {
    OZ_ASSERT(size_ != 0);

    return data_[size_ - 1];
  }

  /**
   * Reference to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem& last()
  {
    OZ_ASSERT(size_ != 0);

    return data_[size_ - 1];
  }

  /**
   * True iff a given value is found in the list.
   */
  template <typename Key>
  bool contains(const Key& key) const
  {
    return Arrays::contains<Elem, Key>(data_, size_, key);
  }

  /**
   * Index of the first occurrence of the value or -1 if not found.
   */
  template <typename Key>
  int index(const Key& key) const
  {
    return Arrays::index<Elem, Key>(data_, size_, key);
  }

  /**
   * Index of the last occurrence of the value or -1 if not found.
   */
  template <typename Key>
  int lastIndex(const Key& key) const
  {
    return Arrays::lastIndex<Elem, Key>(data_, size_, key);
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_>
  Elem& add(Elem_&& elem)
  {
    return insert<Elem_>(size_, static_cast<Elem_&&>(elem));
  }

  /**
   * Add (copy) elements from a given array to the end.
   */
  void addAll(const Elem* array, int arraySize)
  {
    int newSize = size_ + arraySize;

    OZ_ASSERT(uint(newSize) <= uint(CAPACITY));

    Arrays::copy<Elem>(array, arraySize, data_ + size_);
    size_ = newSize;
  }

  /**
   * Add (move) elements from a given array to the end.
   */
  void takeAll(Elem* array, int arraySize)
  {
    int newSize = size_ + arraySize;

    OZ_ASSERT(uint(newSize) <= uint(CAPACITY));

    Arrays::move<Elem>(array, arraySize, data_ + size_);
    size_ = newSize;
  }

  /**
   * Add an element to the end if there is no equal element in the list.
   *
   * @return Position of the inserted or the existing equal element.
   */
  template <typename Elem_>
  Elem& include(Elem_&& elem)
  {
    int i = Arrays::index<Elem, Elem>(data_, size_, elem);

    if (i >= 0) {
      return data_[i];
    }
    else {
      return insert<Elem_>(size_, static_cast<Elem_&&>(elem));
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
    OZ_ASSERT(uint(i) <= uint(size_));
    OZ_ASSERT(uint(size_) < uint(CAPACITY));

    Arrays::moveBackward<Elem>(data_ + i, size_ - i, data_ + i + 1);
    data_[i] = static_cast<Elem_&&>(elem);
    ++size_;

    return data_[i];
  }

  /**
   * Remove the element at a given position.
   *
   * All later elements are shifted to fill the gap.
   */
  void erase(int i)
  {
    OZ_ASSERT(uint(i) < uint(size_));

    --size_;

    if (i == size_) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data_[size_] = Elem();
    }
    else {
      Arrays::move<Elem>(data_ + i + 1, size_ - i, data_ + i);
    }
  }

  /**
   * Remove the element at a given position from an unordered list.
   *
   * The last element is moved to its place.
   */
  void eraseUnordered(int i)
  {
    OZ_ASSERT(uint(i) < uint(size_));

    --size_;

    if (i == size_) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data_[size_] = Elem();
    }
    else {
      data_[i] = static_cast<Elem&&>(data_[size_]);
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
    int i = Arrays::index<Elem, Key>(data_, size_, key);

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
    int i = Arrays::index<Elem, Key>(data_, size_, key);

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
  Elem& pushFirst(Elem_&& elem)
  {
    return insert<Elem_>(0, static_cast<Elem_&&>(elem));
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_>
  Elem& pushLast(Elem_&& elem)
  {
    return insert<Elem_>(size_, static_cast<Elem_&&>(elem));
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
    if (size_ == 0) {
      return Elem();
    }
    else {
      Elem elem = static_cast<Elem&&>(data_[0]);

      --size_;
      Arrays::move<Elem>(data_ + 1, size_, data_);
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
    if (size_ == 0) {
      return Elem();
    }
    else {
      --size_;
      return static_cast<Elem&&>(data_[size_]);
    }
  }

  /**
   * Reverse elements.
   */
  void reverse()
  {
    Arrays::reverse<Elem>(data_, size_);
  }

  /**
   * Sort elements with quicksort.
   */
  template <class LessFunc = Less<Elem>>
  void sort()
  {
    Arrays::sort<Elem, LessFunc>(data_, size_);
  }

  /**
   * Resize the list to the specified number of elements.
   */
  void resize(int newCount)
  {
    OZ_ASSERT(newCount <= CAPACITY);

    Arrays::clear<Elem>(data_ + newCount, size_ - newCount);
    size_ = newCount;
  }

  /**
   * Clear the list.
   */
  void clear()
  {
    Arrays::clear<Elem>(data_, size_);
    size_ = 0;
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    Arrays::free<Elem>(data_, size_);
    size_ = 0;
  }

};

}
