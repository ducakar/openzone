/*
 * img2dds - DDS image builder.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <ozFactory/ozFactory.hh>

#include <cstdlib>
#include <getopt.h>

using namespace oz;

static void usage()
{
  Log::printRaw(
    "Usage: ozDDS [-C] [-M] <inputImage> [outputDir]\n"
    "\t-C\tUse S3 texture compression\n"
    "\t-M\tGenerate mipmaps\n"
  );
}

int main( int argc, char** argv )
{
  System::init();

  int ddsOptions = 0;

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
  if( nArgs < 1 || nArgs > 2 ) {
    usage();
    return EXIT_FAILURE;
  }

  const char* destPath = nArgs == 1 ? "." : argv[optind + 1];

  if( !ImageBuilder::convertToDDS( argv[optind], ddsOptions, destPath ) ) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
