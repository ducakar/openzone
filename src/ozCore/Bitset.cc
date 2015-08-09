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
 * @file ozCore/Bitset.cc
 */

#include "Bitset.hh"

#include "String.hh"

namespace oz
{

Bitset::Bitset(int nBits)
{
  resize(nBits);
}

Bitset::Bitset(const char* s) :
  Bitset(String::length(s))
{
  for (int i = 0; i < bitSize; ++i) {
    data[i / UNIT_BITS] |= size_t(s[i] != '0') << (i % UNIT_BITS);
  }
}

Bitset::~Bitset()
{
  delete[] data;
}

Bitset::Bitset(const Bitset& b) :
  data(Arrays::reallocate<size_t>(nullptr, 0, b.size)), size(b.size), bitSize(b.bitSize)
{
  Arrays::copy<size_t>(b.data, b.size, data);
}

Bitset::Bitset(Bitset&& b) :
  data(b.data), size(b.size), bitSize(b.bitSize)
{
  b.data    = nullptr;
  b.size    = 0;
  b.bitSize = 0;
}

Bitset& Bitset::operator = (const Bitset& b)
{
  if (&b != this) {
    if (size != b.size) {
      delete[] data;

      data = b.size == 0 ? nullptr : new size_t[b.size];
      size = b.size;
    }

    Arrays::copy<size_t>(b.data, b.size, data);
    bitSize = b.bitSize;
  }
  return *this;
}

Bitset& Bitset::operator = (Bitset&& b)
{
  if (&b != this) {
    delete[] data;

    data    = b.data;
    size    = b.size;
    bitSize = b.bitSize;

    b.data    = nullptr;
    b.size    = 0;
    b.bitSize = 0;
  }
  return *this;
}

bool Bitset::operator == (const Bitset& b) const
{
  return bitSize == b.bitSize && Arrays::equals<size_t>(data, size, b.data);
}

bool Bitset::operator != (const Bitset& b) const
{
  return !operator == (b);
}

bool Bitset::isAllSet() const
{
  for (int i = 0; i < size - 1; ++i) {
    if (~data[i] != 0) {
      return false;
    }
  }
  return data[size - 1] == ~(size_t(-1) << (bitSize % UNIT_BITS));
}

bool Bitset::isAnySet() const
{
  for (int i = 0; i < size; ++i) {
    if (data[i] != 0) {
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
  OZ_ASSERT(size == b.size);

  for (int i = 0; i < size; ++i) {
    if ((data[i] & ~b.data[i]) != 0) {
      return false;
    }
  }
  return true;
}

void Bitset::clear()
{
  Arrays::fill<size_t, size_t>(data, size, 0);
}

Bitset Bitset::operator ~ () const
{
  Bitset r = *this;
  return r.flip();
}

Bitset Bitset::operator & (const Bitset& b) const
{
  OZ_ASSERT(size == b.size);

  Bitset r = *this;
  return r &= b;
}

Bitset Bitset::operator | (const Bitset& b) const
{
  OZ_ASSERT(size == b.size);

  Bitset r = *this;
  return r |= b;
}

Bitset Bitset::operator ^ (const Bitset& b) const
{
  OZ_ASSERT(size == b.size);

  Bitset r = *this;
  return r ^= b;
}

Bitset& Bitset::flip()
{
  for (int i = 0; i < size - 1; ++i) {
    data[i] = ~data[i];
  }
  data[size - 1] ^= ~(size_t(-1) << (bitSize % UNIT_BITS));
  return *this;
}

Bitset& Bitset::operator &= (const Bitset& b)
{
  for (int i = 0; i < size; ++i) {
    data[i] &= b.data[i];
  }
  return *this;
}

Bitset& Bitset::operator |= (const Bitset& b)
{
  for (int i = 0; i < size; ++i) {
    data[i] |= b.data[i];
  }
  return *this;
}

Bitset& Bitset::operator ^= (const Bitset& b)
{
  for (int i = 0; i < size; ++i) {
    data[i] ^= b.data[i];
  }
  return *this;
}

void Bitset::resize(int nBits)
{
  OZ_ASSERT(nBits >= 0);

  int nUnits = (nBits + PORT_BITS - 1) / PORT_BITS * (PORT_BITS / UNIT_BITS);

  if (nUnits != size) {
    data = Arrays::reallocate<size_t>(data, size, nUnits);
    size = nUnits;
  }
  bitSize = nBits;
}

}
