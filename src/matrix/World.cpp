/*
 *  World.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "World.h"

namespace oz
{

  World world;

  const float Sector::DIM = 16.0f;
  const float Sector::RADIUS = Sector::DIM * Math::SQRT2;

  const float World::DIM = Sector::DIM * World::MAX / 2.0f;

  World::World() :
      Bounds( Vec3( -World::DIM, -World::DIM, -World::DIM ),
              Vec3( World::DIM, World::DIM, World::DIM ) ),
      addingQueue( 0 ), standbyQueue( 1 ), freedQueue( 2 )
  {}

  void World::put( Object *obj )
  {
    assert( obj->sector == null );

    Sector *sector = getSector( obj->p );

    obj->sector = sector;
    sector->objects << obj;
  }

  void World::cut( Object *obj )
  {
    assert( obj->sector != null );

    obj->sector->objects.remove( obj );
    obj->sector = null;
  }

  int World::add( Structure *str )
  {
    str->mins = bsps[str->bsp]->mins + str->p;
    str->maxs = bsps[str->bsp]->maxs + str->p;

    if( strFreeQueue[freedQueue].isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strFreeQueue[freedQueue] >> str->index;
      structures[str->index] = str;
    }

    getInters( *str, EPSILON );

    for( int x = minSectX; x <= maxSectX; x++ ) {
      for( int y = minSectY; y <= maxSectY; y++ ) {
        sectors[x][y].structures << str->index;
      }
    }
    return str->index;
  }

  int World::add( Object *obj, bool doPut )
  {
    if( objFreeQueue[freedQueue].isEmpty() ) {
      obj->index = objects.length();
      objects << obj;
    }
    else {
      objFreeQueue[freedQueue] >> obj->index;
      objects[obj->index] = obj;
    }

    if( doPut ) {
      put( obj );
    }
    else {
      obj->sector = null;
    }
    return obj->index;
  }

  int World::add( Particle *part )
  {
    if( partFreeQueue[freedQueue].isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partFreeQueue[freedQueue] >> part->index;
      particles[part->index] = part;
    }

    Sector *sector = getSector( part->p );

    part->sector = sector;
    sector->particles << part;

    return part->index;
  }

  void World::remove( Structure *str )
  {
    getInters( *str, EPSILON );

    for( int x = minSectX; x <= maxSectX; x++ ) {
      for( int y = minSectY; y <= maxSectY; y++ ) {
        sectors[x][y].structures.exclude( str->index );
      }
    }

    strFreeQueue[addingQueue] << str->index;

    structures[str->index] = null;
    delete str;
  }

  void World::remove( Object *obj )
  {
    if( obj->sector != null ) {
      obj->sector->objects.remove( obj );
    }

    objFreeQueue[addingQueue] << obj->index;

    objects[obj->index] = null;
    delete obj;
  }

  void World::remove( Particle *part )
  {
    part->sector->particles.remove( part );

    partFreeQueue[addingQueue] << part->index;

    particles[part->index] = null;
    delete part;
  }

  void World::genParticles( int number, const Vec3 &p,
                            const Vec3 &velocity, float velocitySpread,
                            float rejection, float mass, float lifeTime,
                            float size, const Vec3 &color, float colorSpread )
  {
    float velocitySpread2 = velocitySpread / 2.0f;
    float colorSpread2 = colorSpread / 2.0f;

    for( int i = 0; i < number; i++ ) {
      Vec3 velDisturb = Vec3( velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2 );
      Vec3 colorDisturb = Vec3( colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2 );
      float timeDisturb = lifeTime * Math::frand();

      add( new Particle( p, velocity + velDisturb, rejection, mass, 0.5f * lifeTime + timeDisturb,
                         size, color + colorDisturb ) );
    }
  }

  void World::beginUpdate()
  {
    addingQueue = ( addingQueue + 1 ) % 3;
    standbyQueue = ( standbyQueue + 1 ) % 3;
    freedQueue = ( freedQueue + 1 ) % 3;

    world.sky.update();
  }

  void World::endUpdate()
  {}

  void World::trim()
  {
    for( int i = 0; i < 3; i++ ) {
      strFreeQueue[i].trim( 4 );
      objFreeQueue[i].trim( 4 );
      partFreeQueue[i].trim( 4 );
    }

    while( structures.last() == null ) {
      --structures;
    }
    structures.trim( 4 );

    while( objects.last() == null ) {
      --objects;
    }
    objects.trim( 32 );

    while( particles.last() == null ) {
      --particles;
    }
    particles.trim( 128 );

    bsps.trim( 1 );
  }

  void World::add( BSP *bsp )
  {
    bsps << bsp;
  }

  void World::free()
  {
    for( int i = 0; i < World::MAX; i++ ) {
      for( int j = 0; j < World::MAX; j++ ) {
        sectors[i][j].structures.clear();
        sectors[i][j].objects.clear();
        sectors[i][j].particles.clear();
      }
    }

    const int iMax = structures.length();
    for( int i = 0; i < iMax; i++ ) {
      if( structures[i] != null ) {
        delete structures[i];
        structures[i] = null;
      }
    }
    structures.clear();

    const int jMax = objects.length();
    for( int j = 0; j < jMax; j++ ) {
      if( objects[j] != null && objects[j]->sector != null ) {
        delete objects[j];
        objects[j] = null;
      }
    }
    objects.clear();

    const int kMax = particles.length();
    for( int k = 0; k < kMax; k++ ) {
      if( particles[k] != null ) {
        delete particles[k];
        particles[k] = null;
      }
    }
    particles.clear();

    PoolAlloc<Effect, 0>::pool.free();

    bsps.free();
    bsps.clear();
  }

}
