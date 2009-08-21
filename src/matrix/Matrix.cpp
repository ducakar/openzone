/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Matrix.h"

#include "Translator.h"
#include "Physics.h"

namespace oz
{

  Matrix matrix;

  const float Matrix::G_ACCEL = -9.81f;
  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 10000.0f;

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
      synapse.put( new Structure( Vec3( 57, -33, 33 ), translator.bspIndex( "castle" ), Structure::R0 ) );

      synapse.put( translator.createObject( "Lord",   Vec3( 52, -44, 27 ) ) );
      synapse.put( translator.createObject( "Knight", Vec3( 50, -35, 27 ) ) );
      synapse.put( translator.createObject( "Goblin", Vec3( 51, -35, 27 ) ) );

      synapse.put( translator.createObject( "Raptor", Vec3( 70, -60, 30 ) ) );

//       world.genParticles( 1000, Vec3( 40, -42, 74 ), Vec3( 0, 0, 10 ), 15.0f, 1.95f, 0.1f, 5.0f,
//                           0.1f, Vec3( 0.4f, 0.4f, 0.4f ), 0.2f );
      synapse.put( translator.createObject( "MetalBarrel", Vec3( 61, -44, 26 ) ) );
      synapse.put( translator.createObject( "MetalBarrel", Vec3( 61, -44, 28 ) ) );
      synapse.put( translator.createObject( "MetalBarrel", Vec3( 61, -44, 30 ) ) );

      synapse.put( translator.createObject( "Tree1",
                   Vec3( 0, -30, world.terra.height( 0, -30 ) + 5 ) ) );
      synapse.put( translator.createObject( "Tree3",
                   Vec3( 0, -42, world.terra.height( 0, -30 ) + 5 ) ) );

      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -42, 26 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -42, 27 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -42, 28 ) ) );

      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -41, 26 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -41, 27 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 51, -41, 28 ) ) );

      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -42, 26 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -42, 27 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -42, 28 ) ) );

      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -41, 26 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -41, 27 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -41, 28 ) ) );

      synapse.put( translator.createObject( "BigCrate", Vec3( 54, -43, 26 ) ) );

      synapse.put( translator.createObject( "BigCrate", Vec3( 50, -55, 26 ) ) );
      synapse.put( translator.createObject( "BigCrate", Vec3( 50, -55, 28 ) ) );
      synapse.put( translator.createObject( "BigCrate", Vec3( 50, -55, 30 ) ) );

      synapse.put( new Structure( Vec3( 50, -60, 25 ), translator.bspIndex( "pool" ), Structure::R0 ) );
      synapse.put( new Structure( Vec3( 60, -60, 26 ), translator.bspIndex( "test" ), Structure::R0 ) );

      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -61, 30 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -61, 31 ) ) );
      synapse.put( translator.createObject( "SmallCrate", Vec3( 52, -61, 32 ) ) );

      world.commitAll();
      synapse.clear();
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
    buffer.write( config.get( "dir.home", "" ) + String( "/saved.world" ) );

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

    for( int j = 0; j < world.objects.length(); j++ ) {
      Object *obj = world.objects[j];

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
    world.commitPlus();
  }

  void Matrix::sync()
  {
    world.commitMinus();
  }

}
