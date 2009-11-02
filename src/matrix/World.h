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
#include "DynObject.h"
#include "Particle.h"

#include "Translator.h"

namespace oz
{

  class Synapse;

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float RADIUS;

    Vector<int>        structures;
    DList<Object, 0>   objects;
    DList<Particle, 0> particles;
  };

  class World : public Bounds
  {
    friend class Synapse;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      // for returning getInters cell indices
      int                minX;
      int                minY;
      int                maxX;
      int                maxY;

      Sky                sky;
      Terrain            terra;
      Cell               cells[World::MAX][World::MAX];
      Vector<BSP*>       bsps;
      Vector<Structure*> structures;
      Vector<Object*>    objects;
      Vector<Particle*>  particles;

    private:

      Vector<int>        strAvailableIndices;
      Vector<int>        objAvailableIndices;
      Vector<int>        partAvailableIndices;

      Vector<int>        strPendingIndices;
      Vector<int>        objPendingIndices;
      Vector<int>        partPendingIndices;

    public:

      // get pointer to the cell the point is in
      Cell *getCell( float x, float y );
      Cell *getCell( const Vec3 &p );

      // get indices of the cell the point is in
      void getInters( float x, float y, float epsilon = 0.0f );
      void getInters( const Vec3 &p, float epsilon = 0.0f );

      // get indices of min and max cells which the area intersects
      void getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                      float epsilon = 0.0f );

      // get indices of min and max cells which the AABB intersects
      void getInters( const AABB &bb, float epsilon = 0.0f );

      // get indices of min and max cells which the bounds intersects
      void getInters( const Bounds &bounds, float epsilon = 0.0f );

    private:

      void position( Structure *str );
      void unposition( Structure *str );

      void position( Object *obj );
      void unposition( Object *obj );
      void reposition( Object *obj );

      void position( Particle *part );
      void unposition( Particle *part );
      void reposition( Particle *part );

      void put( Structure *str );
      void put( Object *obj );
      void put( Particle *part );

      void cut( Structure *str );
      void cut( Object *obj );
      void cut( Particle *part );

    public:

      explicit World();

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

  inline void World::getInters( float x, float y, float epsilon )
  {
    minX = max( static_cast<int>( x - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    minY = max( static_cast<int>( y - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    maxX = min( static_cast<int>( x + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    maxY = min( static_cast<int>( y + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void World::getInters( const Vec3 &p, float epsilon )
  {
    getInters( p.x, p.y, epsilon );
  }

  inline void World::getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                                float epsilon )
  {
    minX = max( static_cast<int>( minPosX - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    minY = max( static_cast<int>( minPosY - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    maxX = min( static_cast<int>( maxPosX + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    maxY = min( static_cast<int>( maxPosY + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void World::getInters( const AABB &bb, float epsilon )
  {
    getInters( bb.p.x - bb.dim.x, bb.p.y - bb.dim.y, bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
               epsilon );
  }

  inline void World::getInters( const Bounds &bounds, float epsilon )
  {
    getInters( bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
  }

}
