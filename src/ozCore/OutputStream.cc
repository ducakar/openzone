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

#include "System.hh"

namespace oz
{

OutputStream::OutputStream( const OutputStream& os ) :
  InputStream( os.streamPos, os.streamBegin, os.streamEnd, os.order ), buffered( os.buffered )
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

OutputStream::OutputStream( OutputStream&& os ) :
  InputStream( os.streamPos, os.streamBegin, os.streamEnd, os.order ), buffered( os.buffered )
{
  if( os.buffered ) {
    os.streamPos   = nullptr;
    os.streamBegin = nullptr;
    os.streamEnd   = nullptr;
    os.order       = Endian::NATIVE;
    os.buffered    = false;
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

OutputStream& OutputStream::operator = ( OutputStream&& os )
{
  if( &os == this ) {
    return *this;
  }

  if( buffered ) {
    delete[] streamBegin;
  }

  streamPos   = os.streamPos;
  streamBegin = os.streamBegin;
  streamEnd   = os.streamEnd;
  order       = os.order;
  buffered    = os.buffered;

  if( os.buffered ) {
    os.streamPos   = nullptr;
    os.streamBegin = nullptr;
    os.streamEnd   = nullptr;
    os.order       = Endian::NATIVE;
    os.buffered    = false;
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

      streamBegin = aReallocate<char>( streamBegin, size, newSize );
      streamEnd   = streamBegin + newSize;
      streamPos   = streamBegin + length;
      oldPos      = streamPos - count;
    }
    else {
      OZ_ERROR( "oz::OutputStream: Overrun for %d B during a read or write of %d B",
                int( streamPos - streamEnd ), count );
    }
  }

  return oldPos;
}

void OutputStream::writeBool( bool b )
{
  char* data = forward( int( sizeof( bool ) ) );
  *data = char( b );
}

void OutputStream::writeChar( char c )
{
  char* data = forward( int( sizeof( char ) ) );
  *data = char( c );
}

void OutputStream::writeChars( const char* array, int count )
{
  char* data = forward( count * int( sizeof( char ) ) );
  mCopy( data, array, size_t( count ) );
}

void OutputStream::writeByte( byte b )
{
  char* data = forward( int( sizeof( byte ) ) );
  *data = char( b );
}

void OutputStream::writeUByte( ubyte b )
{
  char* data = forward( int( sizeof( ubyte ) ) );
  *data = char( b );
}

void OutputStream::writeShort( short s )
{
  char* data = forward( int( sizeof( short ) ) );

  Endian::ShortToBytes value = { s };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
  }
  else {
    data[0] = value.data[1];
    data[1] = value.data[0];
  }
}

void OutputStream::writeUShort( ushort s )
{
  char* data = forward( int( sizeof( ushort ) ) );

  Endian::UShortToBytes value = { s };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
  }
  else {
    data[0] = value.data[1];
    data[1] = value.data[0];
  }
}

void OutputStream::writeInt( int i )
{
  char* data = forward( int( sizeof( int ) ) );

  Endian::IntToBytes value = { i };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
  }
  else {
    data[0] = value.data[3];
    data[1] = value.data[2];
    data[2] = value.data[1];
    data[3] = value.data[0];
  }
}

void OutputStream::writeUInt( uint i )
{
  char* data = forward( int( sizeof( uint ) ) );

  Endian::UIntToBytes value = { i };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
  }
  else {
    data[0] = value.data[3];
    data[1] = value.data[2];
    data[2] = value.data[1];
    data[3] = value.data[0];
  }
}

void OutputStream::writeLong64( long64 l )
{
  char* data = forward( int( sizeof( long64 ) ) );

  Endian::Long64ToBytes value = { l };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
  }
  else {
    data[0] = value.data[7];
    data[1] = value.data[6];
    data[2] = value.data[5];
    data[3] = value.data[4];
    data[4] = value.data[3];
    data[5] = value.data[2];
    data[6] = value.data[1];
    data[7] = value.data[0];
  }
}

void OutputStream::writeULong64( ulong64 l )
{
  char* data = forward( int( sizeof( ulong64 ) ) );

  Endian::ULong64ToBytes value = { l };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
  }
  else {
    data[0] = value.data[7];
    data[1] = value.data[6];
    data[2] = value.data[5];
    data[3] = value.data[4];
    data[4] = value.data[3];
    data[5] = value.data[2];
    data[6] = value.data[1];
    data[7] = value.data[0];
  }
}

