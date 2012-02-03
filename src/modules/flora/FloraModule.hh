/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/modules/FloraModule.hh
 */

#pragma once

#include "modules/Module.hh"

namespace oz
{
namespace module
{

class FloraModule : public Module
{
  private:

    static const float DENSITY;
    static const float SPACING;

    // plants/m2
    float density;
    // dim * SPACING
    float spacing;
    int   number;

    void addTree( float x, float y );
    void addPlant( const char* type, float x, float y );

  public:

    void seed();

    virtual void load();

    virtual void registerLua() const;

  private:

    OZ_LUA_API( ozFloraGetDensity );
    OZ_LUA_API( ozFloraSetDensity );
    OZ_LUA_API( ozFloraGetSpacing );
    OZ_LUA_API( ozFloraSetSpacing );
    OZ_LUA_API( ozFloraGetNumber );
    OZ_LUA_API( ozFloraSeed );

};

extern FloraModule floraModule;

}
}
