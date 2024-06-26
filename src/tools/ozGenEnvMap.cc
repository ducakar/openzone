/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <ozFactory/ozFactory.hh>

#include <cstdlib>
#include <getopt.h>

using namespace oz;

namespace
{

void printUsage()
{
  Log::printRaw(
    "Usage: ozGenEnvMap [-C] [-M] [<size>]\n"
    "  <size>  Texture size, 32 by default\n"
    "  -C      Use S3 texture compression\n"
    "  -M      Generate mipmaps\n\n");
}

}

int main(int argc, char** argv)
{
  System::init();
  ImageBuilder::init();

  ImageBuilder::options = ImageBuilder::CUBE_MAP_BIT;

  int size = 32;

  int opt = 0;
  while ((opt = getopt(argc, argv, "CM")) >= 0) {
    switch (opt) {
      case 'C': {
        ImageBuilder::options |= ImageBuilder::COMPRESSION_BIT;
        break;
      }
      case 'M': {
        ImageBuilder::options |= ImageBuilder::MIPMAPS_BIT;
        break;
      }
      default: {
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  int nArgs = argc - optind;
  if (nArgs > 1) {
    printUsage();
    return EXIT_FAILURE;
  }
  else if (nArgs == 1) {
    const char* end = nullptr;
    size = String::parseInt(argv[optind], &end);

    if (*end != '\0') {
      printUsage();
      return EXIT_FAILURE;
    }
  }

  int exitCode = EXIT_SUCCESS;

  // For TextureReplacer:
  //TerraBuilder::setBounds(TerraBuilder::NOISE, -1.75f, +0.25f);
  //TerraBuilder::setFrequency(TerraBuilder::NOISE, 0.5f);
  TerraBuilder::setSeed(TerraBuilder::NOISE, 8);
  TerraBuilder::setBounds(TerraBuilder::NOISE, -1.75f, +1.25f);
  TerraBuilder::setOctaveCount(TerraBuilder::NOISE, 3);
  TerraBuilder::setFrequency(TerraBuilder::NOISE, 0.75f);

  ImageData* images = TerraBuilder::generateCubeNoise(size);

  if (!ImageBuilder::createDDS(images, 6, "env.dds")) {
    Log::println("Failed to generate maps: %s", ImageBuilder::getError());
    exitCode = EXIT_FAILURE;
  }

  delete[] images;
  ImageBuilder::destroy();

  return exitCode;
}
