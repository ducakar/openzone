/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Matrix.h"

#include "Lua.h"
#include "Physics.h"
#include "FloraManager.h"

namespace oz
{

  Matrix matrix;

  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 100000.0f;

  void Matrix::loadStressTest()
  {
    static const float DIM = World::DIM;
    int j = 0;

    for( int i = 0; i < 400; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Goblin", Vec3( x, y, z ) );
        j++;
      }
    }
    for( int i = 0; i < 400; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Knight", Vec3( x, y, z ) );
        j++;
      }
    }
    for( int i = 0; i < 400; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Beast", Vec3( x, y, z ) );
        j++;
      }
    }
    for( int i = 0; i < 100; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Droid", Vec3( x, y, z ) );
        j++;
      }
    }
    for( int i = 0; i < 1000; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "MetalBarrel", Vec3( x, y, z ) );
      }
    }
    for( int i = 0; i < 1000; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 2.0f;

      if( z > 0.0f ) {
        synapse.addObject( "SmallCrate", Vec3( x, y, z ) );
      }
    }
  }

  void Matrix::loadSample()
  {
    world.sky.set( 205.0f, 1440.0f, 180.0f );

    int index = synapse.addObject( "Lord", Vec3( 52, -44, 37 ) );
    static_cast<Bot*>( world.objects[index] )->h = 270.0f;

    synapse.addObject( "Knight", Vec3( 50, -35, 37 ) );
    synapse.addObject( "Goblin", Vec3( 51, -35, 37 ) );

    synapse.addObject( "Wine", Vec3( 49, -36, 37 ) );
    synapse.addObject( "FirstAid", Vec3( 50, -36, 37 ) );
    synapse.addObject( "Rifle", Vec3( 51, -36, 37 ) );

    synapse.addObject( "Raptor", Vec3( 70, -60, 37 ) );

    synapse.addStruct( "castle", Vec3( 57, -33, 43 ), Structure::R0 );

    synapse.genParts( 1000, Vec3( 50, -36, 40 ), Vec3( 0, 0, 10 ), 15.0f,
                      Vec3( 0.4f, 0.4f, 0.4f ), 0.2f, 1.95f, 0.1f, 5.0f );

    synapse.addObject( "MetalBarrel", Vec3( 61, -44, 36 ) );
    synapse.addObject( "MetalBarrel", Vec3( 61, -44, 38 ) );
    synapse.addObject( "MetalBarrel", Vec3( 61, -44, 40 ) );

    synapse.addObject( "Tree3", Vec3( 77.7, -40, world.terra.height( 78, -40 ) + 6.5f ) );

    synapse.addObject( "SmallCrate", Vec3( 51, -42, 36 ) );
    synapse.addObject( "SmallCrate", Vec3( 51, -42, 37 ) );
    synapse.addObject( "SmallCrate", Vec3( 51, -42, 38 ) );

    synapse.addObject( "SmallCrate", Vec3( 51, -41, 36 ) );
    synapse.addObject( "SmallCrate", Vec3( 51, -41, 37 ) );
    synapse.addObject( "SmallCrate", Vec3( 51, -41, 38 ) );

    synapse.addObject( "SmallCrate", Vec3( 52, -42, 36 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -42, 37 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -42, 38 ) );

    synapse.addObject( "SmallCrate", Vec3( 52, -41, 36 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -41, 37 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -41, 38 ) );

    synapse.addObject( "BigCrate", Vec3( 54, -43, 36 ) );

    synapse.addObject( "BigCrate", Vec3( 50, -55, 36 ) );
    synapse.addObject( "BigCrate", Vec3( 50, -55, 38 ) );
    synapse.addObject( "BigCrate", Vec3( 50, -55, 40 ) );

    synapse.addStruct( "pool", Vec3( 50, -60, 37 ), Structure::R0 );
    synapse.addStruct( "test", Vec3( 60, -60, 38 ), Structure::R0 );

    synapse.addObject( "SmallCrate", Vec3( 52, -61, 40 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -61, 41 ) );
    synapse.addObject( "SmallCrate", Vec3( 52, -61, 42 ) );
  }

  void Matrix::init()
  {
    log.println( "Initializing Matrix {" );
    log.indent();

    semaphore = SDL_CreateSemaphore( 0 );

    translator.init();
    lua.init();
    world.init();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Freeing Matrix {" );
    log.indent();

    world.free();
    lua.free();
    translator.free();

    SDL_DestroySemaphore( semaphore );

    log.unindent();
    log.println( "}" );
  }

  void Matrix::load( InputStream *istream )
  {
    log.println( "Loading Matrix {" );
    log.indent();

    world.terra.load( "terra/heightmap.png" );
    world.load();

    if( istream != null ) {
      world.read( istream );
    }
    else {
      loadSample();
      loadStressTest();
      floraManager.seed();
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::unload( OutputStream *ostream )
  {
    log.println( "Unloading Matrix {" );
    log.indent();

    if( ostream != null ) {
      world.write( ostream );
    }
    world.unload();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::cleanObjects()
  {
    for( int i = 0; i < world.objects.length(); i++ ) {
      Object *obj = world.objects[i];

      if( obj != null ) {
        obj->events.free();

        if( ( obj->flags & Object::DESTROYED_BIT ) && obj->cell != null ) {
          synapse.remove( obj );
        }
      }
    }
  }

  void Matrix::update()
  {
    for( int i = 0; i < world.particles.length(); i++ ) {
      Particle *part = world.particles[i];

      if( part != null ) {
        part->update();
        physics.updatePart( part );

        if( part->lifeTime <= 0.0f || part->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
          synapse.remove( part );
        }
      }
    }

    for( int i = 0; i < world.objects.length(); i++ ) {
      Object *obj = world.objects[i];

      if( obj == null ) {
        continue;
      }

      obj->update();

      // object might have removed itself within onUpdate()
      if( obj->index == -1 ) {
        continue;
      }

      if( obj->flags & Object::DYNAMIC_BIT ) {
        Dynamic *dyn = static_cast<Dynamic*>( obj );

        if( dyn->cell == null ) {
          assert( dyn->parent != -1 );

          // remove if its container has been removed
          if( world.objects[dyn->parent] == null ) {
            dyn->parent = -1;
            synapse.removeCut( dyn );
          }
        }
        else {
          physics.updateObj( dyn );

          // remove on velocity overflow
          if( dyn->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
            synapse.remove( obj );
          }
        }
      }

      if( obj->life <= 0.0f ) {
        obj->destroy();
      }
    }

    // rotate freeing/waiting/available indices
    world.update();
  }

}
