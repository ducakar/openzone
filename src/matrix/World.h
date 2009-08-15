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

#include "Translator.h"
#include "Synapse.h"

namespace oz
{

  struct Sector
  {
    static const int   DIMI;
    static const float DIM;
    static const float RADIUS;

    Vector<int> structures;

    DList<Object, 0>   objects;
    DList<Particle, 0> particles;
  };

  class World : public Bounds
  {
    public:

      // # of sectors on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      // for returning getInters sector indices
      int                minX;
      int                minY;
      int                maxX;
      int                maxY;

      Sky                sky;
      Terrain            terrain;
      Sector             sectors[World::MAX][World::MAX];
      Vector<BSP*>       bsps;
      Vector<Structure*> structures;
      Vector<Object*>    objects;
      Vector<Particle*>  particles;

    private:

      Vector<int>        strFreeIndices;
      Vector<int>        objFreeIndices;
      Vector<int>        partFreeIndices;

    public:

      // get pointer to the sector the point is in
      Sector *getSector( float x, float y );
      Sector *getSector( const Vec3 &p );

      // get indices of the sector the point is in
      void getInters( float x, float y, float epsilon = 0.0f );
      void getInters( const Vec3 &p, float epsilon = 0.0f );

      // get indices of min and max sectors which the AABB intersects
      void getInters( const AABB &bb, float epsilon = 0.0f );

      // get indices of min and max sectors which the bounds intersects
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

      void genParticles( int number, const Vec3 &p,
                         const Vec3 &velocity, float velocitySpread,
                         float rejection, float mass, float lifeTime,
                         float size, const Vec3 &color, float colorSpread );

      void commit();

      bool read( InputStream *istream );
      bool write( OutputStream *ostream );

  };

  extern World world;

  inline Sector *World::getSector( float x, float y )
  {
    int ix = (int) ( x + World::DIM ) / Sector::DIMI;
    int iy = (int) ( y + World::DIM ) / Sector::DIMI;

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &sectors[ix][iy];
  }

  inline Sector *World::getSector( const Vec3 &p )
  {
    return getSector( p.x, p.y );
  }

  inline void World::getInters( float x, float y, float epsilon )
  {
    minX = (int) ( x - epsilon + World::DIM ) / Sector::DIMI;
    minY = (int) ( y - epsilon + World::DIM ) / Sector::DIMI;

    maxX = (int) ( x + epsilon + World::DIM ) / Sector::DIMI;
    maxY = (int) ( y + epsilon + World::DIM ) / Sector::DIMI;

    minX = bound( minX, 0, World::MAX - 1 );
    minY = bound( minY, 0, World::MAX - 1 );
    maxX = bound( maxX, 0, World::MAX - 1 );
    maxY = bound( maxY, 0, World::MAX - 1 );
  }

  inline void World::getInters( const Vec3 &p, float epsilon )
  {
    return getInters( p.x, p.y, epsilon );
  }

  inline void World::getInters( const AABB &bb, float epsilon )
  {
    minX = (int) ( bb.p.x - bb.dim.x - epsilon + World::DIM ) / Sector::DIMI;
    minY = (int) ( bb.p.y - bb.dim.y - epsilon + World::DIM ) / Sector::DIMI;

    maxX = (int) ( bb.p.x + bb.dim.x + epsilon + World::DIM ) / Sector::DIMI;
    maxY = (int) ( bb.p.y + bb.dim.y + epsilon + World::DIM ) / Sector::DIMI;

    minX = bound( minX, 0, World::MAX - 1 );
    minY = bound( minY, 0, World::MAX - 1 );
    maxX = bound( maxX, 0, World::MAX - 1 );
    maxY = bound( maxY, 0, World::MAX - 1 );
  }

  inline void World::getInters( const Bounds &bounds, float epsilon )
  {
    minX = (int) ( bounds.mins.x - epsilon + World::DIM ) / Sector::DIMI;
    minY = (int) ( bounds.mins.y - epsilon + World::DIM ) / Sector::DIMI;

    maxX = (int) ( bounds.maxs.x + epsilon + World::DIM ) / Sector::DIMI;
    maxY = (int) ( bounds.maxs.y + epsilon + World::DIM ) / Sector::DIMI;

    minX = bound( minX, 0, World::MAX - 1 );
    minY = bound( minY, 0, World::MAX - 1 );
    maxX = bound( maxX, 0, World::MAX - 1 );
    maxY = bound( maxY, 0, World::MAX - 1 );
  }

}
