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
 * @file ozCore/OutputStream.cc
 */

#include "OutputStream.hh"

namespace oz
{

OutputStream::OutputStream( const OutputStream& os ) :
  streamPos( os.streamPos ), streamBegin( os.streamBegin ), streamEnd( os.streamEnd ),
  order( os.order ), buffered( os.buffered )
{
  if( os.buffered ) {
    int length = int( os.streamPos - os.streamBegin );
    int size   = int( os.streamEnd - os.streamBegin );

    streamBegin = size == 0 ? nullptr : new char[size];
    streamEnd   = streamBegin + size;
    streamPos   = streamBegin + length;

    mCopy( streamBegin, os.streamBegin, size_t( size ) );
  }
}

OutputStream& OutputStream::operator = ( const OutputStream& os )
{
  if( &os == this ) {
    return *this;
  }

  if( os.buffered ) {
    int  length      = int( os.streamPos - os.streamBegin );
    int  size        = int( os.streamEnd - os.streamBegin );
    bool sizeMatches = int( streamEnd - streamBegin ) == size;

    if( buffered && !sizeMatches ) {
      delete[] streamBegin;
    }
    if( !buffered || !sizeMatches ) {
      streamBegin = new char[size];
      streamEnd   = streamBegin + size;
    }

    streamPos = streamBegin + length;
    order     = os.order;
    buffered  = os.buffered;

    mCopy( streamBegin, os.streamBegin, size_t( size ) );
  }
  else {
    if( buffered ) {
      delete[] streamBegin;
    }

    streamPos   = os.streamPos;
    streamBegin = os.streamBegin;
    streamEnd   = os.streamEnd;
    order       = os.order;
    buffered    = os.buffered;
  }

  return *this;
}

char* OutputStream::forward( int count )
{
  char* oldPos = streamPos;
  streamPos += count;

  if( streamPos > streamEnd ) {
    if( buffered ) {
      int length  = int( streamPos - streamBegin );
      int size    = int( streamEnd - streamBegin );
      int reqSize = length + count;
      int newSize = size == 0 ? GRANULARITY : 2 * size;

      if( newSize < 0 || reqSize < 0 ) {
        OZ_ERROR( "oz::OutputStream: Capacity overflow" );
      }
      else if( newSize < reqSize ) {
        newSize = ( reqSize + GRANULARITY - 1 ) & ~( GRANULARITY - 1 );
      }

      streamBegin  = aReallocate<char>( streamBegin, size, newSize );
      streamEnd    = streamBegin + newSize;
      streamPos    = streamBegin + length;
      oldPos = streamPos - count;
    }
    else {
      OZ_ERROR( "oz::OutputStream: Overrun for %d B during a read or write of %d B",
                int( streamPos - streamEnd ), count );
    }
  }

  return oldPos;
}

void OutputStream::readChars( char* array, int count )
{
  const char* data = forward( count * int( sizeof( char ) ) );
  mCopy( array, data, size_t( count ) );
}

void OutputStream::writeChars( const char* array, int count )
{
  char* data = forward( count * int( sizeof( char ) ) );
  mCopy( data, array, size_t( count ) );
}

const char* OutputStream::readString()
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

void OutputStream::writeString( const String& s )
{
  int   size = s.length() + 1;
  char* data = forward( size );

  mCopy( data, s.cstr(), size_t( size ) );
}

void OutputStream::writeString( const char* s )
{
  int   size = String::length( s ) + 1;
  char* data = forward( size );

  mCopy( data, s, size_t( size ) );
}

Mat3 OutputStream::readMat3()
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

void OutputStream::writeMat3( const Mat3& m )
{
  char* data = forward( int( sizeof( float[9] ) ) );
  const float* values = m;

  if( order == Endian::NATIVE ) {
    for( int i = 0; i < 9; ++i, data += 4, ++values ) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[0];
      data[1] = value.data[1];
      data[2] = value.data[2];
      data[3] = value.data[3];
    }
  }
  else {
    for( int i = 0; i < 9; ++i, data += 4, ++values ) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[3];
      data[1] = value.data[2];
      data[2] = value.data[1];
      data[3] = value.data[0];
    }
  }
}

Mat4 OutputStream::readMat4()
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

void OutputStream::writeMat4( const Mat4& m )
{
  char* data = forward( int( sizeof( float[16] ) ) );
  const float* values = m;

  if( order == Endian::NATIVE ) {
    for( int i = 0; i < 16; ++i, data += 4, ++values ) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[0];
      data[1] = value.data[1];
      data[2] = value.data[2];
      data[3] = value.data[3];
    }
  }
  else {
    for( int i = 0; i < 16; ++i, data += 4, ++values ) {
      Endian::FloatToBytes value = { *values };

      data[0] = value.data[3];
      data[1] = value.data[2];
      data[2] = value.data[1];
      data[3] = value.data[0];
    }
  }
}

void OutputStream::readBitset( ulong* bitset, int nBits )
{
  int unitBits    = int( sizeof( ulong ) ) * 8;
  int unit64Bits  = int( sizeof( ulong64 ) ) * 8;
  int unitCount   = ( nBits + unitBits - 1 ) / unitBits;
  int unit64Count = ( nBits + unit64Bits - 1 ) / unit64Bits;

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

void OutputStream::writeBitset( const ulong* bitset, int nBits )
{
  int unitBits    = int( sizeof( ulong ) ) * 8;
  int unit64Bits  = int( sizeof( ulong64 ) ) * 8;
  int unitCount   = ( nBits + unitBits - 1 ) / unitBits;
  int unit64Count = ( nBits + unit64Bits - 1 ) / unit64Bits;

  char* data = forward( unit64Count * 8 );

  for( int i = 0; i < unitCount; ++i ) {
#if OZ_SIZEOF_LONG == 4
    Endian::UIntToBytes value = { *reinterpret_cast<const ulong*>( bitset ) };

    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
#else
    Endian::ULong64ToBytes value = { *reinterpret_cast<const ulong*>( bitset ) };

    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
#endif

    bitset += 1;
    data   += sizeof( ulong );
  }

#if OZ_SIZEOF_LONG == 4
  if( unit64Count * 2 != unitCount ) {
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
  }
#endif
}

String OutputStream::readLine()
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

void OutputStream::writeLine( const String& s )
{
  int   length = s.length();
  char* data   = forward( length + 1 );

  mCopy( data, s, size_t( length ) );
  data[length] = '\n';
}

void OutputStream::writeLine( const char* s )
{
  int   length = String::length( s );
  char* data   = forward( length + 1 );

  mCopy( data, s, size_t( length ) );
  data[length] = '\n';
}

}
