/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <ozCore/ozCore.hh>
#include <ozFactory/ModelBuilder.hh>

using namespace oz;

int main( int argc, char** argv )
{
  System::init();

  File file = argc == 2 ? argv[1] : "data/oz_main/mdl/tank/data.obj";
  OutputStream os( 0 );

  if( !ModelBuilder::buildModel( file, &os ) ) {
    Log() << ModelBuilder::getError() << "\n";
  }
  return 0;
}
