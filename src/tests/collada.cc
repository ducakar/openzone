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
 * @file tests/collada.cc
 */

#include <ozCore/ozCore.hh>

#include <dae.h>
#include <1.4/dom/domCOLLADA.h>

using namespace oz;
using namespace ColladaDOM141;

int main()
{
  System::init();

  File file( File::NATIVE, "/home/davorin/untitled.dae" );

  DAE dae;
  domElement* root = dae.open( file.path().cstr() );
  hard_assert( root != nullptr );
  domCOLLADA* dom = static_cast<domCOLLADA*>( dae.getDom( file.name() ) );
  hard_assert( dom != nullptr );

  auto geometries = dom->getLibrary_geometries_array();
  for( size_t i = 0; i < geometries.getCount(); ++i ) {
    Log() << "Geometry #" << i << ": " << geometries[i]->getName() << "\n";

    auto meshes = geometries[i]->getGeometry_array();
    for( size_t j = 0; j < meshes.getCount(); ++j ) {
      Log() << "+ Mesh #" << j << ": " << meshes[j]->getName() << "\n";
    }
  }
  return 0;
}
