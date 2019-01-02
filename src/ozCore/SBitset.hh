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
 * @file ozCore/SBitset.hh
 *
 * `SBitset` class.
 */

#pragma once

#include "Alloc.hh"
#include "SList.hh"

namespace oz
{

/**
 * Bit array with static storage.
 *
 * Bits are stored in an array of `uint64`s so its size is always a multiple of 64.
 *
 * @sa `oz::Bitset`
 */
template <int BITS>
class SBitset
{
  static_assert(BITS > 0, "oz::SBitset length must be at least 1");

private:

  /// Number of bits per the internal unit.
  static constexpr int UNIT_BITS = sizeof(uint64) * 8;

  /// Number of units.
  static constexpr int SIZE = (BITS + UNIT_BITS - 1) / UNIT_BITS;

private:

  SList<uint64, SIZE> data_; ///< Bit storage.

public:

  /**
   * Initialise all bits to zero.
   */
  SBitset()
    : data_(SIZE)
  {}

  /**
   * Initialise from a string of zeros and ones.
   *
   * Characters other than '0' are treated as ones. The rest of the bitset is initialised to zeros
   * if the string is shorter than the bitset.
   */
  explicit SBitset(const char* s)
    : data_(SIZE)
  {
    for (int i = 0; s[i] != '\0'; ++i) {
      data_[i / UNIT_BITS] |= uint64(s[i] != '0') << (i % UNIT_BITS);
    }
  }

  /**
   * True iff all bits are equal.
   */
  bool operator==(const SBitset& other) const
  {
    return data_ == other.data_;
  }

  /**
   * Constant pointer to the first unit.
   */
  OZ_ALWAYS_INLINE
  const uint64* begin() const noexcept
  {
    return data_.begin();
  }

  /**
   * Pointer to the first unit.
   */
  OZ_ALWAYS_INLINE
  uint64* begin() noexcept
  {
    return data_.begin();
  }

  /**
   * Constant pointer past the last unit.
   */
  OZ_ALWAYS_INLINE
  const uint64* end() const noexcept
  {
    return data_.end();
  }

  /**
   * Pointer past the last unit.
   */
  OZ_ALWAYS_INLINE
  uint64* end() noexcept
  {
    return data_.end();
  }

  /**
   * Size in bits.
   */
  OZ_ALWAYS_INLINE
  int size() const noexcept
  {
    return SIZE * UNIT_BITS;
  }

  /**
   * Always false.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return false;
  }

  /**
   * True iff all bits are true.
   */
  bool isAllSet() const
  {
    for (int i = 0; i < SIZE; ++i) {
      if (~data_[i] != 0) {
        return false;
      }
    }
    return true;
  }

  /**
   * True iff at least one bit is true.
   */
  bool isAnySet() const
  {
    for (int i = 0; i < SIZE; ++i) {
      if (data_[i] != 0) {
        return true;
      }
    }
    return false;
  }

  /**
   * True iff all bits are false.
   */
  bool isNoneSet() const
  {
    return !isAnySet();
  }

  /**
   * True iff this bitset is a subset of a given bitset.
   *
   * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
   * subset of the second one. Other explanation: the result is true iff the following statement is
   * true: if first bitset has true on the i-th position then the second bitset also has true on the
   * i-th position.
   */
  bool isSubset(const SBitset& b) const
  {
    for (int i = 0; i < SIZE; ++i) {
      if ((data_[i] & ~b.data_[i]) != 0) {
        return false;
      }
    }
    return true;
  }

  /**
   * Get the `i`-th bit.
   */
  OZ_ALWAYS_INLINE
  bool get(int i) const
  {
    return (data_[i / UNIT_BITS] & (uint64(1) << (i % UNIT_BITS))) != 0;
  }

  /**
   * %Set the `i`-th bit to true.
   */
  OZ_ALWAYS_INLINE
  void set(int i)
  {
    data_[i / UNIT_BITS] |= uint64(1) << (i % UNIT_BITS);
  }

  /**
   * %Set the `i`-th bit to false.
   */
  OZ_ALWAYS_INLINE
  void clear(int i)
  {
    data_[i / UNIT_BITS] &= ~(uint64(1) << (i % UNIT_BITS));
  }

  /**
   * Flip the `i`-th bit's value.
   */
  OZ_ALWAYS_INLINE
  void flip(int i)
  {
    data_[i / UNIT_BITS] ^= uint64(1) << (i % UNIT_BITS);
  }

  /**
   * %Set all bits to false.
   */
  void clear()
  {
    Arrays::fill<uint64, uint64>(data_.begin(), SIZE, 0);
  }

  /**
   * NOT of the bitset.
   */
  SBitset operator~() const
  {
    SBitset r = *this;
    return r.flip();
  }

  /**
   * Return AND of two bitsets.
   */
  SBitset operator&(const SBitset& b) const
  {
    SBitset r = *this;
    return r &= b;
  }

  /**
   * Return OR of two bitsets.
   */
  SBitset operator|(const SBitset& b) const
  {
    SBitset r = *this;
    return r |= b;
  }

  /**
   * Return XOR of two bitsets.
   */
  SBitset operator^(const SBitset& b) const
  {
    SBitset r = *this;
    return r ^= b;
  }

  /**
   * NOT of the bitset.
   */
  SBitset& flip()
  {
    for (int i = 0; i < SIZE; ++i) {
      data_[i] = ~data_[i];
    }
    return *this;
  }

  /**
   * AND of two bitsets.
   */
  SBitset& operator&=(const SBitset& b)
  {
    for (int i = 0; i < SIZE; ++i) {
      data_[i] &= b.data_[i];
    }
    return *this;
  }

  /**
   * OR of two bitsets.
   */
  SBitset& operator|=(const SBitset& b)
  {
    for (int i = 0; i < SIZE; ++i) {
      data_[i] |= b.data_[i];
    }
    return *this;
  }

  /**
   * XOR of two bitsets.
   */
  SBitset& operator^=(const SBitset& b)
  {
    for (int i = 0; i < SIZE; ++i) {
      data_[i] ^= b.data_[i];
    }
    return *this;
  }

};

}
