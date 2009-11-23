/*
 *  World.h
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world)
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "Structure.h"
#include "Bot.h"
#include "Particle.h"

#include "Translator.h"

namespace oz
{

  class Synapse;
  class Physics;

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float RADIUS;

    Vector<int>        structures;
    DList<Object, 0>   objects;
    DList<Particle, 0> particles;
  };

  struct Area
  {
    int minX;
    int maxX;
    int minY;
    int maxY;
  };

  class World : public Bounds
  {
    friend class Synapse;
    friend class Physics;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      Sky                sky;
      Terrain            terra;
      Cell               cells[World::MAX][World::MAX];
      Vector<BSP*>       bsps;
      Vector<Structure*> structures;
      Vector<Object*>    objects;
      Vector<Particle*>  particles;

    private:

      Area               area;

      Vector<int>        strAvailableIndices;
      Vector<int>        objAvailableIndices;
      Vector<int>        partAvailableIndices;

      Vector<int>        strPendingIndices;
      Vector<int>        objPendingIndices;
      Vector<int>        partPendingIndices;

      void position( Structure *str );
      void unposition( Structure *str );

      void position( Object *obj );
      void unposition( Object *obj );
      void reposition( Object *obj );

      void position( Particle *part );
      void unposition( Particle *part );
      void reposition( Particle *part );

      void add( Structure *str );
      void add( Object *obj );
      void add( Particle *part );

      void remove( Structure *str );
      void remove( Object *obj );
      void remove( Particle *part );

    public:

      // get pointer to the cell the point is in
      Cell *getCell( float x, float y );
      Cell *getCell( const Vec3 &p );

      // get indices of the cell the point is in
      void getInters( Area &area, float x, float y, float epsilon = 0.0f );
      void getInters( Area &area, const Vec3 &p, float epsilon = 0.0f );

      // get indices of min and max cells which the area intersects
      void getInters( Area &area, float minPosX, float minPosY, float maxPosX, float maxPosY,
                      float epsilon = 0.0f );

      // get indices of min and max cells which the AABB intersects
      void getInters( Area &area, const AABB &bb, float epsilon = 0.0f );

      // get indices of min and max cells which the bounds intersects
      void getInters( Area &area, const Bounds &bounds, float epsilon = 0.0f );

      void init();
      void free();

      void load();
      void unload();

      void update();

      bool read( InputStream *istream );
      bool write( OutputStream *ostream );

  };

  extern World world;

  inline Cell *World::getCell( float x, float y )
  {
    int ix = static_cast<int>( x + World::DIM ) / Cell::SIZEI;
    int iy = static_cast<int>( y + World::DIM ) / Cell::SIZEI;

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &cells[ix][iy];
  }

  inline Cell *World::getCell( const Vec3 &p )
  {
    return getCell( p.x, p.y );
  }

  inline void World::getInters( Area &area, float x, float y, float epsilon )
  {
    area.minX = max( static_cast<int>( x - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    area.minY = max( static_cast<int>( y - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    area.maxX = min( static_cast<int>( x + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    area.maxY = min( static_cast<int>( y + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void World::getInters( Area &area, const Vec3 &p, float epsilon )
  {
    getInters( area, p.x, p.y, epsilon );
  }

  inline void World::getInters( Area &area, float minPosX, float minPosY,
                                float maxPosX, float maxPosY, float epsilon )
  {
    area.minX = max( static_cast<int>( minPosX - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    area.minY = max( static_cast<int>( minPosY - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    area.maxX = min( static_cast<int>( maxPosX + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    area.maxY = min( static_cast<int>( maxPosY + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void World::getInters( Area &area, const AABB &bb, float epsilon )
  {
    getInters( area, bb.p.x - bb.dim.x, bb.p.y - bb.dim.y, bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
               epsilon );
  }

  inline void World::getInters( Area &area, const Bounds &bounds, float epsilon )
  {
    getInters( area, bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
  }

  inline void World::position( Structure *str )
  {
    const Bounds &bsp = *bsps[str->bsp];

    switch( str->rot ) {
      case Structure::R0: {
        str->mins = bsp.mins + str->p;
        str->maxs = bsp.maxs + str->p;
        break;
      }
      case Structure::R90: {
        str->mins = Vec3( -bsp.maxs.y + str->p.x, bsp.mins.x + str->p.y, bsp.mins.z + str->p.z );
        str->maxs = Vec3( -bsp.mins.y + str->p.x, bsp.maxs.x + str->p.y, bsp.maxs.z + str->p.z );
        break;
      }
      case Structure::R180: {
        str->mins = str->p + Vec3( -bsp.maxs.x, -bsp.maxs.y, +bsp.mins.z );
        str->maxs = str->p + Vec3( -bsp.mins.x, -bsp.mins.y, +bsp.maxs.z );
        break;
      }
      case Structure::R270: {
        str->mins = Vec3( bsp.mins.y + str->p.x, -bsp.maxs.x + str->p.y, bsp.mins.z + str->p.z );
        str->maxs = Vec3( bsp.maxs.y + str->p.x, -bsp.mins.x + str->p.y, bsp.maxs.z + str->p.z );
        break;
      }
      default: {
        assert( false );
        break;
      }
    }

    getInters( area, *str, EPSILON );

    for( int x = area.minX; x <= area.maxX; x++ ) {
      for( int y = area.minY; y <= area.maxY; y++ ) {
        assert( !cells[x][y].structures.contains( str->index ) );

        cells[x][y].structures << str->index;
      }
    }
  }

  inline void World::unposition( Structure *str )
  {
    getInters( area, *str, EPSILON );

    for( int x = area.minX; x <= area.maxX; x++ ) {
      for( int y = area.minY; y <= area.maxY; y++ ) {
        assert( cells[x][y].structures.contains( str->index ) );

        cells[x][y].structures.exclude( str->index );
      }
    }
  }

  inline void World::position( Object *obj )
  {
    assert( obj->cell == null );

    obj->cell = getCell( obj->p );

    assert( !obj->cell->objects.contains( obj ) );

    obj->cell->objects << obj;
  }

  inline void World::unposition( Object *obj )
  {
    assert( obj->cell != null );
    assert( obj->cell->objects.contains( obj ) );

    obj->cell->objects.remove( obj );
    obj->cell = null;
  }

  inline void World::reposition( Object *obj )
  {
    assert( obj->cell != null );

    Cell *oldCell = obj->cell;
    Cell *newCell = getCell( obj->p );

    if( newCell != oldCell ) {
      assert( oldCell->objects.contains( obj ) );
      assert( !newCell->objects.contains( obj ) );

      oldCell->objects.remove( obj );
      newCell->objects << obj;
      obj->cell = newCell;
    }
  }

  inline void World::position( Particle *part )
  {
    assert( part->cell == null );

    part->cell = getCell( part->p );

    assert( !part->cell->particles.contains( part ) );

    part->cell->particles << part;
  }

  inline void World::unposition( Particle *part )
  {
    assert( part->cell != null );
    assert( part->cell->particles.contains( part ) );

    part->cell->particles.remove( part );
    part->cell = null;
  }

  inline void World::reposition( Particle *part )
  {
    assert( part->cell != null );

    Cell *oldCell = part->cell;
    Cell *newCell = getCell( part->p );

    if( newCell != oldCell ) {
      assert( oldCell->particles.contains( part ) );
      assert( !newCell->particles.contains( part ) );

      oldCell->particles.remove( part );
      newCell->particles << part;
      part->cell = newCell;
    }
  }

  inline void World::add( Structure *str )
  {
    assert( str->index == -1 );

    if( strAvailableIndices.isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strAvailableIndices >> str->index;
      structures[str->index] = str;
    }
  }

  inline void World::add( Object *obj )
  {
    assert( obj->index == -1 );
    assert( obj->cell == null );

    if( objAvailableIndices.isEmpty() ) {
      obj->index = objects.length();
      objects << obj;
    }
    else {
      objAvailableIndices >> obj->index;
      objects[obj->index] = obj;
    }
  }

  inline void World::add( Particle *part )
  {
    assert( part->index == -1 );
    assert( part->cell == null );

    if( partAvailableIndices.isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partAvailableIndices >> part->index;
      particles[part->index] = part;
    }
  }

  inline void World::remove( Structure *str )
  {
    assert( str->index >= 0 );

    strPendingIndices << str->index;
    structures[str->index] = null;
    str->index = -1;
  }

  inline void World::remove( Object *obj )
  {
    assert( obj->index >= 0 );
    assert( obj->cell == null );

    objPendingIndices << obj->index;
    objects[obj->index] = null;
    obj->index = -1;
  }

  inline void World::remove( Particle *part )
  {
    assert( part->index >= 0 );
    assert( part->cell == null );

    partPendingIndices << part->index;
    particles[part->index] = null;
    part->index = -1;
  }

}
