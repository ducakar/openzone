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
 * @file client/BSP.hh
 */

#pragma once

#include <client/Mesh.hh>

namespace oz
{
namespace client
{

class BSP
{
  private:

    const oz::BSP* bsp;
    Mesh           mesh;

  public:

    Vec4 waterFogColour;
    Vec4 lavaFogColour;

  public:

    explicit BSP( const oz::BSP* bsp );
    ~BSP();

    Vec3 dim() const
    {
      return mesh.dim;
    }

    bool isPreloaded() const
    {
      return mesh.isPreloaded();
    }

    bool isLoaded() const
    {
      return mesh.isLoaded();
    }

    void schedule( const Struct* str, Mesh::QueueType queue );

    void preload();
    void load();

};

}
}
