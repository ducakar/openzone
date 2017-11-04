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
 * @file ozCore/Box.hh
 *
 * `Box` class template.
 */

#pragma once

#include "Arrays.hh"

namespace oz
{

/**
 * Unique pointer to a heap-allocated object.
 */
template <typename Type>
class Box
{
private:

  Type* data_ = nullptr; ///< Wrapped pointer.

public:

  /**
   * Create an empty box.
   */
  OZ_ALWAYS_INLINE
  constexpr Box() = default;

  /**
   * Create an owner box for a given object.
   */
  OZ_ALWAYS_INLINE
  constexpr Box(Type* data)
    : data_(data)
  {}

  /**
   * Release the object.
   */
  ~Box()
  {
    delete data_;
  }

  /**
   * Create a new owner, the source box is emptied.
   */
  OZ_ALWAYS_INLINE
  constexpr Box(Box&& other) noexcept
    : data_(other.data_)
  {
    other.data_ = nullptr;
  }

  /**
   * Empty and take ownership, the source box is emptied.
   */
  Box& operator=(Box&& other) noexcept
  {
    if (&other != this) {
      delete data_;

      data_ = other.data_;

      other.data_ = nullptr;
    }

    return *this;
  }

  /**
   * Empty and take ownership, the source box is emptied.
   */
  Box& operator=(Type* data)
  {
    delete data_;

    data_ = data;

    return *this;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const Box& other) const
  {
    return data_ == other.data_;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const Type* p) const
  {
    return data_ == p;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  friend bool operator==(const Type* p, const Box& box)
  {
    return p == box.data_;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  bool operator<(const Box& other) const
  {
    return data_ < other.data_;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  bool operator<(const Type* p) const
  {
    return data_ < p;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  friend bool operator<(const Type* p, const Box& box)
  {
    return p < box.data_;
  }

  /**
   * Constant pointer to the object.
   */
  OZ_ALWAYS_INLINE
  const Type* get() const noexcept
  {
    return data_;
  }

  /**
   * Pointer to the object.
   */
  OZ_ALWAYS_INLINE
  Type* get() noexcept
  {
    return data_;
  }

  /**
   * True iff not null.
   */
  OZ_ALWAYS_INLINE
  explicit operator bool() const noexcept
  {
    return data_ != nullptr;
  }

  /**
   * Constant reference to the object.
   */
  OZ_ALWAYS_INLINE
  const Type& operator*() const
  {
    return *data_;
  }

  /**
   * Reference to the object.
   */
  OZ_ALWAYS_INLINE
  Type& operator*()
  {
    return *data_;
  }

  /**
   * Constant pointer to the object's member.
   */
  OZ_ALWAYS_INLINE
  const Type* operator->() const noexcept
  {
    return data_;
  }

  /**
   * Pointer to the object's member.
   */
  OZ_ALWAYS_INLINE
  Type* operator->() noexcept
  {
    return data_;
  }

};

/**
 * Unique pointer to a heap-allocated array.
 */
template <typename Type>
class Box<Type[]>
{
private:

  Type* data_ = nullptr; ///< Wrapped pointer.

public:

  /**
   * Create an empty box.
   */
  OZ_ALWAYS_INLINE
  constexpr Box() = default;

  /**
   * Create an owner box for a given array.
   */
  OZ_ALWAYS_INLINE
  constexpr Box(Type data[])
    : data_(data)
  {}

  /**
   * Release the array.
   */
  ~Box()
  {
    delete[] data_;
  }

  /**
   * Create a new owner, the source box is emptied.
   */
  OZ_ALWAYS_INLINE
  constexpr Box(Box&& other) noexcept
    : data_(other.data_)
  {
    other.data_ = nullptr;
  }

  /**
   * Empty and take ownership, the source box is emptied.
   */
  Box& operator=(Box&& other) noexcept
  {
    if (&other != this) {
      delete[] data_;

      data_ = other.data_;

      other.data_ = nullptr;
    }

    return *this;
  }

  /**
   * Empty and take ownership.
   */
  Box& operator=(Type data[])
  {
    delete[] data_;

    data_ = data;
    return *this;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const Box& other) const
  {
    return data_ == other.data_;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const Type* data) const
  {
    return data_ == data;
  }

  /**
   * True iff pointing to the same location.
   */
  OZ_ALWAYS_INLINE
  friend bool operator==(const Type* data, const Box& box)
  {
    return data == box.data_;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  bool operator<(const Box& other) const
  {
    return data_ < other.data_;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  bool operator<(const Type* data) const
  {
    return data_ < data;
  }

  /**
   * True iff pointing to a lower location.
   */
  OZ_ALWAYS_INLINE
  friend bool operator<(const Type* data, const Box& box)
  {
    return data < box.data_;
  }

  /**
   * Constant pointer to the first element.
   */
  OZ_ALWAYS_INLINE
  const Type* get() const noexcept
  {
    return data_;
  }

  /**
   * Pointer to the first element.
   */
  OZ_ALWAYS_INLINE
  Type* get() noexcept
  {
    return data_;
  }

  /**
   * True iff not null.
   */
  OZ_ALWAYS_INLINE
  explicit operator bool() const noexcept
  {
    return data_ != nullptr;
  }

  /**
   * Constant reference to the i-th element.
   */
  OZ_ALWAYS_INLINE
  const Type& operator[](int i) const
  {
    return data_[i];
  }

  /**
   * Reference to the i-th element.
   */
  OZ_ALWAYS_INLINE
  Type& operator[](int i)
  {
    return data_[i];
  }

  /**
   * Reallocate an array to a different size, moving its elements.
   *
   * Allocate a new zero-initialised array of `newSize` elements, move first `min(size, newSize)`
   * elements to the newly created array and free the original array.
   *
   * If `newSize` is 0 the existing array is deleted and replaced by `nullptr`.
   */
  void reallocate(int size, int newSize)
  {
    OZ_ASSERT(size >= 0 && newSize >= 0);

    Type* newData = nullptr;

    if (newSize != 0) {
      newData = new Type[newSize] {};
      Arrays::move<Type>(data_, min<int>(size, newSize), newData);
    }

    delete[] data_;
    data_ = newData;
  }

};

}
