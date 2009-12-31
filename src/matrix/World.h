/*
 *  World.h
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world).
 *  The world should not be manipulated directly; use Synapse instead.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "Structure.h"
#include "Dynamic.h"
#include "Particle.h"

#include "Translator.h"
#include "Lua.h"

namespace oz
{

  struct Synapse;
  struct Physics;

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float INV_SIZE;
    static const float RADIUS;

    SVector<int, 7> structs;
    DList<Object>   objects;
    DList<Particle> parts;
  };

  struct World : Bounds
  {
    friend struct Synapse;
    friend struct Physics;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      Sky                sky;
      Terrain            terra;
      Cell               cells[World::MAX][World::MAX];
      Vector<BSP*>       bsps;
      Vector<Structure*> structs;
      Vector<Object*>    objects;
      Vector<Particle*>  parts;

    private:

      Area               area;

      /*
       * Index reusing: when an entity is removed, there may still be references to it (from other
       * entities or from render or audio subsystems); that's why every cycle all references must
       * be checked if the slot they're pointing at (all references should be indices of a slot
       * in World::structures/objects/particles vectors). If the target slot is null, the referenced
       * entity doesn't exist any more, so reference must be cleared. To make sure all references
       * can be checked that way, a full world update must pass before a slot is reused. Otherwise
       * an entity may be removed and immediately after that another added into it's slot; when an
       * another entity would retrieve the target entity via the reference: 1) it wouldn't get the
       * expected entity but a new one; that may result in program crash if the new one is not of
       * the same type, 2) it wouldn't detect the old entity has been removed/destroyed/whatever;
       * that may pose a big problem to rendering and audio subsystems as those must clear
       * models/audio objects of removed world objects.
       */

      int freeing;
      int waiting;

      // [freeing]: vector for indices that are currently being freed
      // [waiting]: indices that have been freed previous cycle; those can be reused next time
      Vector<int>        strFreedIndices[2];
      Vector<int>        objFreedIndices[2];
      Vector<int>        partFreedIndices[2];

      // indices of slots that can be reused
      Vector<int>        strAvailableIndices;
      Vector<int>        objAvailableIndices;
      Vector<int>        partAvailableIndices;

    public:

      void requestBSP( int bspIndex );

    private:

      bool position( Structure* str );
      void unposition( Structure* str );

      void position( Object* obj );
      void unposition( Object* obj );
      void reposition( Object* obj );

      void position( Particle* part );
      void unposition( Particle* part );
      void reposition( Particle* part );

      int  addStruct( const char* name, const Vec3& p, Structure::Rotation rot );
      int  addObject( const char* name, const Vec3& p );
      int  addPart( const Vec3& p, const Vec3& velocity, const Vec3& color,
                    float rejection, float mass, float lifeTime );

      void remove( Structure* str );
      void remove( Object* obj );
      void remove( Particle* part );

    public:

      // get pointer to the cell the point is in
      Cell* getCell( float x, float y );
      Cell* getCell( const Vec3& p );

      // get indices of the cell the point is in
      void getInters( Area& area, float x, float y, float epsilon = 0.0f ) const;
      void getInters( Area& area, const Vec3& p, float epsilon = 0.0f ) const;

      // get indices of min and max cells which the area intersects
      void getInters( Area& area, float minPosX, float minPosY, float maxPosX, float maxPosY,
                      float epsilon = 0.0f ) const;

      // get indices of min and max cells which the AABB intersects
      void getInters( Area& area, const AABB& bb, float epsilon = 0.0f ) const;

      // get indices of min and max cells which the bounds intersects
      void getInters( Area& area, const Bounds& bounds, float epsilon = 0.0f ) const;

      World();

      void init();
      void free();

      void load();
      void unload();

      void update();

      bool read( InputStream* istream );
      bool write( OutputStream* ostream );

  };

  extern World world;

  inline Cell* World::getCell( float x, float y )
  {
    int ix = int( ( x + World::DIM ) * Cell::INV_SIZE );
    int iy = int( ( y + World::DIM ) * Cell::INV_SIZE );

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &cells[ix][iy];
  }

  inline Cell* World::getCell( const Vec3& p )
  {
    return getCell( p.x, p.y );
  }

  inline void World::getInters( Area& area, float x, float y, float epsilon ) const
  {
    area.minX = max( int( ( x - epsilon + World::DIM ) * Cell::INV_SIZE ), 0 );
    area.minY = max( int( ( y - epsilon + World::DIM ) * Cell::INV_SIZE ), 0 );
    area.maxX = min( int( ( x + epsilon + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
    area.maxY = min( int( ( y + epsilon + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
  }

  inline void World::getInters( Area& area, const Vec3& p, float epsilon ) const
  {
    getInters( area, p.x, p.y, epsilon );
  }

  inline void World::getInters( Area& area, float minPosX, float minPosY,
                                float maxPosX, float maxPosY, float epsilon ) const
  {
    area.minX = max( int( ( minPosX - epsilon + World::DIM ) * Cell::INV_SIZE ), 0 );
    area.minY = max( int( ( minPosY - epsilon + World::DIM ) * Cell::INV_SIZE ), 0 );
    area.maxX = min( int( ( maxPosX + epsilon + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
    area.maxY = min( int( ( maxPosY + epsilon + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
  }

  inline void World::getInters( Area& area, const AABB& bb, float epsilon ) const
  {
    getInters( area, bb.p.x - bb.dim.x, bb.p.y - bb.dim.y, bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
               epsilon );
  }

  inline void World::getInters( Area& area, const Bounds& bounds, float epsilon ) const
  {
    getInters( area, bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
  }

  inline bool World::position( Structure* str )
  {
    str->setRotation( *bsps[str->bsp], str->rot );

    getInters( area, *str, EPSILON );

    for( int x = area.minX; x <= area.maxX; x++ ) {
      for( int y = area.minY; y <= area.maxY; y++ ) {
        if( cells[x][y].structs.length() == cells[x][y].structs.capacity() ) {
          return false;
        }
      }
    }

    for( int x = area.minX; x <= area.maxX; x++ ) {
      for( int y = area.minY; y <= area.maxY; y++ ) {
        assert( !cells[x][y].structs.contains( str->index ) );

        cells[x][y].structs << str->index;
      }
    }
    return true;
  }

  inline void World::unposition( Structure* str )
  {
    getInters( area, *str, EPSILON );

    for( int x = area.minX; x <= area.maxX; x++ ) {
      for( int y = area.minY; y <= area.maxY; y++ ) {
        assert( cells[x][y].structs.contains( str->index ) );

        cells[x][y].structs.exclude( str->index );
      }
    }
  }

  inline void World::position( Object* obj )
  {
    assert( obj->cell == null );

    obj->cell = getCell( obj->p );

    assert( !obj->cell->objects.contains( obj ) );

    obj->cell->objects << obj;
  }

  inline void World::unposition( Object* obj )
  {
    assert( obj->cell != null );
    assert( obj->cell->objects.contains( obj ) );

    obj->cell->objects.remove( obj );
    obj->cell = null;
  }

  inline void World::reposition( Object* obj )
  {
    assert( obj->cell != null );

    Cell* oldCell = obj->cell;
    Cell* newCell = getCell( obj->p );

    if( newCell != oldCell ) {
      assert( oldCell->objects.contains( obj ) );
      assert( !newCell->objects.contains( obj ) );

      oldCell->objects.remove( obj );
      newCell->objects << obj;
      obj->cell = newCell;
    }
  }

  inline void World::position( Particle* part )
  {
    assert( part->cell == null );

    part->cell = getCell( part->p );

    assert( !part->cell->parts.contains( part ) );

    part->cell->parts << part;
  }

  inline void World::unposition( Particle* part )
  {
    assert( part->cell != null );
    assert( part->cell->parts.contains( part ) );

    part->cell->parts.remove( part );
    part->cell = null;
  }

  inline void World::reposition( Particle* part )
  {
    assert( part->cell != null );

    Cell* oldCell = part->cell;
    Cell* newCell = getCell( part->p );

    if( newCell != oldCell ) {
      assert( oldCell->parts.contains( part ) );
      assert( !newCell->parts.contains( part ) );

      oldCell->parts.remove( part );
      newCell->parts << part;
      part->cell = newCell;
    }
  }

  inline void World::requestBSP( int bspIndex ) {
    if( bsps[bspIndex] == null ) {
      bsps[bspIndex] = new BSP();
      if( !bsps[bspIndex]->load( translator.bsps[bspIndex].name ) ) {
        throw Exception( "Matrix BSP loading failed" );
      }
    }
  }

  inline int World::addStruct( const char* name, const Vec3& p, Structure::Rotation rot )
  {
    int index;

    if( strAvailableIndices.isEmpty() ) {
      index = structs.length();
      structs << translator.createStruct( index, name, p, rot );
    }
    else {
      strAvailableIndices >> index;
      structs[index] = translator.createStruct( index, name, p, rot );
    }
    return index;
  }

  // has to be reentrant, can be called again from translator.createObject
  inline int World::addObject( const char* name, const Vec3& p )
  {
    int index;

    if( objAvailableIndices.isEmpty() ) {
      index = objects.length();
      // reserve slot so reentrant calls cannot occupy it again
      objects << null;
    }
    else {
      objAvailableIndices >> index;
    }
    // objects vector may relocate during createObject call, we must use this workaround
    Object* obj = translator.createObject( index, name, p );
    objects[index] = obj;

    if( objects[index]->flags & Object::LUA_BIT ) {
      lua.registerObject( index );
    }
    return index;
  }

  inline int World::addPart( const Vec3& p, const Vec3& velocity, const Vec3& color,
                             float rejection, float mass, float lifeTime )
  {
    int index;

    if( partAvailableIndices.isEmpty() ) {
      index = parts.length();
      parts << new Particle( index, p, velocity, color, rejection, mass, lifeTime );
    }
    else {
      partAvailableIndices >> index;
      parts[index] = new Particle( index, p, velocity, color, rejection, mass, lifeTime );
    }
    return index;
  }

  inline void World::remove( Structure* str )
  {
    assert( str->index >= 0 );

    strFreedIndices[freeing] << str->index;
    structs[str->index] = null;
    str->index = -1;
  }

  inline void World::remove( Object* obj )
  {
    assert( obj->index >= 0 );
    assert( obj->cell == null );

    if( obj->flags & Object::LUA_BIT ) {
      lua.unregisterObject( obj->index );
    }
    objFreedIndices[freeing] << obj->index;
    objects[obj->index] = null;
    obj->index = -1;
  }

  inline void World::remove( Particle* part )
  {
    assert( part->index >= 0 );
    assert( part->cell == null );

    partFreedIndices[freeing] << part->index;
    parts[part->index] = null;
    part->index = -1;
  }

}
