/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Buffer.cc
 */

#include "Buffer.hh"

namespace oz
{

Buffer::Buffer() :
  data( null ), size( 0 )
{}

Buffer::~Buffer()
{
  dealloc();
}

Buffer::Buffer( const Buffer& b ) :
  data( null ), size( b.size )
{
  if( b.size != 0 ) {
    data = new char[size];
    aCopy( data, b.data, size );
  }
}

Buffer::Buffer( Buffer&& b ) :
  data( b.data ), size( b.size )
{
  b.data = null;
  b.size = 0;
}

Buffer& Buffer::operator = ( const Buffer& b )
{
  if( &b == this ) {
    return *this;
  }

  if( size < b.size ) {
    dealloc();
    alloc( b.size );
  }

  aCopy( data, b.data, b.size );
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

  b.data = null;
  b.size = 0;

  return *this;
}

Buffer::Buffer( int size_ ) :
  data( new char[size_] ), size( size_ )
{}

void Buffer::alloc( int size_ )
{
  hard_assert( size == 0 && size_ > 0 );

  data = new char[size_];
  size = size_;
}

void Buffer::dealloc()
{
  delete[] data;

  data = null;
  size = 0;
}

InputStream Buffer::inputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return InputStream( data, data + size, order );
}

OutputStream Buffer::outputStream( Endian::Order order ) const
{
  hard_assert( data != null );

  return OutputStream( data, data + size, order );
}

}
