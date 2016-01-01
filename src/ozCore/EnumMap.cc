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

#include "EnumMap.hh"

#include "String.hh"

namespace oz
{

namespace detail
{

int EnumMapImpl::defaultValue() const
{
  return entries[0].key;
}

int EnumMapImpl::operator [] (const char* name) const
{
  for (const Pair& pair : entries) {
    if (String::equals(pair.value, name)) {
      return pair.key;
    }
  }

  OZ_ERROR("oz::EnumMap: Invalid name '%s'", name);
}

EnumMapImpl::EnumMapImpl(InitialiserList<Pair> l) :
  entries(l)
{}

const char* EnumMapImpl::defaultName() const
{
  return entries[0].value;
}

bool EnumMapImpl::has(int value) const
{
  return entries.contains<int>(value);
}

bool EnumMapImpl::has(const char* name) const
{
  for (const Pair& pair : entries) {
    if (String::equals(pair.value, name)) {
      return true;
    }
  }
  return false;
}

const char* EnumMapImpl::operator [] (int value) const
{
  const char* const* name = entries.find(value);

  if (name == nullptr) {
    OZ_ERROR("oz::EnumMap: Invalid value %d", value);
  }
  return *name;
}

}

}
