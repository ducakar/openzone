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

#include "System.hh"
#include "String.hh"

namespace oz
{

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
class EnumMap
{
private:

  /**
   * EnumMap entry.
   */
  struct Entry
  {
    int         value; ///< Enumerator value as integer.
    const char* name;  ///< Enumerator value name.
  };

  Entry* entries;  ///< %Map entries.
  int    nEntries; ///< Number of map entries.

public:

  /**
   * Create enumerator mapping from an array.
   */
  EnumMap( InitialiserList<Entry> l ) :
    entries( new Entry[ l.size() ] ), nEntries( int( l.size() ) )
  {
    hard_assert( l.size() != 0 );

    aCopy<Entry>( l.begin(), int( l.size() ), entries );
  }

  /**
   * Destructor
   */
  ~EnumMap()
  {
    delete[] entries;
  }

  /**
   * Return first enumerator value in the map.
   */
  Enum defaultValue() const
  {
    return Enum( entries[0].value );
  }

  /**
   * Return first name in the map.
   */
  const char* defaultName() const
  {
    return entries[0].name;
  }

  /**
   * True iff a given enumerator value exists in the map.
   */
  bool has( Enum value ) const
  {
    for( int i = 0; i < nEntries; ++i ) {
      if( entries[i].value == value ) {
        return true;
      }
    }
    return false;
  }

  /**
   * True iff a given name exists in the map.
   */
  bool has( const char* name ) const
  {
    for( int i = 0; i < nEntries; ++i ) {
      if( String::equals( entries[i].name, name ) ) {
        return true;
      }
    }
    return false;
  }

  /**
   * Return name for a enumerator value or invoke `System::error()` on an invalid value.
   */
  const char* operator [] ( Enum value ) const
  {
    for( int i = 0; i < nEntries; ++i ) {
      if( entries[i].value == value ) {
        return entries[i].name;
      }
    }

    OZ_ERROR( "oz::EnumMap: Invalid value %d", value );
  }

  /**
   * Return enumerator value for a given name or invoke `System::error()` on an invalid name.
   */
  Enum operator[] ( const char* name ) const
  {
    for( int i = 0; i < nEntries; ++i ) {
      if( String::equals( entries[i].name, name ) ) {
        return Enum( entries[i].value );
      }
    }

    OZ_ERROR( "oz::EnumMap: Invalid name '%s'", name );
  }

};

}
