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
 * @file ozCore/Bitset.hh
 *
 * `Bitset` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Bit array with dynamically allocated storage.
 *
 * Bits are stored in an array of `ulong`s, so the its length in bits is always a multiple of
 * `sizeof(ulong) * 8`.
 *
 * @sa `oz::SBitset`
 */
class Bitset
{
private:

  /// Size of unit in bytes.
  static const int UNIT_SIZE = int(sizeof(ulong));

  /// Number of bits per unit.
  static const int UNIT_BITSIZE = int(sizeof(ulong)) * 8;

  ulong* data = nullptr; ///< Pointer to array of units that holds the data.
  int    size = 0;       ///< Size of data array (in units, not in bits).

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
   * Destructor.
   */
  ~Bitset();

  /**
   * Copy constructor, copies storage.
   */
  Bitset(const Bitset& b);

  /**
   * Move constructor, moves storage.
   */
  Bitset(Bitset&& b);

  /**
   * Copy operator, copies storage.
   *
   * Existing storage is reused if its size matches.
   */
  Bitset& operator = (const Bitset& b);

  /**
   * Move operator, moves storage.
   */
  Bitset& operator = (Bitset&& b);

  /**
   * True iff same size and respective bits are equal.
   */
  bool operator == (const Bitset& b) const;

  /**
   * True iff different size or any respective bits differ.
   */
  bool operator != (const Bitset& b) const;

  /**
   * Get constant pointer to `data` array.
   */
  OZ_ALWAYS_INLINE
  operator const ulong* () const
  {
    return data;
  }

  /**
   * Get pointer to `data` array.
   */
  OZ_ALWAYS_INLINE
  operator ulong* ()
  {
    return data;
  }

  /**
   * Size in bits.
   */
  OZ_ALWAYS_INLINE
  int length() const
  {
    return size * UNIT_BITSIZE;
  }

  /**
   * Size in units.
   */
  OZ_ALWAYS_INLINE
  int unitLength() const
  {
    return size;
  }

  /**
   * True iff empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return size == 0;
  }

  /**
   * Get the `i`-th bit.
   */
  OZ_ALWAYS_INLINE
  bool get(int i) const
  {
    hard_assert(uint(i) < uint(size * UNIT_BITSIZE));

    return (data[i / UNIT_BITSIZE] & (1ul << (i % UNIT_BITSIZE))) != 0ul;
  }

  /**
   * %Set the `i`-th bit to true.
   */
  OZ_ALWAYS_INLINE
  void set(int i)
  {
    hard_assert(uint(i) < uint(size * UNIT_BITSIZE));

    data[i / UNIT_BITSIZE] |= 1ul << (i % UNIT_BITSIZE);
  }

  /**
   * %Set the `i`-th bit to false.
   */
  OZ_ALWAYS_INLINE
  void clear(int i)
  {
    hard_assert(uint(i) < uint(size * UNIT_BITSIZE));

    data[i / UNIT_BITSIZE] &= ~(1ul << (i % UNIT_BITSIZE));
  }

  /**
   * True iff all bits are true.
   */
  bool isAllSet() const;

  /**
   * True iff all bits are false.
   */
  bool isAllClear() const;

  /**
   * True iff this bitset is a subset of a given bitset.
   *
   * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
   * subset of the second one. Other explanation: the result is true iff the following statement is
   * true: if first bitset has true on the i-th position then the second bitset also has true on the
   * i-th position.
   *
   * Both bitsets must be the same size.
   */
  bool isSubset(const Bitset& b) const;

  /**
   * %Set bits from inclusively start to non-inclusively end to true.
   */
  void set(int start, int end);

  /**
   * %Set bits from inclusively start to non-inclusively end to false.
   */
  void clear(int start, int end);

  /**
   * %Set all bits to true.
   */
  void setAll();

  /**
   * %Set all bits to false.
   */
  void clearAll();

  /**
   * NOT of the bitset.
   */
  Bitset operator ~ () const;

  /**
   * Return AND of two same-length bitsets.
   */
  Bitset operator & (const Bitset& b) const;

  /**
   * Return OR of two same-length bitsets.
   */
  Bitset operator | (const Bitset& b) const;

  /**
   * Return XOR of two same-length bitsets.
   */
  Bitset operator ^ (const Bitset& b) const;

  /**
   * AND of two same-length bitsets.
   */
  Bitset& operator &= (const Bitset& b);

  /**
   * OR of two same-length bitsets.
   */
  Bitset& operator |= (const Bitset& b);

  /**
   * XOR of two same-length bitsets.
   */
  Bitset& operator ^= (const Bitset& b);

  /**
   * Resize bitmap.
   *
   * Resizing to zero frees all allocated storage. The added bits are initialised to zero.
   */
  void resize(int nBits);

};

}
