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
 * @file ozCore/Bitset.hh
 *
 * `Bitset` class.
 */

#pragma once

#include "List.hh"

namespace oz
{

/**
 * Bit array with dynamically allocated storage.
 *
 * Bits are stored in an array of `size_t`s and its size is always a multiple of 64 bits.
 *
 * @sa `oz::SBitset`
 */
class Bitset
{
private:

  /// Number of bits per the internal unit.
  static const int UNIT_BITS = sizeof(size_t) * 8;

  /// Number of bits per the platfrom-independent unit.
  static const int PORT_BITS = sizeof(ulong64) * 8;

private:

  List<size_t> data_; ///< Bit storage.

public:

  /**
   * Create an empty bitset.
   */
  Bitset() = default;

  /**
   * Allocate a new bitset that holds at least `nBits` bits initialised to 0.
   *
   * The size of `data` array is adjusted to the smallest unit number that can hold the requested
   * number of bits. No memory is allocated if `nBits == 0`.
   */
  explicit Bitset(int nBits);

  /**
   * Initialise from a string of zeros and ones.
   *
   * Characters other than '0' are treated as ones.
   */
  explicit Bitset(const char* s);

  /**
   * True iff same size and respective bits are equal.
   */
  bool operator==(const Bitset& other) const;

  /**
   * True iff different size or any respective bits differ.
   */
  bool operator!=(const Bitset& other) const;

  /**
   * Constant pointer to the first unit.
   */
  OZ_ALWAYS_INLINE
  const size_t* begin() const
  {
    return data_.begin();
  }

  /**
   * Pointer to the first unit.
   */
  OZ_ALWAYS_INLINE
  size_t* begin()
  {
    return data_.begin();
  }

  /**
   * Constant pointer past the last unit.
   */
  OZ_ALWAYS_INLINE
  const size_t* end() const
  {
    return data_.end();
  }

  /**
   * Pointer past the last unit.
   */
  OZ_ALWAYS_INLINE
  size_t* end()
  {
    return data_.end();
  }

  /**
   * Size in bits.
   */
  OZ_ALWAYS_INLINE
  int size() const
  {
    return data_.size() * UNIT_BITS;
  }

  /**
   * True iff empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return data_.isEmpty();
  }

  /**
   * True iff all bits are true.
   */
  bool isAllSet() const;

  /**
   * True iff at least one bit is true.
   */
  bool isAnySet() const;

  /**
   * True iff all bits are false.
   */
  bool isNoneSet() const;

  /**
   * True iff this bitset is a subset of a given bitset.
   *
   * Both bitsets must be the same size.
   */
  bool isSubset(const Bitset& b) const;

  /**
   * Get the `i`-th bit.
   */
  OZ_ALWAYS_INLINE
  bool get(int i) const
  {
    return (data_[i / UNIT_BITS] & (size_t(1) << (i % UNIT_BITS))) != 0;
  }

  /**
   * %Set the `i`-th bit to true.
   */
  OZ_ALWAYS_INLINE
  void set(int i)
  {
    data_[i / UNIT_BITS] |= size_t(1) << (i % UNIT_BITS);
  }

  /**
   * %Set the `i`-th bit to false.
   */
  OZ_ALWAYS_INLINE
  void clear(int i)
  {
    data_[i / UNIT_BITS] &= ~(size_t(1) << (i % UNIT_BITS));
  }

  /**
   * Flip the `i`-th bit's value.
   */
  OZ_ALWAYS_INLINE
  void flip(int i)
  {
    data_[i / UNIT_BITS] ^= size_t(1) << (i % UNIT_BITS);
  }

  /**
   * %Set all bits to false.
   */
  void clear();

  /**
   * NOT of the bitset.
   */
  Bitset operator~() const;

  /**
   * Return AND of two same-length bitsets.
   */
  Bitset operator&(const Bitset& b) const;

  /**
   * Return OR of two same-length bitsets.
   */
  Bitset operator|(const Bitset& b) const;

  /**
   * Return XOR of two same-length bitsets.
   */
  Bitset operator^(const Bitset& b) const;

  /**
   * NOT of the bitset.
   */
  Bitset& flip();

  /**
   * AND of two same-length bitsets.
   */
  Bitset& operator&=(const Bitset& b);

  /**
   * OR of two same-length bitsets.
   */
  Bitset& operator|=(const Bitset& b);

  /**
   * XOR of two same-length bitsets.
   */
  Bitset& operator^=(const Bitset& b);

  /**
   * Resize bitset.
   *
   * Resizing to zero frees all allocated storage. The added bits are initialised to zero.
   */
  void resize(int nBits);

};

}
