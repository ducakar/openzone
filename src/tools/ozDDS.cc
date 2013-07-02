/*
 * img2dds - DDS image builder.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <cstdio>
#include <cstdlib>
#include <getopt.h>

using namespace oz;

static void usage()
{
  Log::printRaw(
    "Usage: img2dds [-c] [-m] [-q] <inputImage> <outputDDS>\n"
    "\t-c\tUse S3 texture compression\n"
    "\t-m\tGenerate mipmaps\n"
  );
}

int main( int argc, char** argv )
{
  System::init();

  int ddsOptions = 0;

  int opt;
  while( ( opt = getopt( argc, argv, "cmq" ) ) >= 0 ) {
    switch( opt ) {
      case 'c': {
        ddsOptions |= ImageBuilder::COMPRESSION_BIT;
        break;
      }
      case 'm': {
        ddsOptions |= ImageBuilder::MIPMAPS_BIT;
        break;
      }
      default: {
        usage();
        return EXIT_FAILURE;
      }
    }
  }

  if( argc - optind != 2 ) {
    usage();
    return EXIT_FAILURE;
  }

  OutputStream ostream( 0 );
  if( !ImageBuilder::buildDDS( argv[optind], ddsOptions, &ostream ) ) {
    return EXIT_FAILURE;
  }

  if( !File( argv[optind + 1] ).write( ostream.begin(), ostream.tell() ) ) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
