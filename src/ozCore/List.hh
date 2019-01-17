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
 * @file ozCore/List.hh
 *
 * `List` class template.
 */

#pragma once

#include "Arrays.hh"

namespace oz
{

/**
 * Array list.
 *
 * In contrast with `std::vector` all allocated elements are constructed all the time. This yields
 * slightly better performance and simplifies implementation. When an element is removed its
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::SList`, `oz::Set`, `oz::Heap`
 */
template <typename Elem>
class List
{
public:

  /**
   * %Range with constant access to elements.
   */
  using CRangeType = Arrays::CRangeType<Elem>;

  /**
   * %Range with non-constant access to elements.
   */
  using RangeType = Arrays::RangeType<Elem>;

protected:

  Elem* data_     = nullptr; ///< Array of elements.
  int   size_     = 0;       ///< Number of elements.
  int   capacity_ = 0;       ///< Capacity, number of elements in storage.

protected:

  /**
   * Ensure a given capacity.
   *
   * The capacity is increased if necessary with growth factor 1.5 or to (at least) 8 slots as the
   * initial allocation.
   */
  void ensureCapacity(int requestedCapacity)
  {
    if (requestedCapacity < 0) {
      OZ_ERROR("oz::List: Negative capacity (overflow?)");
    }
    else if (capacity_ < requestedCapacity) {
      capacity_ = capacity_ == 0 ? 8 : capacity_ + capacity_ / 2;
      capacity_ = max<int>(capacity_, requestedCapacity);
      data_     = Arrays::reallocate<Elem>(data_, size_, capacity_);
    }
  }

public:

  /**
   * Create an empty list.
   */
  List() = default;

  /**
   * Create a list with a given initial length and capacity.
   */
  explicit List(int size)
    : data_(Arrays::reallocate<Elem>(nullptr, 0, size)), size_(size), capacity_(size)
  {}

  /**
   * Initialise from a C++ array.
   */
  explicit List(const Elem* array, int size)
    : List(size)
  {
    Arrays::copy<Elem>(array, size, data_);
  }

  /**
   * Initialise from an initialiser list.
   */
  List(initializer_list<Elem> il)
    : List(il.begin(), int(il.size()))
  {}

  /**
   * Destructor.
   */
  ~List()
  {
    delete[] data_;
  }

  /**
   * Copy constructor, copies elements.
   */
  List(const List& other)
    : List(other.data_, other.size_)
  {}

  /**
   * Move constructor, moves element storage.
   */
  List(List&& other) noexcept
    : List()
  {
    swap(*this, other);
  }

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  List& operator=(const List& other)
  {
    if (&other != this) {
      size_ = 0;
      addAll(other.data_, other.size_);
      Arrays::clear<Elem>(data_ + other.size_, size_ - other.size_);
    }
    return *this;
  }

  /**
   * Move operator, swaps contents.
   */
  List& operator=(List&& other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  List& operator=(initializer_list<Elem> il)
  {
    Arrays::clear<Elem>(data_ + il.size(), size_ - il.size());
    size_ = 0;

    addAll(il.begin(), int(il.size()));
    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator==(const List& other) const
  {
    return size_ == other.size_ && Arrays::equals<Elem>(data_, size_, other.data_);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* cbegin() const noexcept
  {
    return data_;
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* begin() const noexcept
  {
    return data_;
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin() noexcept
  {
    return data_;
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* cend() const noexcept
  {
    return data_ + size_;
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* end() const noexcept
  {
    return data_ + size_;
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end() noexcept
  {
    return data_ + size_;
  }

  /**
   * Number of elements.
   */
  OZ_ALWAYS_INLINE
  int size() const noexcept
  {
    return size_;
  }

  /**
   * True iff empty (no storage is allocated).
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return size_ == 0;
  }

  /**
   * Number of allocated elements.
   */
  OZ_ALWAYS_INLINE
  int capacity() const noexcept
  {
    return capacity_;
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

    ensureCapacity(newSize);

    Arrays::copy<Elem>(array, arraySize, data_ + size_);
    size_ = newSize;
  }

  /**
   * Add (move) elements from a given array to the end.
   */
  void takeAll(Elem* array, int arraySize)
  {
    int newSize = size_ + arraySize;

    ensureCapacity(newSize);

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
    return insert<Elem_>(size_, static_cast<Elem_&&>(elem));
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

    ensureCapacity(size_ + 1);

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

    Elem elem = static_cast<Elem&&>(data_[0]);

    --size_;
    Arrays::move<Elem>(data_ + 1, size_, data_);
    return elem;
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

    --size_;
    return static_cast<Elem&&>(data_[size_]);
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
   * Resize the list (and optionally its capacity too) to the specified number of elements.
   */
  void resize(int newSize, bool exactCapacity = false)
  {
    if (exactCapacity) {
      if (newSize != capacity_) {
        data_     = Arrays::reallocate<Elem>(data_, size_, newSize);
        capacity_ = newSize;
      }
    }
    else {
      ensureCapacity(newSize);
      Arrays::clear<Elem>(data_ + newSize, size_ - newSize);
    }

    size_ = newSize;
  }

  /**
   * Increase capacity (exactly) to the given value if smaller.
   */
  void reserve(int capacity, bool exactCapacity = false)
  {
    if (exactCapacity) {
      if (capacity_ < capacity) {
        data_     = Arrays::reallocate<Elem>(data_, size_, capacity);
        capacity_ = capacity;
      }
    }
    else {
      ensureCapacity(capacity);
    }
  }

  /**
   * Trim capacity to the current number of elements.
   */
  void trim()
  {
    if (size_ < capacity_) {
      data_     = Arrays::reallocate<Elem>(data_, size_, size_);
      capacity_ = size_;
    }
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
