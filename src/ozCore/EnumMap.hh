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
 * @file ozCore/EnumMap.hh
 *
 * `EnumMap` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Internal EnumMap implementation.
 */
namespace detail
{

class EnumMapImpl
{
protected:

  /**
   * EnumMap entry.
   */
  struct Entry
  {
    int         value; ///< Enumerator value as integer.
    const char* name;  ///< Enumerator value name.
  };

  Entry* entries;  ///< %Map of entries.
  int    nEntries; ///< Number of map entries.

public:

  /**
   * Create enumerator mapping from an array.
   */
  EnumMapImpl( InitialiserList<Entry> l );

  /**
   * Destructor
   */
  ~EnumMapImpl();

  /**
   * No copying.
   */
  EnumMapImpl( const EnumMapImpl& ) = delete;

  /**
   * No copying.
   */
  EnumMapImpl& operator = ( const EnumMapImpl& ) = delete;

  /**
   * Return first enumerator value in the map.
   */
  int defaultValue() const;

  /**
   * Return first name in the map.
   */
  const char* defaultName() const;

  /**
   * True iff a given enumerator value exists in the map.
   */
  bool has( int value ) const;

  /**
   * True iff a given name exists in the map.
   */
  bool has( const char* name ) const;

  /**
   * Return name for a enumerator value or invoke `System::error()` on an invalid value.
   */
  const char* operator [] ( int value ) const;

  /**
   * Return enumerator value for a given name or invoke `System::error()` on an invalid name.
   */
  int operator[] ( const char* name ) const;

};

}

/**
 * %Map between enumerator values and their string representations.
 *
 * It can be used like:
 * @code
 * enum MyEnum
 * {
 *   FOO = 1,
 *   BAR = 42
 * };
 *
 * EnumMap<MyEnum> myEnumMap = {
 *   { FOO, "FOO" },
 *   { BAR, "BAR" }
 * };
 * Log() << map[FOO];   // "FOO"
 * Log() << map["BAR"]; // 42
 * @endcode
 */
template <class Enum = int>
class EnumMap : public detail::EnumMapImpl
{
public:

  /**
   * Create enumerator mapping from an initialiser list.
   */
  EnumMap( InitialiserList<Entry> l ) :
    EnumMapImpl( l )
  {}

  /**
   * Return first enumerator value in the map.
   */
  Enum defaultValue() const
  {
    return Enum( EnumMapImpl::defaultValue() );
  }

  /**
   * Return enumerator value for a given name or invoke `System::error()` on an invalid name.
   */
  Enum operator[] ( const char* name ) const
  {
    return Enum( EnumMapImpl::operator [] ( name ) );
  }

};

}
