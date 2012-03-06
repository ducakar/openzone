/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file tests/test.cc
 */

#include "oz/oz.hh"

// #include <QtGui/QApplication>
// #include <QtGui/QPushButton>

#include <cstdio>

using namespace oz;

int array[] = { 1, 2, 3, 4 };

StackTrace foo()
{
  return { 0, {} };
}

int main( int argc, char** argv )
{
  static_cast<void>( argc );
  static_cast<void>( argv );

  StackTrace st = foo();

  foreach( i, citer( st.frames ) ) {
    printf( "%p :: ", *i );
  }
  printf( "\n" );
  return 0;
}
