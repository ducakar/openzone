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

#include "Bitset.hh"

#include "Alloc.hh"
#include "String.hh"

namespace oz
{

Bitset::Bitset(int nBits)
  : data_((nBits + UNIT_BITS - 1) / UNIT_BITS)
{}

Bitset::Bitset(const char* s)
  : Bitset(String::length(s))
{
  for (int i = 0; s[i] != '\0'; ++i) {
    data_[i / UNIT_BITS] |= uint64(s[i] != '0') << (i % UNIT_BITS);
  }
}

bool Bitset::operator==(const Bitset& other) const
{
  return data_ == other.data_;
}

bool Bitset::isAllSet() const
{
  for (uint64 u : data_) {
    if (~u != 0) {
      return false;
    }
  }
  return true;
}

bool Bitset::isAnySet() const
{
  for (uint64 u : data_) {
    if (u != 0) {
      return true;
    }
  }
  return false;
}

bool Bitset::isNoneSet() const
{
  return !isAnySet();
}

bool Bitset::isSubset(const Bitset& b) const
{
  OZ_ASSERT(data_.size() == b.data_.size());

  for (int i = 0; i < data_.size(); ++i) {
    if ((data_[i] & ~b.data_[i]) != 0) {
      return false;
    }
  }
  return true;
}

void Bitset::clear()
{
  Arrays::fill<uint64, uint64>(data_.begin(), data_.size(), 0);
}

Bitset Bitset::operator~() const
{
  Bitset r = *this;
  return r.flip();
}

Bitset Bitset::operator&(const Bitset& b) const
{
  OZ_ASSERT(data_.size() == b.data_.size());

  Bitset r = *this;
  return r &= b;
}

Bitset Bitset::operator|(const Bitset& b) const
{
  OZ_ASSERT(data_.size() == b.data_.size());

  Bitset r = *this;
  return r |= b;
}

Bitset Bitset::operator^(const Bitset& b) const
{
  OZ_ASSERT(data_.size() == b.data_.size());

  Bitset r = *this;
  return r ^= b;
}

Bitset& Bitset::flip()
{
  for (uint64& u : data_) {
    u = ~u;
  }
  return *this;
}

Bitset& Bitset::operator&=(const Bitset& b)
{
  for (int i = 0; i < data_.size(); ++i) {
    data_[i] &= b.data_[i];
  }
  return *this;
}

Bitset& Bitset::operator|=(const Bitset& b)
{
  for (int i = 0; i < data_.size(); ++i) {
    data_[i] |= b.data_[i];
  }
  return *this;
}

Bitset& Bitset::operator^=(const Bitset& b)
{
  for (int i = 0; i < data_.size(); ++i) {
    data_[i] ^= b.data_[i];
  }
  return *this;
}

void Bitset::resize(int nBits)
{
  OZ_ASSERT(nBits >= 0);

  data_.resize((nBits + UNIT_BITS - 1) / UNIT_BITS, true);
}

}
