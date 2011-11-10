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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Synapse.hpp
 *
 * Orbis manipulation interface.
 */

#pragma once

#include "matrix/Orbis.hpp"

namespace oz
{
namespace matrix
{

class Bot;

class Synapse
{
  public:

    enum Mode
    {
      SINGLE, ///< No need to worry about synchronisation of object creation, deletion,
              ///< interactions ...
      SERVER, ///< Create notification about every object creation/deletion/put/cut/etc that can
              ///< be later serialised and sent to clients.
      CLIENT  ///< Do not create or delete any objects by yourself, every creation/deletion must
              ///< be ordered by the server.
    };

    Vector<int> putObjects;
    Vector<int> cutObjects;

    Vector<int> addedStructs;
    Vector<int> addedObjects;
    Vector<int> addedFrags;

    Vector<int> removedStructs;
    Vector<int> removedObjects;
    Vector<int> removedFrags;

    Mode mode;

    Synapse();

    void use( Bot* user, Object* target );

    // schedule for position in the world
    void put( Dynamic* obj );

    // schedule for unposition from world
    void cut( Dynamic* obj );

    // create an object, schedule for addition in the world and return predicted world index
    int  addStruct( const char* bspName, const Point3& p, Heading heading );
    int  addObject( const char* className, const Point3& p, Heading heading );
    int  addFrag( const char* poolName, const Point3& p, const Vec3& velocity );
    void genFrags( const char* poolName, int nFrags, const Bounds& bb, const Vec3& velocity );

    int  addStruct( const BSP* bsp, const Point3& p, Heading heading );
    int  addObject( const ObjectClass* clazz, const Point3& p, Heading heading );
    int  addFrag( const FragPool* pool, const Point3& p, const Vec3& velocity );
    void genFrags( const FragPool* pool, int nFrags, const Bounds& bb, const Vec3& velocity );

    // schedule for removal from physical world and delete it
    void remove( Struct* str );
    void remove( Object* obj );
    void remove( Frag* frag );

    // clear lists for actions, additions, removals
    void update();

    void load();
    void unload();

};

extern Synapse synapse;

}
}
