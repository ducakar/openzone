/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/BSP.hpp
 */

#pragma once

#include "matrix/Struct.hpp"
#include "matrix/BSP.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

class BSP
{
  private:

    int  id;
    int  flags;
    Mesh mesh;

    const matrix::BSP* bsp;

    void playSound( const Struct::Entity* entity, int sound ) const;
    void playContSound( const Struct::Entity* entity, int sound ) const;

  public:

    bool isLoaded;

    explicit BSP( int id );
    ~BSP();

    void draw( const Struct* str, int mask ) const;
    void play( const Struct* str ) const;

    void load();

};

}
}
