/*
 * ozFactory - OpenZone Assets Builder Library.
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

/**
 * @file ozFactory/ModelBuilder.hh
 *
 * `ModelBuilder` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %ModelBuilder converts generic 3D model to OpenZone format (.ozModel).
 *
 * Assimp (Open Asset Import Library) is used to read source models.
 */
class ModelBuilder
{
public:

  /**
   * Forbid instances.
   */
  ModelBuilder() = delete;

  /**
   * Get the last error string from Assimp.
   */
  static const char* getError();

  /**
   * True iff the given 3D model format is supported by Assimp (only file extension is checked).
   */
  static bool isModel(const File& file);

  /**
   * Build a Collada (.dae) model into OpenZone (.ozModel) format.
   */
  static bool buildModel(const File& file, Stream* os);

};

}
