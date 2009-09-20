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

  const float Sector::SIZE = (float) Sector::SIZEI;
  const float Sector::RADIUS = Sector::SIZE * Math::SQRT2 / 2.0f;

  const float World::DIM = Sector::SIZE * World::MAX / 2.0f;

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

    getInters( *str, EPSILON );

    for( int x = minX; x <= maxX; x++ ) {
      for( int y = minY; y <= maxY; y++ ) {
        sectors[x][y].structures << str->index;
      }
    }
  }

  void World::unposition( Structure *str )
  {
    getInters( *str, EPSILON );

    for( int x = minX; x <= maxX; x++ ) {
      for( int y = minY; y <= maxY; y++ ) {
        sectors[x][y].structures.exclude( str->index );
      }
    }
  }

  void World::position( Object *obj )
  {
    obj->sector = world.getSector( obj->p );
    obj->sector->objects << obj;
  }

  void World::unposition( Object *obj )
  {
    obj->sector->objects.remove( obj );
    obj->sector = null;
  }

  void World::reposition( Object *obj )
  {
    Sector *oldSector = obj->sector;
    Sector *newSector = world.getSector( obj->p );

    if( newSector != oldSector ) {
      oldSector->objects.remove( obj );
      newSector->objects << obj;
      obj->sector = newSector;
    }
  }

  void World::position( Particle *part )
  {
    part->sector = world.getSector( part->p );
    part->sector->particles << part;
  }

  void World::unposition( Particle *part )
  {
    part->sector->particles.remove( part );
    part->sector = null;
  }

  void World::reposition( Particle *part )
  {
    Sector *oldSector = part->sector;
    Sector *newSector = world.getSector( part->p );

    if( newSector != oldSector ) {
      oldSector->particles.remove( part );
      newSector->particles << part;
      part->sector = newSector;
    }
  }

  void World::put( Structure *str )
  {
    if( strAvailableIndices.isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strAvailableIndices >> str->index;
      structures[str->index] = str;
    }
  }

  void World::put( Object *obj )
  {
   if( objAvailableIndices.isEmpty() ) {
      obj->index = objects.length();
      objects << obj;
    }
    else {
      objAvailableIndices >> obj->index;
      objects[obj->index] = obj;
    }
  }

  void World::put( Particle *part )
  {
    if( partAvailableIndices.isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partAvailableIndices >> part->index;
      particles[part->index] = part;
    }
  }

  void World::cut( Structure *str )
  {
    strPendingIndices << str->index;
    structures[str->index] = null;
    str->index = -1;
  }

  void World::cut( Object *obj )
  {
    objPendingIndices << obj->index;
    objects[obj->index] = null;
    obj->index = -1;
  }

  void World::cut( Particle *part )
  {
    partPendingIndices << part->index;
    particles[part->index] = null;
    part->index = -1;
  }

  World::World() :
      Bounds( Vec3( -World::DIM, -World::DIM, -World::DIM ),
              Vec3(  World::DIM,  World::DIM,  World::DIM ) )
  {}

  void World::init()
  {
    terra.init( -DIM );

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

    foreach( bsp, bsps.iterator() ) {
      ( *bsp )->free();
      delete *bsp;
    }
    bsps.clear();

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

    PoolAlloc<Object::Event, 0>::pool.free();
    PoolAlloc<Particle, 0>::pool.free();
  }

  void World::update()
  {
    assert( strPendingIndices.isEmpty() );

    strAvailableIndices.addAll( strPendingIndices );
    objAvailableIndices.addAll( objPendingIndices );
    partAvailableIndices.addAll( partPendingIndices );

    assert( strAvailableIndices.isEmpty() );

    strPendingIndices.clear();
    objPendingIndices.clear();
    partPendingIndices.clear();
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

      put( new Particle( p, velocity + velDisturb, rejection, mass, 0.5f * lifeTime + timeDisturb,
                         size, color + colorDisturb ) );
    }
  }

  bool World::read( InputStream *istream )
  {
    assert( structures.length() == 0 && objects.length() == 0 && particles.length() == 0 );

    log.print( "Loading world ..." );
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
          structures << null;
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
          structures << str;
          position( str );
        }
      }
      for( int i = 0; i < nObjects; i++ ) {
        istream->readString( typeName );

        if( typeName.length() == 0 ) {
          objects << null;
        }
        else {
          obj = translator.createObject( typeName, istream );
          obj->index = i;
          objects << obj;
          position( obj );
        }
      }
      for( int i = 0; i < nParticles; i++ ) {
        bool exists = istream->readBool();

        if( !exists ) {
          particles << null;
        }
        else {
          part = new Particle();
          part->readFull( istream );
          part->index = i;
          particles << part;
          position( part );
        }
      }
    }
    catch( const Exception &e ) {
      log.printEnd( " %s", e.message );
      return false;
    }
    log.printEnd( " OK" );
    return true;
  }

  bool World::write( OutputStream *ostream )
  {
    log.print( "Saving world ..." );
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
    catch( const Exception &e ) {
      log.printEnd( " %s", e.message );
      return false;
    }
    log.printEnd( " OK" );
    return true;
  }

}
