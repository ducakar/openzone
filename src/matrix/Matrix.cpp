/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Matrix.h"

#include "Physics.h"
#include "FloraManager.h"

namespace oz
{

  Matrix matrix;

  const float Matrix::G_ACCEL = -9.81f;
  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 10000.0f;

  void Matrix::loadStressTest()
  {
    static const float DIM = World::DIM;
//    static const float DIM = 100;

    for( int i = 0; i < 400; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      synapse.addObject( "Goblin", Vec3( x, y, z ) );
    }
    for( int i = 0; i < 1000; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 1.0f;

      synapse.addObject( "BigCrate", Vec3( x, y, z ) );
    }
    for( int i = 0; i < 1000; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 2.0f;

      synapse.addObject( "SmallCrate", Vec3( x, y, z ) );
    }
  }

  void Matrix::load()
  {
    log.println( "Loading Matrix {" );
    log.indent();

    semaphore = SDL_CreateSemaphore( 0 );

    translator.init();
    world.init();
    physics.setG( G_ACCEL );

    world.sky.init( 70.0f, 600.0f );
    world.sky.time = 200.0f;
    world.terra.load( "terra/heightmap.png" );

    Buffer buffer( 1024 * 1024 * 10 );
    if( buffer.load( config.get( "dir.home", "" ) + String( "/saved.world" ) ) ) {
      InputStream istream = buffer.inputStream();
      world.read( &istream );
    }
    else {
//      Bot *lord = (Bot*) translator.createObject( "Lord", Vec3( 52, -44, 37 ) );
//      lord->h = 270;
//      synapse.put( lord );

      synapse.addObject( "Knight", Vec3( 50, -35, 37 ) );
      synapse.addObject( "Goblin", Vec3( 51, -35, 37 ) );

      synapse.addObject( "FirstAid", Vec3( 50, -36, 37 ) );

//      synapse.addObject( "Raptor", Vec3( 70, -60, 40 ) );

      synapse.addStruct( "castle", Vec3( 57, -33, 43 ), Structure::R0 );

//      world.genParticles( 1000, Vec3( 40, -42, 74 ), Vec3( 0, 0, 10 ), 15.0f, 1.95f, 0.1f, 5.0f,
//                          0.1f, Vec3( 0.4f, 0.4f, 0.4f ), 0.2f );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 36 ) );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 38 ) );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 40 ) );

//      synapse.addObject( "Tree2",  Vec3( 0, 80, world.terra.height( 0, 80 ) + 3.5f ) );
      synapse.addObject( "Tree3", Vec3( 77.8, -40, world.terra.height( 78, -40 ) + 6.5f ) );

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

      loadStressTest();
      floraManager.seed();
    }
    buffer.free();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Shutting down Matrix {" );
    log.indent();

    Buffer buffer( 1024 * 1024 * 10 );
    OutputStream ostream = buffer.outputStream();
    world.write( &ostream );
//     buffer.write( config.get( "dir.home", "" ) + String( "/saved.world" ) );

    world.free();
    translator.free();

    SDL_DestroySemaphore( semaphore );

    log.unindent();
    log.println( "}" );
  }

  void Matrix::update()
  {
    physics.update();

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

      if( obj != null ) {
        obj->update();

        if( obj->flags & Object::DYNAMIC_BIT ) {
          DynObject *dynObj = (DynObject*) obj;

          physics.updateObj( dynObj );

          if( dynObj->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
            synapse.remove( obj );
            continue;
          }
        }
        if( obj->life <= 0.0f || !world.includes( *obj ) ) {
          synapse.remove( obj );
        }
      }
    }

    world.update();
  }

}
