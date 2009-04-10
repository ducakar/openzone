/*
 *  World.h
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world)
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "Structure.h"
#include "DynObject.h"
#include "Particle.h"

namespace oz
{

  struct Sector
  {
    static const float DIM;
    static const float RADIUS;

    Vector<int> structures;

    DList<Object, 0>    objects;
    DList<Particle, 0>  particles;
  };

  class World : public Bounds
  {
    private:

      // List of free indices. Indices can be reused after one full world update pass, so that all
      // references to those indices are removed (object update functions should remove all invalid
      // references).
      Vector<int> strFreeQueue[3];
      Vector<int> objFreeQueue[3];
      Vector<int> partFreeQueue[3];

      int addingQueue;
      int standbyQueue;
      int freedQueue;

    public:

      // # of sectors on each (x, y) axis
      static const int MAX = 128;
      static const float DIM;

      Sky     sky;
      Terrain terrain;
      Sector  sectors[World::MAX][World::MAX];

      Vector<Structure*>  structures;
      Vector<Object*>     objects;
      Vector<Particle*>   particles;

      // bsp list (should not change during world existance)
      Vector<BSP*>        bsps;

      int minSectX, minSectY;
      int maxSectX, maxSectY;

      World();

      // get pointer to the sector the point is in
      Sector* getSector( float x, float y );
      Sector* getSector( const Vec3 &p );

      // get indices of the sector the point is in
      void getInters( float x, float y, float epsilon = 0.0f );
      void getInters( const Vec3 &p, float epsilon = 0.0f );

      // get indices of min and max sectors which the AABB intersects
      void getInters( const AABB &bb, float epsilon = 0.0f );

      // get indices of min and max sectors which the bounds intersects
      void getInters( const Bounds &bounds, float epsilon = 0.0f );

      /**
       * Put the object into the world hashspace
       */
      void put( Object *obj );
      void cut( Object *obj );

      int add( Structure *str );
      int add( Object *obj, bool doPut = true );
      int add( Particle *part );

      void remove( Structure *str );
      void remove( Object *obj );
      void remove( Particle *part );

      void genParticles( int number, const Vec3 &p,
                         const Vec3 &velocity, float velocitySpread,
                         float rejection, float mass, float lifeTime,
                         float size, const Vec3 &color, float colorSpread );

      void beginUpdate();
      void endUpdate();
      // trim vectors to optimize memory usage
      void trim();

      void add( BSP *bsp );

      void free();
  };

  extern World world;

  inline Sector *World::getSector( float x, float y )
  {
    int ix = (int)( ( x + World::DIM ) / Sector::DIM );
    int iy = (int)( ( y + World::DIM ) / Sector::DIM );

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &sectors[ix][iy];
  }

  inline Sector *World::getSector( const Vec3 &p )
  {
    int ix = (int)( ( p.x + World::DIM ) / Sector::DIM );
    int iy = (int)( ( p.y + World::DIM ) / Sector::DIM );

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &sectors[ix][iy];
  }

  inline void World::getInters( float x, float y, float epsilon )
  {
    minSectX = (int)( ( x - epsilon + World::DIM ) / Sector::DIM );
    minSectY = (int)( ( y - epsilon + World::DIM ) / Sector::DIM );

    maxSectX = (int)( ( x + epsilon + World::DIM ) + 1 / Sector::DIM );
    maxSectY = (int)( ( y + epsilon + World::DIM ) + 1 / Sector::DIM );

    minSectX = bound( minSectX, 0, World::MAX - 1 );
    minSectY = bound( minSectY, 0, World::MAX - 1 );
    maxSectX = bound( maxSectX, 0, World::MAX - 1 );
    maxSectY = bound( maxSectY, 0, World::MAX - 1 );
  }

  inline void World::getInters( const Vec3 &p, float epsilon )
  {
    minSectX = (int)( ( p.x - epsilon + World::DIM ) / Sector::DIM );
    minSectY = (int)( ( p.y - epsilon + World::DIM ) / Sector::DIM );

    maxSectX = (int)( ( p.x + epsilon + World::DIM ) / Sector::DIM );
    maxSectY = (int)( ( p.y + epsilon + World::DIM ) / Sector::DIM );

    minSectX = bound( minSectX, 0, World::MAX - 1 );
    minSectY = bound( minSectY, 0, World::MAX - 1 );
    maxSectX = bound( maxSectX, 0, World::MAX - 1 );
    maxSectY = bound( maxSectY, 0, World::MAX - 1 );
  }

  inline void World::getInters( const AABB &bb, float epsilon )
  {
    minSectX = (int)( ( bb.p.x - bb.dim.x - epsilon + World::DIM ) / Sector::DIM );
    minSectY = (int)( ( bb.p.y - bb.dim.y - epsilon + World::DIM ) / Sector::DIM );

    maxSectX = (int)( ( bb.p.x + bb.dim.x + epsilon + World::DIM ) / Sector::DIM );
    maxSectY = (int)( ( bb.p.y + bb.dim.y + epsilon + World::DIM ) / Sector::DIM );

    minSectX = bound( minSectX, 0, World::MAX - 1 );
    minSectY = bound( minSectY, 0, World::MAX - 1 );
    maxSectX = bound( maxSectX, 0, World::MAX - 1 );
    maxSectY = bound( maxSectY, 0, World::MAX - 1 );
  }

  inline void World::getInters( const Bounds &bounds, float epsilon )
  {
    minSectX = (int)( ( bounds.mins.x - epsilon + World::DIM ) / Sector::DIM );
    minSectY = (int)( ( bounds.mins.y - epsilon + World::DIM ) / Sector::DIM );

    maxSectX = (int)( ( bounds.maxs.x + epsilon + World::DIM ) / Sector::DIM );
    maxSectY = (int)( ( bounds.maxs.y + epsilon + World::DIM ) / Sector::DIM );

    minSectX = bound( minSectX, 0, World::MAX - 1 );
    minSectY = bound( minSectY, 0, World::MAX - 1 );
    maxSectX = bound( maxSectX, 0, World::MAX - 1 );
    maxSectY = bound( maxSectY, 0, World::MAX - 1 );
  }

}
