/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Buffer.cpp
 */

#include "Buffer.hpp"

namespace oz
{

Buffer::Buffer() : data( null ), size( 0 )
{}

Buffer::~Buffer()
{
  dealloc();
}

Buffer::Buffer( const Buffer& b ) : data( null ), size( b.size )
{
  if( b.size != 0 ) {
    data = new char[size];
    aCopy( data, b.data, size );
  }
}

Buffer::Buffer( Buffer&& b ) : data( b.data ), size( b.size )
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

Buffer::Buffer( int initSize ) : data( new char[initSize] ), size( initSize )
{}

void Buffer::alloc( int initSize )
{
  hard_assert( size == 0 && initSize > 0 );

  data = new char[initSize];
  size = initSize;
}

void Buffer::dealloc()
{
  delete[] data;

  data = null;
  size = 0;
}

InputStream Buffer::inputStream() const
{
  hard_assert( data != null );

  return InputStream( data, data + size );
}

OutputStream Buffer::outputStream() const
{
  hard_assert( data != null );

  return OutputStream( data, data + size );
}

}
