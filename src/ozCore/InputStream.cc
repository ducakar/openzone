/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/InputStream.cc
 */

#include "InputStream.hh"

namespace oz
{

void InputStream::readChars( char* array, int count )
{
  const char* data = forward( count * int( sizeof( char ) ) );
  mCopy( array, data, size_t( count ) );
}

const char* InputStream::readString()
{
  const char* begin = streamPos;

  while( streamPos < streamEnd && *streamPos != '\0' ) {
    ++streamPos;
  }
  if( streamPos == streamEnd ) {
    OZ_ERROR( "oz::OutputStream: Buffer overrun while looking for the end of a string." );
  }

  ++streamPos;
  return begin;
}

Mat3 InputStream::readMat3()
{
  const char* data = forward( int( sizeof( float[9] ) ) );

  Mat3 m;
  float* values = m;

  if( order == Endian::NATIVE ) {
    for( int i = 0; i < 9; ++i, data += 4, ++values ) {
      Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for( int i = 0; i < 9; ++i, data += 4, ++values ) {
      Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }

  return m;
}

Mat4 InputStream::readMat4()
{
  const char* data = forward( int( sizeof( float[16] ) ) );

  Mat4 m;
  float* values = m;

  if( order == Endian::NATIVE ) {
    for( int i = 0; i < 16; ++i, data += 4, ++values ) {
      Endian::BytesToFloat value = { { data[0], data[1], data[2], data[3] } };

      *values = value.value;
    }
  }
  else {
    for( int i = 0; i < 16; ++i, data += 4, ++values ) {
      Endian::BytesToFloat value = { { data[3], data[2], data[1], data[0] } };

      *values = value.value;
    }
  }

  return m;
}

void InputStream::readBitset( ulong* bitset, int bits )
{
  int unitBits    = int( sizeof( ulong ) ) * 8;
  int unit64Bits  = int( sizeof( ulong64 ) ) * 8;
  int unitCount   = ( bits + unitBits - 1 ) / unitBits;
  int unit64Count = ( bits + unit64Bits - 1 ) / unit64Bits;

  const char* data = forward( unit64Count * 8 );

  for( int i = 0; i < unitCount; ++i ) {
#if OZ_SIZEOF_LONG == 4
    Endian::BytesToUInt value = { { data[0], data[1], data[2], data[3] } };
#else
    Endian::BytesToULong64 value = {
      { data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] }
    };
#endif
    *bitset = ulong( value.value );

    bitset += 1;
    data   += sizeof( ulong );
  }
}

String InputStream::readLine()
{
  const char* begin = streamPos;

  while( streamPos < streamEnd && *streamPos != '\n' && *streamPos != '\r' ) {
    ++streamPos;
  }

  int length = int( streamPos - begin );

  streamPos += ( streamPos < streamEnd ) +
               ( streamPos < streamEnd - 1 && streamPos[0] == '\r' && streamPos[1] == '\n' );
  return String( begin, length );
}

}
