/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <matrix/NamePool.hh>

#include <matrix/Liber.hh>

namespace oz
{

static List<String> names;
static List<int>    listPositions;

const String& NamePool::genName( int list ) const
{
  hard_assert( names.length() > 0 );

  if( list < 0 ) {
    return names[0];
  }

  int index = listPositions[list] + Math::rand( listPositions[list + 1] - listPositions[list] );
  return names[index];
}

void NamePool::init()
{
  Log::print( "Initialising NamePool ..." );

  // a default entry, we need it if nothing is read
  names.add( "" );
  listPositions.add( names.length() );

  for( int i = 0; i < liber.nameLists.length(); ++i ) {
    File file = liber.nameLists[i].path;

    if( file.type() != File::REGULAR ) {
      OZ_ERROR( "Reading '%s' failed", liber.nameLists[i].path.cstr() );
    }

    InputStream is = file.inputStream();

    const char* lineBegin = is.pos();

    while( is.isAvailable() ) {
      char ch = is.readChar();

      if( ch == '\n' ) {
        String name( lineBegin, int( is.pos() - lineBegin ) );
        name = name.trim();

        if( !name.isEmpty() && name[0] != '/' && name[0] != '#' ) {
          names.add( name );
        }

        lineBegin = is.pos();
      }
    }

    listPositions.add( names.length() );
  }

  hard_assert( listPositions.length() == liber.nameLists.length() + 1 );

  Log::printEnd( " OK" );
}

void NamePool::destroy()
{
  Log::print( "Destroying NamePool ..." );

  names.clear();
  names.trim();
  listPositions.clear();
  listPositions.trim();

  Log::printEnd( " OK" );
}

NamePool namePool;

}