void OutputStream::writeFloat( float f )
{
  char* data = forward( int( sizeof( float ) ) );

  Endian::FloatToBytes value = { f };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
  }
  else {
    data[0] = value.data[3];
    data[1] = value.data[2];
    data[2] = value.data[1];
    data[3] = value.data[0];
  }
}

void OutputStream::writeDouble( double d )
{
  char* data = forward( int( sizeof( double ) ) );

  Endian::DoubleToBytes value = { d };

  if( order == Endian::NATIVE ) {
    data[0] = value.data[0];
    data[1] = value.data[1];
    data[2] = value.data[2];
    data[3] = value.data[3];
    data[4] = value.data[4];
    data[5] = value.data[5];
    data[6] = value.data[6];
    data[7] = value.data[7];
  }
  else {
    data[0] = value.data[7];
    data[1] = value.data[6];
    data[2] = value.data[5];
    data[3] = value.data[4];
    data[4] = value.data[3];
    data[5] = value.data[2];
    data[6] = value.data[1];
    data[7] = value.data[0];
  }
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

void OutputStream::writeVec3( const Vec3& v )
{
  char* data = forward( int( sizeof( float[3] ) ) );

  Endian::FloatToBytes x = { v.x };
  Endian::FloatToBytes y = { v.y };
  Endian::FloatToBytes z = { v.z };

  if( order == Endian::NATIVE ) {
    data[ 0] = x.data[0];
    data[ 1] = x.data[1];
    data[ 2] = x.data[2];
    data[ 3] = x.data[3];
    data[ 4] = y.data[0];
    data[ 5] = y.data[1];
    data[ 6] = y.data[2];
    data[ 7] = y.data[3];
    data[ 8] = z.data[0];
    data[ 9] = z.data[1];
    data[10] = z.data[2];
    data[11] = z.data[3];
  }
  else {
    data[ 0] = x.data[3];
    data[ 1] = x.data[2];
    data[ 2] = x.data[1];
    data[ 3] = x.data[0];
    data[ 4] = y.data[3];
    data[ 5] = y.data[2];
    data[ 6] = y.data[1];
    data[ 7] = y.data[0];
    data[ 8] = z.data[3];
    data[ 9] = z.data[2];
    data[10] = z.data[1];
    data[11] = z.data[0];
  }
}

void OutputStream::writeVec4( const Vec4& v )
{
  char* data = forward( int( sizeof( float[4] ) ) );

  Endian::FloatToBytes x = { v.x };
  Endian::FloatToBytes y = { v.y };
  Endian::FloatToBytes z = { v.z };
  Endian::FloatToBytes w = { v.w };

  if( order == Endian::NATIVE ) {
    data[ 0] = x.data[0];
    data[ 1] = x.data[1];
    data[ 2] = x.data[2];
    data[ 3] = x.data[3];
    data[ 4] = y.data[0];
    data[ 5] = y.data[1];
    data[ 6] = y.data[2];
    data[ 7] = y.data[3];
    data[ 8] = z.data[0];
    data[ 9] = z.data[1];
    data[10] = z.data[2];
    data[11] = z.data[3];
    data[12] = w.data[0];
    data[13] = w.data[1];
    data[14] = w.data[2];
    data[15] = w.data[3];
  }
  else {
    data[ 0] = x.data[3];
    data[ 1] = x.data[2];
    data[ 2] = x.data[1];
    data[ 3] = x.data[0];
    data[ 4] = y.data[3];
    data[ 5] = y.data[2];
    data[ 6] = y.data[1];
    data[ 7] = y.data[0];
    data[ 8] = z.data[3];
    data[ 9] = z.data[2];
    data[10] = z.data[1];
    data[11] = z.data[0];
    data[12] = w.data[3];
    data[13] = w.data[2];
    data[14] = w.data[1];
    data[15] = w.data[0];
  }
}

void OutputStream::writePoint( const Point& p )
{
  char* data = forward( int( sizeof( float[3] ) ) );

  Endian::FloatToBytes x = { p.x };
  Endian::FloatToBytes y = { p.y };
  Endian::FloatToBytes z = { p.z };

  if( order == Endian::NATIVE ) {
    data[ 0] = x.data[0];
    data[ 1] = x.data[1];
    data[ 2] = x.data[2];
    data[ 3] = x.data[3];
    data[ 4] = y.data[0];
    data[ 5] = y.data[1];
    data[ 6] = y.data[2];
    data[ 7] = y.data[3];
    data[ 8] = z.data[0];
    data[ 9] = z.data[1];
    data[10] = z.data[2];
    data[11] = z.data[3];
  }
  else {
    data[ 0] = x.data[3];
    data[ 1] = x.data[2];
    data[ 2] = x.data[1];
    data[ 3] = x.data[0];
    data[ 4] = y.data[3];
    data[ 5] = y.data[2];
    data[ 6] = y.data[1];
    data[ 7] = y.data[0];
    data[ 8] = z.data[3];
    data[ 9] = z.data[2];
    data[10] = z.data[1];
    data[11] = z.data[0];
  }
}

void OutputStream::writePlane( const Plane& p )
{
  char* data = forward( int( sizeof( float[4] ) ) );

  Endian::FloatToBytes nx = { p.n.x };
  Endian::FloatToBytes ny = { p.n.y };
  Endian::FloatToBytes nz = { p.n.z };
  Endian::FloatToBytes d  = { p.d };

  if( order == Endian::NATIVE ) {
    data[ 0] = nx.data[0];
    data[ 1] = nx.data[1];
    data[ 2] = nx.data[2];
    data[ 3] = nx.data[3];
    data[ 4] = ny.data[0];
    data[ 5] = ny.data[1];
    data[ 6] = ny.data[2];
    data[ 7] = ny.data[3];
    data[ 8] = nz.data[0];
    data[ 9] = nz.data[1];
    data[10] = nz.data[2];
    data[11] = nz.data[3];
    data[12] = d.data[0];
    data[13] = d.data[1];
    data[14] = d.data[2];
    data[15] = d.data[3];
  }
  else {
    data[ 0] = nx.data[3];
    data[ 1] = nx.data[2];
    data[ 2] = nx.data[1];
    data[ 3] = nx.data[0];
    data[ 4] = ny.data[3];
    data[ 5] = ny.data[2];
    data[ 6] = ny.data[1];
    data[ 7] = ny.data[0];
    data[ 8] = nz.data[3];
    data[ 9] = nz.data[2];
    data[10] = nz.data[1];
    data[11] = nz.data[0];
    data[12] = d.data[3];
    data[13] = d.data[2];
    data[14] = d.data[1];
    data[15] = d.data[0];
  }
}

void OutputStream::writeQuat( const Quat& q )
{
  char* data = forward( int( sizeof( float[4] ) ) );

  Endian::FloatToBytes x = { q.x };
  Endian::FloatToBytes y = { q.y };
  Endian::FloatToBytes z = { q.z };
  Endian::FloatToBytes w = { q.w };

  if( order == Endian::NATIVE ) {
    data[ 0] = x.data[0];
    data[ 1] = x.data[1];
    data[ 2] = x.data[2];
    data[ 3] = x.data[3];
    data[ 4] = y.data[0];
    data[ 5] = y.data[1];
    data[ 6] = y.data[2];
    data[ 7] = y.data[3];
    data[ 8] = z.data[0];
    data[ 9] = z.data[1];
    data[10] = z.data[2];
    data[11] = z.data[3];
    data[12] = w.data[0];
    data[13] = w.data[1];
    data[14] = w.data[2];
    data[15] = w.data[3];
  }
  else {
    data[ 0] = x.data[3];
    data[ 1] = x.data[2];
    data[ 2] = x.data[1];
    data[ 3] = x.data[0];
    data[ 4] = y.data[3];
    data[ 5] = y.data[2];
    data[ 6] = y.data[1];
    data[ 7] = y.data[0];
    data[ 8] = z.data[3];
    data[ 9] = z.data[2];
    data[10] = z.data[1];
    data[11] = z.data[0];
    data[12] = w.data[3];
    data[13] = w.data[2];
    data[14] = w.data[1];
    data[15] = w.data[0];
  }
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

void OutputStream::deallocate()
{
  if( buffered ) {
    delete[] streamBegin;

    streamPos   = nullptr;
    streamBegin = nullptr;
    streamEnd   = nullptr;
  }
}

}
