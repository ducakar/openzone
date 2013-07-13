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
#include <ozEngine/ozEngine.hh>
#include <ozFactory/ozFactory.hh>

using namespace oz;

int main()
{
  System::init();

  TerraBuilder::setBounds( TerraBuilder::NOISE, -0.75f, +0.75f );
  TerraBuilder::setOctaveCount( TerraBuilder::NOISE, 2 );
  TerraBuilder::setFrequency( TerraBuilder::NOISE, 0.04f );

  char** images = TerraBuilder::generateCubeNoise( 128 );

  ImageBuilder::createDDS( images[0], 128, 128, 24, 0, "x-.dds" );
  ImageBuilder::createDDS( images[1], 128, 128, 24, 0, "x+.dds" );
  ImageBuilder::createDDS( images[2], 128, 128, 24, 0, "y-.dds" );
  ImageBuilder::createDDS( images[3], 128, 128, 24, 0, "y+.dds" );
  ImageBuilder::createDDS( images[4], 128, 128, 24, 0, "z-.dds" );
  ImageBuilder::createDDS( images[5], 128, 128, 24, 0, "z+.dds" );

  delete[] images[0];
  delete[] images[1];
  delete[] images[2];
  delete[] images[3];
  delete[] images[4];
  delete[] images[5];
  delete[] images;
  return 0;
}
