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

namespace oz
{

  Matrix matrix;

  const float Matrix::G_ACCEL = -9.81f;
  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 10000.0f;

  void Matrix::loadStressTest()
  {
    static const float DIM = World::DIM;
//     static const float DIM = 100;

    for( int i = 0; i < 4000; i++ ) {
      float x = -DIM + 2.0f * DIM * Math::frand();
      float y = -DIM + 2.0f * DIM * Math::frand();
      float z = world.terra.height( x, y ) + 400.0f;

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
    world.terra.load( "terra/large.png" );

    Buffer buffer( 1024 * 1024 * 10 );
    if( buffer.load( config.get( "dir.home", "" ) + String( "/saved.world" ) ) ) {
      InputStream istream = buffer.inputStream();
      world.read( &istream );
    }
    else {
      synapse.addStruct( "castle", Vec3( 57, -33, 33 ), Structure::R0 );

      Bot *lord = (Bot*) translator.createObject( "Lord", Vec3( 52, -44, 27 ) );
      lord->h = 270;
      synapse.put( lord );

      synapse.addObject( "Knight", Vec3( 50, -35, 27 ) );
      synapse.addObject( "Goblin", Vec3( 51, -35, 27 ) );

      synapse.addObject( "Raptor", Vec3( 70, -60, 30 ) );

//       world.genParticles( 1000, Vec3( 40, -42, 74 ), Vec3( 0, 0, 10 ), 15.0f, 1.95f, 0.1f, 5.0f,
//                           0.1f, Vec3( 0.4f, 0.4f, 0.4f ), 0.2f );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 26 ) );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 28 ) );
      synapse.addObject( "MetalBarrel", Vec3( 61, -44, 30 ) );

      synapse.addObject( "Tree2",  Vec3( 0, 80, world.terra.height( 0, 80 ) + 3.5f ) );
      synapse.addObject( "Tree3", Vec3( 10, 85, world.terra.height( 0, 85 ) + 6.5f ) );

      synapse.addObject( "SmallCrate", Vec3( 51, -42, 26 ) );
      synapse.addObject( "SmallCrate", Vec3( 51, -42, 27 ) );
      synapse.addObject( "SmallCrate", Vec3( 51, -42, 28 ) );

      synapse.addObject( "SmallCrate", Vec3( 51, -41, 26 ) );
      synapse.addObject( "SmallCrate", Vec3( 51, -41, 27 ) );
      synapse.addObject( "SmallCrate", Vec3( 51, -41, 28 ) );

      synapse.addObject( "SmallCrate", Vec3( 52, -42, 26 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -42, 27 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -42, 28 ) );

      synapse.addObject( "SmallCrate", Vec3( 52, -41, 26 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -41, 27 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -41, 28 ) );

      synapse.addObject( "BigCrate", Vec3( 54, -43, 26 ) );

      synapse.addObject( "BigCrate", Vec3( 50, -55, 26 ) );
      synapse.addObject( "BigCrate", Vec3( 50, -55, 28 ) );
      synapse.addObject( "BigCrate", Vec3( 50, -55, 30 ) );

      synapse.addStruct( "pool", Vec3( 50, -60, 25 ), Structure::R0 );
      synapse.addStruct( "test", Vec3( 60, -60, 26 ), Structure::R0 );

      synapse.addObject( "SmallCrate", Vec3( 52, -61, 30 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -61, 31 ) );
      synapse.addObject( "SmallCrate", Vec3( 52, -61, 32 ) );

      loadStressTest();
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

    int iMax = world.particles.length();
    for( int i = 0; i < iMax; i++ ) {
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
