/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file matrix/Synapse.hh
 *
 * Orbis manipulation interface.
 */

#pragma once

#include <matrix/Orbis.hh>

namespace oz
{

class Bot;

class Synapse
{
public:

  enum Mode
  {
    SINGLE, ///< No need to worry about synchronisation of object creation, deletion,
            ///< interactions ...
    SERVER, ///< Create notification about every object creation/deletion/put/cut/etc that can be
            ///< later serialised and sent to clients.
    CLIENT  ///< Do not create or delete any objects by yourself, every creation/deletion must be
            ///< ordered by the server.
  };

  List<int> putObjects;
  List<int> cutObjects;

  List<int> addedStructs;
  List<int> addedObjects;
  List<int> addedFrags;

  List<int> removedStructs;
  List<int> removedObjects;
  List<int> removedFrags;

  Mode mode;

public:

  explicit Synapse();

  void use( Bot* user, Object* target );
  void trigger( Entity* target );
  void lock( Bot* user, Entity* target );

  // schedule for position in the world
  void put( Dynamic* obj );

  // schedule for unposition from world
  void cut( Dynamic* obj );

  // create an object, schedule for addition in the world and return predicted world index
  Struct* add( const BSP* bsp, const Point& p, Heading heading, bool empty );
  Object* add( const ObjectClass* clazz, const Point& p, Heading heading, bool empty );
  Frag*   add( const FragPool* pool, const Point& p, const Vec3& velocity );
  void    gen( const FragPool* pool, int nFrags, const Bounds& bb, const Vec3& velocity );

  Struct* addStruct( const char* bspName, const Point& p, Heading heading, bool empty );
  Object* addObject( const char* className, const Point& p, Heading heading, bool empty );
  Frag*   addFrag( const char* poolName, const Point& p, const Vec3& velocity );
  void    genFrags( const char* poolName, int nFrags, const Bounds& bb, const Vec3& velocity );

  // schedule for removal from physical world and delete it
  void remove( Struct* str );
  void remove( Object* obj );
  void remove( Frag* frag );

  void removeStruct( int index );
  void removeObject( int index );
  void removeFrag( int index );

  // clear lists for actions, additions, removals
  void update();

  void load();
  void unload();

};

extern Synapse synapse;

}
