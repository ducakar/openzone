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
 * A `env.dds` file is generated in the current directory. It should be copied to `oz_base/glsl`.
 */

#include <ozFactory/ozFactory.hh>

#include <cstdlib>
#include <getopt.h>

using namespace oz;

static void usage()
{
  Log::printRaw(
    "Usage: ozGenEnvMap [-C] [-M] [size]\n"
    "\tsize\tTexture size, 32 by default"
    "\t-C\tUse S3 texture compression\n"
    "\t-M\tGenerate mipmaps\n"
  );
}

int main( int argc, char** argv )
{
  System::init();

  int ddsOptions = 0;
  int size       = 32;

  int opt;
  while( ( opt = getopt( argc, argv, "CM" ) ) >= 0 ) {
    switch( opt ) {
      case 'C': {
        ddsOptions |= ImageBuilder::COMPRESSION_BIT;
        break;
      }
      case 'M': {
        ddsOptions |= ImageBuilder::MIPMAPS_BIT;
        break;
      }
      default: {
        usage();
        return EXIT_FAILURE;
      }
    }
  }

  int nArgs = argc - optind;
  if( nArgs > 1 ) {
    usage();
    return EXIT_FAILURE;
  }
  else if( nArgs == 1 ) {
    const char* end;

    size = String::parseInt( argv[optind], &end );

    if( *end != '\0' ) {
      usage();
      return EXIT_FAILURE;
    }
  }

  int exitCode = EXIT_SUCCESS;

  TerraBuilder::setBounds( TerraBuilder::NOISE, -1.75f, +1.75f );
  TerraBuilder::setOctaveCount( TerraBuilder::NOISE, 1 );
  TerraBuilder::setFrequency( TerraBuilder::NOISE, 2.0f );

  char** images = TerraBuilder::generateCubeNoise( size );

  if( !ImageBuilder::createDDS( images, 6, size, size, 24, ddsOptions, "env.dds" ) ) {
    Log::println( "Failed to generate maps: %s", ImageBuilder::getError() );
    exitCode = EXIT_FAILURE;
  }

  delete[] images[0];
  delete[] images[1];
  delete[] images[2];
  delete[] images[3];
  delete[] images[4];
  delete[] images[5];
  delete[] images;

  return exitCode;
}
