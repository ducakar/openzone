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

static void printUsage()
{
  Log::printRaw(
    "Usage: ozDDS [options] <inputImage> [<outputDirOrFile>]\n"
    "  -c  Use S3 texture compression (DXT1 or DXT5 if the image has transparent pixels)\n"
    "  -h  Flip horizontally\n"
    "  -m  Generate mipmaps\n"
    "  -n  Set normal map flag (DDPF_NORMAL)\n"
    "  -N  Set normal map flag if the image looks like a RGB = XYZ normal map\n"
    "      disable -n, -s and -S options otherwise\n"
    "  -s  Do RGB -> GGGR swizzle (for DXT5nm)\n"
    "  -S  Do RGB -> BGBR swizzle (for DXT5nm+z)\n"
    "  -v  Flip vertically\n\n");
}

int main(int argc, char** argv)
{
  System::init();

  bool detectNormals = false;

  int opt;
  while ((opt = getopt(argc, argv, "chmnNsSv")) >= 0) {
    switch (opt) {
      case 'c': {
        ImageBuilder::options |= ImageBuilder::COMPRESSION_BIT;
        break;
      }
      case 'h': {
        ImageBuilder::options |= ImageBuilder::FLOP_BIT;
        break;
      }
      case 'm': {
        ImageBuilder::options |= ImageBuilder::MIPMAPS_BIT;
        break;
      }
      case 'n': {
        ImageBuilder::options |= ImageBuilder::NORMAL_MAP_BIT;
        break;
      }
      case 'N': {
        detectNormals = true;
        break;
      }
      case 's': {
        ImageBuilder::options |= ImageBuilder::YYYX_BIT;
        break;
      }
      case 'S': {
        ImageBuilder::options |= ImageBuilder::ZYZX_BIT;
        break;
      }
      case 'v': {
        ImageBuilder::options |= ImageBuilder::FLIP_BIT;
        break;
      }
      default: {
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  int nArgs = argc - optind;
  if (nArgs < 1 || nArgs > 2) {
    printUsage();
    return EXIT_FAILURE;
  }

  ImageData image = ImageBuilder::loadImage(argv[optind]);

  if (image.isEmpty()) {
    Log::println("Failed to open image '%s'.", argv[optind]);
    return EXIT_FAILURE;
  }

  if (detectNormals) {
    if (image.isNormalMap()) {
      ImageBuilder::options |= ImageBuilder::NORMAL_MAP_BIT;
    }
    else {
      ImageBuilder::options &= ~ImageBuilder::NORMAL_MAP_BIT;
      ImageBuilder::options &= ~(ImageBuilder::YYYX_BIT | ImageBuilder::ZYZX_BIT);
    }
  }

  const char* destPath = nArgs == 1 ? "." : argv[optind + 1];

  if (!ImageBuilder::convertToDDS(argv[optind], destPath)) {
    Log::println("%s", ImageBuilder::getError());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
