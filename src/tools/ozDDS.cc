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
    "Usage: ozDDS [-v] [-m] [-c | -n | -N] <inputImage> [outputDirOrFile]\n"
    "\t-v\tFlip vertically\n"
    "\t-m\tGenerate mipmaps\n"
    "\t-c\tUse S3 texture compression (DXT1 or DXT5 if the image has transparent pixels)\n"
    "\t-n\tDo RGB -> GGGR swizzle (for DXT5nm)\n"
    "\t-N\tDo RGB -> BGBR swizzle (for DXT5nm+z)\n"
  );
}

int main(int argc, char** argv)
{
  System::init();

  int ddsOptions = 0;

  int opt;
  while ((opt = getopt(argc, argv, "vmcnN")) >= 0) {
    switch (opt) {
      case 'v': {
        ddsOptions |= ImageBuilder::FLIP_BIT;
        break;
      }
      case 'm': {
        ddsOptions |= ImageBuilder::MIPMAPS_BIT;
        break;
      }
      case 'c': {
        ddsOptions |= ImageBuilder::COMPRESSION_BIT;
        break;
      }
      case 'n': {
        ddsOptions |= ImageBuilder::YYYX_BIT;
        break;
      }
      case 'N': {
        ddsOptions |= ImageBuilder::ZYZX_BIT;
        break;
      }
      default: {
        usage();
        return EXIT_FAILURE;
      }
    }
  }

  int nArgs = argc - optind;
  if (nArgs < 1 || nArgs > 2) {
    usage();
    return EXIT_FAILURE;
  }

  const char* destPath = nArgs == 1 ? "." : argv[optind + 1];

  if (!ImageBuilder::convertToDDS(argv[optind], ddsOptions, destPath)) {
    Log::println("%s", ImageBuilder::getError());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
