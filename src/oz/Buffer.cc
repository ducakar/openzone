/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/Buffer.cc
 */

#include "Buffer.hh"

#include <cstring>

namespace oz
{

Buffer::Buffer() :
  data( nullptr ), size( 0 )
{}

Buffer::~Buffer()
{
  deallocate();
}

Buffer::Buffer( const Buffer& b ) :
  data( nullptr ), size( b.size )
{
  if( b.size != 0 ) {
    data = new char[size];
    memcpy( data, b.data, size_t( size ) );
  }
}

Buffer::Buffer( Buffer&& b ) :
  data( b.data ), size( b.size )
{
  b.data = nullptr;
  b.size = 0;
}

Buffer& Buffer::operator = ( const Buffer& b )
{
  if( &b == this ) {
    return *this;
  }

  if( size < b.size ) {
    deallocate();
    allocate( b.size );
  }

  memcpy( data, b.data, size_t( b.size ) );
  return *this;
}

Buffer& Buffer::operator = ( Buffer&& b )
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

Buffer::Buffer( int size_ ) :
  data( new char[size_] ), size( size_ )
{}

void Buffer::allocate( int size_ )
{
  hard_assert( size == 0 && size_ > 0 );

  data = new char[size_];
  size = size_;
}

void Buffer::deallocate()
{
  delete[] data;

  data = nullptr;
  size = 0;
}

InputStream Buffer::inputStream( Endian::Order order ) const
{
  hard_assert( data != nullptr );

  return InputStream( data, data + size, order );
}

OutputStream Buffer::outputStream( Endian::Order order ) const
{
  hard_assert( data != nullptr );

  return OutputStream( data, data + size, order );
}

}
