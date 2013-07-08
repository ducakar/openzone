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
#include <ozFactory/ozFactory.hh>
#include <algorithm>

using namespace oz;

int main()
{
  System::init();

  File file = "data/oz_main/mdl/tank/data.obj";
  OutputStream os( 0 );

  if( !ModelBuilder::buildModel( file, &os ) ) {
    Log() << ModelBuilder::getError();
  }

  File out = "share/openzone/oz_main/mdl/tank/data.ozcSMM";
  out.write( os.begin(), os.tell() );
  return 0;
}
