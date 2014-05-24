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

#include "arrays.hh"

namespace oz
{

Bitset::Bitset( int nBits ) :
  data( nullptr ), size( 0 )
{
  resize( nBits );
}

Bitset::~Bitset()
{
  delete[] data;
}

Bitset::Bitset( const Bitset& b ) :
  data( b.size == 0 ? nullptr : new ulong[b.size] ), size( b.size )
{
  aCopy<ulong>( b.data, b.size, data );
}

Bitset::Bitset( Bitset&& b ) :
  data( b.data ), size( b.size )
{
  b.data = nullptr;
  b.size = 0;
}

Bitset& Bitset::operator = ( const Bitset& b )
{
  if( &b == this ) {
    return *this;
  }

  if( size != b.size ) {
    delete[] data;

    data = b.size == 0 ? nullptr : new ulong[b.size];
    size = b.size;
  }

  aCopy<ulong>( b.data, b.size, data );

  return *this;
}

Bitset& Bitset::operator = ( Bitset&& b )
{
  if( &b == this ) {
    return *this;
  }

  delete[] data;

  data = b.data;
  size = b.size;

  b.data = nullptr;
  b.size = 0;

  return *this;
}

bool Bitset::operator == ( const Bitset& b ) const
{
  return size == b.size && aEquals<ulong>( data, size, b.data );
}

bool Bitset::operator != ( const Bitset& b ) const
{
  return !operator == ( b );
}

bool Bitset::isAllSet() const
{
  for( int i = 0; i < size; ++i ) {
    if( data[i] != ~0ul ) {
      return false;
    }
  }
  return true;
}

bool Bitset::isAllClear() const
{
  for( int i = 0; i < size; ++i ) {
    if( data[i] != 0ul ) {
      return false;
    }
  }
  return true;
}

bool Bitset::isSubset( const Bitset& b ) const
{
  hard_assert( size == b.size );

  for( int i = 0; i < size; ++i ) {
    if( ( data[i] & ~b.data[i] ) != 0ul ) {
      return false;
    }
  }
  return true;
}

void Bitset::set( int start, int end )
{
  hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * UNIT_BITSIZE ) );

  int   startUnit   = start / UNIT_BITSIZE;
  int   startOffset = start % UNIT_BITSIZE;

  int   endUnit     = end / UNIT_BITSIZE;
  int   endOffset   = end % UNIT_BITSIZE;

  ulong startMask   = ~0ul << startOffset;
  ulong endMask     = ~( ~0ul << endOffset );

  if( startUnit == endUnit ) {
    data[startUnit] |= startMask & endMask;
  }
  else {
    data[startUnit] |= startMask;
    data[endUnit]   |= endMask;

    for( int i = startUnit + 1; i < endUnit; ++i ) {
      data[i] = ~0ul;
    }
  }
}

void Bitset::clear( int start, int end )
{
  hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * UNIT_BITSIZE ) );

  int   startUnit   = start / UNIT_BITSIZE;
  int   startOffset = start % UNIT_BITSIZE;

  int   endUnit     = end / UNIT_BITSIZE;
  int   endOffset   = end % UNIT_BITSIZE;

  ulong startMask   = ~( ~0ul << startOffset );
  ulong endMask     = ~0ul << endOffset;

  if( startUnit == endUnit ) {
    data[startUnit] &= startMask | endMask;
  }
  else {
    data[startUnit] &= startMask;
    data[endUnit]   &= endMask;

    for( int i = startUnit + 1; i < endUnit; ++i ) {
      data[i] = 0ul;
    }
  }
}

void Bitset::setAll()
{
  aFill<ulong, ulong>( data, size * UNIT_SIZE, ~0ul );
}

void Bitset::clearAll()
{
  aFill<ulong, ulong>( data, size * UNIT_SIZE, 0ul );
}

Bitset Bitset::operator ~ () const
{
  Bitset r( size );

  for( int i = 0; i < size; ++i ) {
    r.data[i] = ~data[i];
  }
  return r;
}

Bitset Bitset::operator & ( const Bitset& b ) const
{
  hard_assert( size == b.size );

  Bitset r( size );

  for( int i = 0; i < size; ++i ) {
    r.data[i] = data[i] & b.data[i];
  }
  return r;
}

Bitset Bitset::operator | ( const Bitset& b ) const
{
  hard_assert( size == b.size );

  Bitset r( size );

  for( int i = 0; i < size; ++i ) {
    r.data[i] = data[i] | b.data[i];
  }
  return r;
}

Bitset Bitset::operator ^ ( const Bitset& b ) const
{
  hard_assert( size == b.size );

  Bitset r( size );

  for( int i = 0; i < size; ++i ) {
    r.data[i] = data[i] ^ b.data[i];
  }
  return r;
}

Bitset& Bitset::operator &= ( const Bitset& b )
{
  for( int i = 0; i < size; ++i ) {
    data[i] &= b.data[i];
  }
  return *this;
}

Bitset& Bitset::operator |= ( const Bitset& b )
{
  for( int i = 0; i < size; ++i ) {
    data[i] |= b.data[i];
  }
  return *this;
}

Bitset& Bitset::operator ^= ( const Bitset& b )
{
  for( int i = 0; i < size; ++i ) {
    data[i] ^= b.data[i];
  }
  return *this;
}

void Bitset::resize( int nBits )
{
  hard_assert( nBits >= 0 );

  int nUnits = ( nBits + UNIT_BITSIZE - 1 ) / UNIT_BITSIZE;

  if( nUnits != size ) {
    data = aReallocate<ulong>( data, size, nUnits );
    size = nUnits;
  }
}

}
