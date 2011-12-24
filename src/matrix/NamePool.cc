/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 */

/**
 * @file matrix/NamePool.cc
 */

#include "stable.hh"

#include "matrix/NamePool.hh"

#include "matrix/Library.hh"

namespace oz
{
namespace matrix
{

NamePool namePool;

char NamePool::buffer[LINE_LENGTH];

const String& NamePool::genName( int list ) const
{
  hard_assert( names.length() > 0 );

  if( list == -1 ) {
    return names[0];
  }

  int index = listPositions[list] + Math::rand( listPositions[list + 1] - listPositions[list] );
  return names[index];
}

void NamePool::init()
{
  log.print( "Loading name databases ..." );

  // a default entry, we need it if nothing is read
  names.add( "" );
  listPositions.add( names.length() );

  for( int i = 0; i < library.nameLists.length(); ++i ) {
    PhysFile file( library.nameLists[i].path );
    if( !file.map() ) {
      throw Exception( "Reading '%s' failed", library.nameLists[i].path.cstr() );
    }

    InputStream is = file.inputStream();

    FILE* fs = fmemopen( const_cast<char*>( is.begin() ), size_t( is.capacity() ), "r" );
    if( fs == null ) {
      throw Exception( "Reading '%s' failed", library.nameLists[i].path.cstr() );
    }

    while( fgets( buffer, LINE_LENGTH, fs ) != null ) {
      String name = buffer;
      name = name.trim();

      if( !name.isEmpty() ) {
        names.add( name );
      }
    }

    fclose( fs );
    file.unmap();

    listPositions.add( names.length() );
  }

  hard_assert( listPositions.length() == library.nameLists.length() + 1 );

  log.printEnd( " OK" );
}

void NamePool::free()
{
  names.clear();
  names.dealloc();
  listPositions.clear();
  listPositions.dealloc();
}

}
}