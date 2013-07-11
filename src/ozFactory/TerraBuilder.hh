/*
 * ozFactory - OpenZone Assets Builder Library.
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

/**
 * @file ozFactory/TerraBuilder.hh
 *
 * `TerraBuilder` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Utilities for generating terrain heightmaps and textures.
 */
class TerraBuilder
{
  public:

    enum Module
    {
      PLAINS,
      MOUNTAINS,
      TERRAIN_TYPE,
      TURBULENCE
    };

  public:

    /**
     * Forbid instances.
     */
    explicit TerraBuilder() = delete;

    /**
     * Get the last error string.
     */
    static const char* getError();

    /**
     * Set seed.
     */
    static void setSeed( int seed );

    /**
     * Set number of octaves for a given module.
     *
     * Default: 6.
     */
    static void setOctaveCount( Module module, int count );

    /**
     * Set frequency for a given module.
     *
     * Default: 1.0.
     */
    static void setFrequency( Module module, float frequency );

    /**
     * Set persistence for a given module.
     *
     * Default: 0.5.
     */
    static void setPersistence( Module module, float persistence );

    /**
     * Set height values to which -1.0 and +1.0 map respectively.
     *
     * This roughly represents the highest and the lowest height in the heightmap. Note that the
     * generated height may go also slightly out of these bounds.
     *
     * Default: -1.0 and +1.0.
     */
    static void setBounds( float bottomHeight, float topHeight );

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
     *
     * Returned image is in RGB format with lines aligned on 32-bit boundaries.
     */
    static char* generateImage( int width, int height );

};

}
