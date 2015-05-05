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
 * @file ozCore/EnumMap.cc
 */

#include "EnumMap.hh"

#include "String.hh"

namespace oz
{

namespace detail
{

EnumMapImpl::EnumMapImpl(InitialiserList<Entry> l) :
  entries(new Entry[l.size()]), nEntries(int(l.size()))
{
  hard_assert(l.size() != 0);

  Arrays::copy<Entry>(l.begin(), int(l.size()), entries);
}

EnumMapImpl::~EnumMapImpl()
{
  delete[] entries;
}

int EnumMapImpl::length() const
{
  return nEntries;
}

int EnumMapImpl::defaultValue() const
{
  return entries[0].value;
}

const char* EnumMapImpl::defaultName() const
{
  return entries[0].name;
}

bool EnumMapImpl::has(int value) const
{
  for (int i = 0; i < nEntries; ++i) {
    if (entries[i].value == value) {
      return true;
    }
  }
  return false;
}

bool EnumMapImpl::has(const char* name) const
{
  for (int i = 0; i < nEntries; ++i) {
    if (String::equals(entries[i].name, name)) {
      return true;
    }
  }
  return false;
}

const char* EnumMapImpl::operator [] (int value) const
{
  for (int i = 0; i < nEntries; ++i) {
    if (entries[i].value == value) {
      return entries[i].name;
    }
  }

  OZ_ERROR("oz::EnumMap: Invalid value %d", value);
}

int EnumMapImpl::operator [] (const char* name) const
{
  for (int i = 0; i < nEntries; ++i) {
    if (String::equals(entries[i].name, name)) {
      return entries[i].value;
    }
  }

  OZ_ERROR("oz::EnumMap: Invalid name '%s'", name);
}

}

}
