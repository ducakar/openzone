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

#include <builder/Context.hh>

#include <client/SMMImago.hh>
#include <client/SMMVehicleImago.hh>
#include <client/ExplosionImago.hh>
#include <client/MD2Imago.hh>
#include <client/MD2WeaponImago.hh>
#include <client/BasicAudio.hh>
#include <client/BotAudio.hh>
#include <client/VehicleAudio.hh>

namespace oz::builder
{

namespace
{

static const char* const IMAGE_EXTENSIONS[] = {
  ".dds",
  ".png",
  ".jpeg",
  ".jpg",
  ".bmp",
  ".tga",
  ".tiff"
};

}

bool Context::isBaseTexture(const String& name)
{
  return !name.endsWith("_d") && !name.endsWith("_m") && !name.endsWith("_s") &&
         !name.endsWith("_spec") && !name.endsWith("_g") && !name.endsWith("_glow") &&
         !name.endsWith(".blend") && !name.endsWith("_n") && !name.endsWith("_nm") &&
         !name.endsWith("_normal") && !name.endsWith("_local");
}

void Context::buildTexture(const File& basePath, const File& destBasePath, bool allLayers)
{
  Log::print("Building texture(s) '%s' -> '%s' ...", basePath.c(), destBasePath.c());

  ImageBuilder::options = ImageBuilder::MIPMAPS_BIT;
  ImageBuilder::scale   = 1.0f;

  if (useS3TC) {
    ImageBuilder::options |= ImageBuilder::COMPRESSION_BIT;
  }
  if (useFast) {
    ImageBuilder::options |= ImageBuilder::FAST_BIT;
  }

  const File& diffuseBasePath   = basePath;
  File        masksBasePath     = basePath + "_m";
  File        specular1BasePath = basePath + "_s";
  File        specular2BasePath = basePath + "_spec";
  File        specular3BasePath = basePath + "_h";
  File        emission1BasePath = basePath + "_g";
  File        emission2BasePath = basePath + "_glow";
  File        emission3BasePath = basePath + ".blend";
  File        normals1BasePath  = basePath + "_n";
  File        normals2BasePath  = basePath + "_nm";
  File        normals3BasePath  = basePath + "_normal";
  File        normals4BasePath  = basePath + "_local";

  File diffuse, masks, specular, emission, normals;

  for (const char* extension : IMAGE_EXTENSIONS) {
    if (diffuse.isEmpty() || !diffuse.exists()) {
      diffuse = diffuseBasePath + extension;
    }

    if (allLayers) {
      if (masks.isEmpty() || !masks.exists()) {
        masks = masksBasePath + extension;
      }

      if (specular.isEmpty() || !specular.exists()) {
        specular = specular1BasePath + extension;
      }
      if (!specular.exists()) {
        specular = specular2BasePath + extension;
      }
      if (!specular.exists()) {
        specular = specular3BasePath + extension;
      }

      if (emission.isEmpty() || !emission.exists()) {
        emission = emission1BasePath + extension;
      }
      if (!emission.exists()) {
        emission = emission2BasePath + extension;
      }
      if (!emission.exists()) {
        emission = emission3BasePath + extension;
      }

      if (normals.isEmpty() || !normals.exists()) {
        normals = normals1BasePath + extension;
      }
      if (!normals.exists()) {
        normals = normals2BasePath + extension;
      }
      if (!normals.exists()) {
        normals = normals3BasePath + extension;
      }
      if (!normals.exists()) {
        normals = normals4BasePath + extension;
      }
    }
  }

  if (diffuse.exists()) {
    ImageBuilder::convertToDDS(diffuse, destBasePath + ".dds");
  }
  else {
    OZ_ERROR("Missing texture '%s' (.png, .jpeg, .jpg and .tga checked)", basePath.c());
  }

  if (masks.exists()) {
    ImageBuilder::convertToDDS(masks, destBasePath + "_m.dds");
  }
  else {
    ImageData specularImage;
    ImageData emissionImage;

    if (specular.exists()) {
      specularImage = ImageBuilder::loadImage(specular);
      specularImage.setAlpha(false);
    }
    if (emission.exists()) {
      emissionImage = ImageBuilder::loadImage(emission);
      emissionImage.setAlpha(false);
    }

    if (specularImage.isEmpty() && emissionImage.isEmpty()) {
      // Drop through.
    }
    else if (emissionImage.isEmpty()) {
      for (int i = 0; i < specularImage.width() * specularImage.height(); ++i) {
        char& r = specularImage[i*4 + 0];
        char& g = specularImage[i*4 + 1];
        char& b = specularImage[i*4 + 2];
        char& a = specularImage[i*4 + 3];

        char s = char((r + g + b) / 3);

        r = s;
        g = 0;
        b = 0;
        a = char(0xff);
      }

      ImageBuilder::createDDS(&specularImage, 1, destBasePath + "_m.dds");
    }
    else if (specularImage.isEmpty()) {
      for (int i = 0; i < emissionImage.width() * emissionImage.height(); ++i) {
        char& r = emissionImage[i*4 + 0];
        char& g = emissionImage[i*4 + 1];
        char& b = emissionImage[i*4 + 2];
        char& a = emissionImage[i*4 + 3];

        char e = char((r + g + b) / 3);

        r = 0;
        g = e;
        b = 0;
        a = char(0xff);
      }

      ImageBuilder::createDDS(&emissionImage, 1, destBasePath + "_m.dds");
    }
    else {
      if (specularImage.width() != emissionImage.width() ||
          specularImage.height() != emissionImage.height())
      {
        OZ_ERROR("Specular and emission texture masks must have the same size.");
      }

      for (int i = 0; i < specularImage.width() * specularImage.height(); ++i) {
        char& r = specularImage[i*4 + 0];
        char& g = specularImage[i*4 + 1];
        char& b = specularImage[i*4 + 2];
        char& a = specularImage[i*4 + 3];

        char& er = emissionImage[i*4 + 0];
        char& eg = emissionImage[i*4 + 1];
        char& eb = emissionImage[i*4 + 2];

        char s = char((r + g + b) / 3);
        char e = char((er + eg + eb) / 3);

        r = s;
        g = e;
        b = 0;
        a = char(0xff);
      }

      ImageBuilder::createDDS(&specularImage, 1, destBasePath + "_m.dds");
    }
  }

  if (normals.exists()) {
    ImageBuilder::options |= ImageBuilder::NORMAL_MAP_BIT | ImageBuilder::ZYZX_BIT;

    ImageBuilder::convertToDDS(normals, destBasePath + "_n.dds");
  }

  Log::printEnd(" OK");
}

void Context::init()
{}

void Context::destroy()
{
  usedTextures.clear();
  usedTextures.trim();
  usedSounds.clear();
  usedSounds.trim();
  usedModels.clear();
  usedModels.trim();
}

Context context;

}
