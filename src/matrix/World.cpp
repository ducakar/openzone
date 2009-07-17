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

  void World::init()
  {
    foreach( bsp, translator.bsps.iterator() ) {
      bsps << new BSP();
      if( !bsps.last()->load( bsp->name ) ) {
        throw Exception( 0, "BSP loading failed" );
      }
    }
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

    foreach( str, structures.iterator() ) {
      if( *str != null ) {
        delete *str;
      }
    }
    structures.clear();

    foreach( obj, objects.iterator() ) {
      if( *obj != null ) {
        delete *obj;
      }
    }
    objects.clear();

    foreach( part, particles.iterator() ) {
      if( *part != null ) {
        delete *part;
      }
    }
    particles.clear();

    foreach( bsp, bsps.iterator() ) {
      (*bsp)->free();
      delete *bsp;
    }
    bsps.clear();

    PoolAlloc<Object::Event, 0>::pool.free();
    PoolAlloc<Object::Effect, 0>::pool.free();

    if( net.isServer ) {
      net.world << Net::Action( Net::CLEAR );
    }
  }

  void World::position( Structure *str )
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
        str->mins = Vec3( -bsp.maxs.x + str->p.x, -bsp.mins.x + str->p.y, bsp.mins.z + str->p.z );
        str->maxs = Vec3( -bsp.maxs.y + str->p.x, -bsp.maxs.y + str->p.y, bsp.maxs.z + str->p.z );
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

    getInters( *str, EPSILON );

    for( int x = minSectX; x <= maxSectX; x++ ) {
      for( int y = minSectY; y <= maxSectY; y++ ) {
        sectors[x][y].structures << str->index;
      }
    }
  }

  void World::unposition( Structure *str )
  {
    getInters( *str, EPSILON );

    for( int x = minSectX; x <= maxSectX; x++ ) {
      for( int y = minSectY; y <= maxSectY; y++ ) {
        sectors[x][y].structures.exclude( str->index );
      }
    }
  }

  inline void World::position( Object *obj )
  {
    obj->sector = world.getSector( obj->p );
    obj->sector->objects << obj;
  }

  inline void World::unposition( Object *obj )
  {
    obj->sector->objects.remove( obj );
    obj->sector = null;
  }

  inline void World::reposition( Object *obj )
  {
    Sector *oldSector = obj->sector;
    Sector *newSector = world.getSector( obj->p );

    if( newSector != oldSector ) {
      oldSector->objects.remove( obj );
      newSector->objects << obj;
      obj->sector = newSector;
    }
  }

  inline void World::position( Particle *part )
  {
    part->sector = world.getSector( part->p );
    part->sector->particles << part;
  }

  inline void World::unposition( Particle *part )
  {
    part->sector = null;
    part->sector->particles.remove( part );
  }

  inline void World::reposition( Particle *part )
  {
    Sector *oldSector = part->sector;
    Sector *newSector = world.getSector( part->p );

    if( newSector != oldSector ) {
      oldSector->particles.remove( part );
      newSector->particles << part;
      part->sector = newSector;
    }
  }

  inline void World::put( Object *obj )
  {
    if( net.isClient ) {
      return;
    }

    assert( obj->sector == null );
    position( obj );

    if( net.isServer ) {
      net.objects << Net::Action( Net::PUT, obj->index );
    }
  }

  inline void World::cut( Object *obj )
  {
    if( net.isClient ) {
      return;
    }

    assert( obj->sector != null );
    unposition( obj );

    if( net.isServer ) {
      net.objects << Net::Action( Net::CUT, obj->index );
    }
  }

  void World::add( Structure *str )
  {
    if( net.isClient ) {
      return;
    }

    if( strFreeQueue[freedQueue].isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strFreeQueue[freedQueue] >> str->index;
      structures[str->index] = str;
    }

    position( str );

    if( net.isServer ) {
      net.structs << Net::Action( Net::ADD, str->index );
    }
  }

  void World::add( Object *obj, bool doPut )
  {
    if( net.isClient ) {
      return;
    }

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

    if( net.isServer ) {
      net.objects << Net::Action( doPut ? Net::ADD : Net::ADD_NOPUT, obj->index );
    }
  }

  void World::add( Particle *part )
  {
    if( net.isClient ) {
      return;
    }
    if( partFreeQueue[freedQueue].isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partFreeQueue[freedQueue] >> part->index;
      particles[part->index] = part;
    }

    position( part );

    if( net.isServer ) {
      net.particles << Net::Action( Net::ADD, part->index );
    }
  }

  void World::remove( Structure *str )
  {
    if( net.isClient ) {
      return;
    }
    else if( net.isServer ) {
      net.structs << Net::Action( Net::REMOVE, str->index );
    }

    unposition( str );

    strFreeQueue[addingQueue] << str->index;

    structures[str->index] = null;
    delete str;
  }

  void World::remove( Object *obj )
  {
    if( net.isClient ) {
      return;
    }
    else if( net.isServer ) {
      net.objects << Net::Action( Net::REMOVE, obj->index );
    }

    if( obj->sector != null ) {
      unposition( obj );
    }

    objFreeQueue[addingQueue] << obj->index;

    objects[obj->index] = null;
    delete obj;
  }

  void World::remove( Particle *part )
  {
    if( net.isClient ) {
      return;
    }
    else if( net.isServer ) {
      net.particles << Net::Action( Net::REMOVE, part->index );
    }

    unposition( part );

    partFreeQueue[addingQueue] << part->index;

    particles[part->index] = null;
    delete part;
  }

  void World::genParticles( int number, const Vec3 &p,
                            const Vec3 &velocity, float velocitySpread,
                            float rejection, float mass, float lifeTime,
                            float size, const Vec3 &color, float colorSpread )
  {
    if( net.isClient ) {
      return;
    }

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
  }

  bool World::read( InputStream *istream )
  {
    assert( structures.length() == 0 && objects.length() == 0 && particles.length() == 0 );

    try {
      int nStructures = istream->readInt();
      int nObjects    = istream->readInt();
      int nParticles  = istream->readInt();

      String    bspFile;
      Structure *str;
      Object    *obj;
      String    typeName;
      Particle  *part;

      for( int i = 0; i < nStructures; i++ ) {
        istream->readString( bspFile );

        if( bspFile.length() == 0 ) {
          structures.add( null );
        }
        else {
          int bspIndex = translator.bspIndex( bspFile );
          if( bspIndex == -1 ) {
            throw Exception( 0, "BSP not loaded" );
          }
          str = new Structure();
          str->readFull( istream );
          str->index = i;
          str->bsp = bspIndex;
          position( str );
          structures << str;
        }
      }
      for( int i = 0; i < nObjects; i++ ) {
        istream->readString( typeName );

        if( typeName.length() == 0 ) {
          objects.add( null );
        }
        else {
          obj = translator.createObject( typeName.cstr(), istream );
          obj->index = i;
          position( obj );
          objects << obj;
        }
      }
      for( int i = 0; i < nParticles; i++ ) {
        bool exists = istream->readBool();

        if( !exists ) {
          particles.add( null );
        }
        else {
          part = new Particle();
          part->readFull( istream );
          part->index = i;
          position( part );
          particles << part;
        }
      }
    }
    catch( Exception ) {
      return false;
    }
    return true;
  }

  bool World::write( OutputStream *ostream )
  {
    try {
      ostream->writeInt( structures.length() );
      ostream->writeInt( objects.length() );
      ostream->writeInt( particles.length() );

      String    typeName;
      Structure *str;
      Object    *obj;
      Particle  *part;

      for( int i = 0; i < structures.length(); i++ ) {
        str = structures[i];

        if( str == null ) {
          ostream->writeString( "" );
        }
        else {
          ostream->writeString( translator.bsps[str->bsp].name );
          str->writeFull( ostream );
        }
      }
      for( int i = 0; i < objects.length(); i++ ) {
        obj = objects[i];

        if( obj == null ) {
          ostream->writeString( "" );
        }
        else {
          ostream->writeString( obj->type->name );
          obj->writeFull( ostream );
        }
      }
      for( int i = 0; i < particles.length(); i++ ) {
        part = particles[i];

        if( part == null ) {
          ostream->writeBool( false );
        }
        else {
          ostream->writeBool( true );
          part->writeFull( ostream );
        }
      }
    }
    catch( Exception ) {
      return false;
    }
    return true;
  }

}
