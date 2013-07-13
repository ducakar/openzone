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
 * @file tools/ozGenEnvMap.cc
 *
 * Generate environment cube map.
 *
 * In the current directory, the following files are generated:
 * @li env-x.dds,
 * @li env+x.dds,
 * @li env-y.dds,
 * @li env+y.dds,
 * @li env-z.dds,
 * @li env+z.dds
 *
 * Those files are to be copied to `oz_base/glsl` directory.
 */

#include <ozCore/ozCore.hh>
#include <ozFactory/ozFactory.hh>

#include <cstdlib>

using namespace oz;

int main()
{
  System::init();

  TerraBuilder::setBounds( TerraBuilder::NOISE, -1.75f, +1.75f );
  TerraBuilder::setOctaveCount( TerraBuilder::NOISE, 1 );
  TerraBuilder::setFrequency( TerraBuilder::NOISE, 0.02f );

  char** images = TerraBuilder::generateCubeNoise( 128 );

  if( !ImageBuilder::createDDS( images[0], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env-x.dds" ) ||
      !ImageBuilder::createDDS( images[1], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env+x.dds" ) ||
      !ImageBuilder::createDDS( images[2], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env-y.dds" ) ||
      !ImageBuilder::createDDS( images[3], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env+y.dds" ) ||
      !ImageBuilder::createDDS( images[4], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env-z.dds" ) ||
      !ImageBuilder::createDDS( images[5], 128, 128, 24, ImageBuilder::MIPMAPS_BIT, "env+z.dds" ) )
  {
    OZ_ERROR( "Failed to generate maps: %s", ImageBuilder::getError() );
  }

  delete[] images[0];
  delete[] images[1];
  delete[] images[2];
  delete[] images[3];
  delete[] images[4];
  delete[] images[5];
  delete[] images;

  return EXIT_SUCCESS;
}
