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
 * @file ozFactory/TerraBuilder.hh
 *
 * `TerraBuilder` class.
 */

#pragma once

#include "ImageBuilder.hh"

namespace oz
{

/**
 * Utilities for generating terrain heightmaps and textures.
 */
class TerraBuilder
{
public:

  /**
   * Noise-producing modules for different layer of the terrain.
   */
  enum Module
  {
    PLAINS,
    MOUNTAINS,
    COMBINER,
    TURBULENCE,
    NOISE
  };

public:

  /**
   * Forbid instances.
   */
  explicit TerraBuilder() = delete;

  /**
   * Set height values to which -1.0 and +1.0 map respectively.
   *
   * This roughly represents the highest and the lowest height in the heightmap. Note that the
   * generated height may go also slightly out of these bounds.
   *
   * Default: -1.0 and +1.0.
   * Applies to: PLAINS, MOUNTAINS, NOISE.
   */
  static bool setBounds( Module module, float bottomHeight, float topHeight );

  /**
   * Set seed.
   *
   * Default: 0.
   * Applies to: PLAINS, MOUNTAINS, COMBINER, TURBULENCE, NOISE.
   */
  static bool setSeed( Module module, int seed );

  /**
   * Set number of octaves for a given module.
   *
   * Default: 6.
   * Applies to: PLAINS, MOUNTAINS, COMBINER, NOISE.
   */
  static bool setOctaveCount( Module module, int count );

  /**
   * Set roughness.
   *
   * Default: 3.
   * Applies to: TURBULENCE.
   */
  static bool setRoughness( Module module, int roughness );

  /**
   * Set frequency for a given module.
   *
   * Default: 1.0.
   * Applies to: PLAINS, MOUNTAINS, COMBINER, TURBULENCE, NOISE.
   */
  static bool setFrequency( Module module, float frequency );

  /**
   * Set persistence for a given module.
   *
   * Default: 0.5.
   * Applies to: PLAINS, COMBINER, NOISE.
   */
  static bool setPersistence( Module module, float persistence );

  /**
   * Set power.
   *
   * Default: 1.0.
   * Applies to: TURBULENCE.
   */
  static bool setPower( Module module, float power );

  /**
   * Set combiner control module which defines mountain regions (PLAINS or COMBINER).
   *
   * Default: COMBINER.
   * Applies to: COMBINER.
   */
  static void setMountainsControl( Module module );

  /**
   * Set lower bound for combiner control input to switch to mountain terrain.
   *
   * Default: -1.0 and +1.0.
   * Applies to: COMBINER.
   */
  static void setMountainsBounds( float lower, float upper );

  /**
   * Set edge smoothing between plain and mountain regions.
   *
   * Default: 0.0.
   * Applies to: COMBINER.
   */
  static void setEdgeFalloff( float falloff );

  /**
   * Add a colour to gradient scale. W coordinate is used as level.
   */
  static void addGradientPoint( const Vec4& point );

  /**
   * Clear gradient scale.
   */
  static void clearGradient();

  /**
   * Generate column-major heightmap array.
   */
  static float* generateHeightmap( int width, int height );

  /**
   * Generate terrain texture using the given gradient scale.
   */
  static ImageData generateImage( int width, int height );

  /**
   * Generate grey noise on cube map.
   *
   * Returns an array of 6 pointers to RGB images { +x, -x, +y, -y, +z, -z } for OpenGL cube map
   * texture. -1.0 maps to black and +1.0 maps to white, so make sure NOISE module has bounds close
   * to [-1.0, +1.0].
   *
   * All textures are stretched for an amount of 1 pixel, so the adjacent pixels edge pixels of
   * adjacent textures actually map to the same point in an effort to reduce artefacts on borders of
   * low-resolution textures.
   *
   * Do not forget to free the returned array and all its members.
   */
  static ImageData* generateCubeNoise( int size );

};

}
