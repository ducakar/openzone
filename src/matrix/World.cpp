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

  inline void World::position( Object *obj )
  {
    obj->sector = world.getSector( obj->p );
    obj->sector->objects << obj;
  }

  inline void World::unposition( Object *obj )
  {
    obj->sector->objects.remove( obj );
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

  void World::put( Structure *str )
  {
    if( strFreeIndices.isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strFreeIndices >> str->index;
      structures[str->index] = str;
    }

    position( str );
  }

  void World::put( Object *obj )
  {
    if( objFreeIndices.isEmpty() ) {
      obj->index = objects.length();
      objects << obj;
    }
    else {
      objFreeIndices >> obj->index;
      objects[obj->index] = obj;
    }

    position( obj );
  }

  void World::put( Particle *part )
  {
    if( partFreeIndices.isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partFreeIndices >> part->index;
      particles[part->index] = part;
    }

    position( part );
  }

  void World::cut( Structure *str )
  {
    unposition( str );

    strFreeIndices << str->index;
    structures[str->index] = null;
  }

  void World::cut( Object *obj )
  {
    unposition( obj );

    objFreeIndices << obj->index;
    objects[obj->index] = null;
  }

  void World::cut( Particle *part )
  {
    unposition( part );

    partFreeIndices << part->index;
    particles[part->index] = null;
  }

  World::World() :
      Bounds( Vec3( -World::DIM, -World::DIM, -World::DIM ),
              Vec3(  World::DIM,  World::DIM,  World::DIM ) )
  {}

  void World::init()
  {
    terrain.init( -DIM );

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
      (*bsp)->free();
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

    PoolAlloc<Object::Event, 0>::pool.free();

    foreach( part, particles.iterator() ) {
      if( *part != null ) {
        delete *part;
      }
    }
    particles.clear();
  }



  void World::genParticles( int number, const Vec3 &p,
                            const Vec3 &velocity, float velocitySpread,
                            float rejection, float mass, float lifeTime,
                            float size, const Vec3 &color, float colorSpread )
  {
    if( synapse.isClient ) {
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

      put( new Particle( p, velocity + velDisturb, rejection, mass, 0.5f * lifeTime + timeDisturb,
                         size, color + colorDisturb ) );
    }
  }

  void World::commit()
  {
    synapse.clearTickets();

    // put
    foreach( i, synapse.putStructs.iterator() ) {
      put( *i );
      synapse.putStructsIndices << ( *i )->index;
    }
    foreach( i, synapse.putObjects.iterator() ) {
      put( *i );
      synapse.putObjectsIndices << ( *i )->index;
    }
    foreach( i, synapse.putParts.iterator() ) {
      put( *i );
      synapse.putPartsIndices << ( *i )->index;
    }

    // cut
    foreach( i, synapse.cutStructs.iterator() ) {
      cut( *i );
    }
    foreach( i, synapse.cutObjects.iterator() ) {
      cut( *i );
    }
    foreach( i, synapse.cutParts.iterator() ) {
      cut( *i );
    }

    // remove (cut & delete)
    foreach( i, synapse.removeStructs.iterator() ) {
      cut( *i );
      delete *i;
    }
    foreach( i, synapse.removeObjects.iterator() ) {
      cut( *i );
      delete *i;
    }
    foreach( i, synapse.removeParts.iterator() ) {
      cut( *i );
      delete *i;
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
