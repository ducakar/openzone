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

  void Matrix::load()
  {
    logFile.println( "Loading Matrix {" );
    logFile.indent();

    semaphore = SDL_CreateSemaphore( 0 );

    translator.init();
    world.init();
    physics.setG( G_ACCEL );

    world.sky.init( 70.0f, 600.0f );
    world.sky.time = 200.0f;
    world.terrain.loadIMG( "terra/heightmap.png" );

    synapse.put( new Structure( Vec3( 47.0f, -33.0f, 82.5f ), translator.bspIndex( "castle" ), Structure::R0 ) );

    synapse.put( translator.createObject( "Knight", Vec3( 42, -45, 80 ) ) );
    //world.add( new B_Spirit( Vec3( 40, -70, 90 ), 0.0f, 0.0f ) );

    synapse.put( translator.createObject( "Knight", Vec3( 40, -35, 85 ) ) );
    synapse.put( translator.createObject( "Goblin", Vec3( 41, -35, 85 ) ) );

//     world.genParticles( 1000, Vec3( 40, -42, 74 ), Vec3( 0, 0, 10 ), 15.0f, 1.95f, 0.1f, 5.0f,
//                        0.1f, Vec3( 0.4f, 0.4f, 0.4f ), 0.2f );
    synapse.put( translator.createObject( "MetalBarrel", Vec3( 51.0f, -44.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "MetalBarrel", Vec3( 51.0f, -44.0f, 82.0f ) ) );
    synapse.put( translator.createObject( "MetalBarrel", Vec3( 51.0f, -44.0f, 84.0f ) ) );
//
//     world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 80.0f ) ) );
//     world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 82.0f ) ) );
//     world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 84.0f ) ) );
//
    synapse.put( translator.createObject( "Tree1",
                 Vec3( 0.0f, -30.0f, world.terrain.height( 0.0f, -30.0f ) + 5.0f ) ) );
    synapse.put( translator.createObject( "Tree3",
                 Vec3( 0.0f, -42.0f, world.terrain.height( 0.0f, -30.0f ) + 5.0f ) ) );

    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -42.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -42.0f, 81.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -42.0f, 82.0f ) ) );

    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -41.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -41.0f, 81.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 41.0f, -41.0f, 82.0f ) ) );

    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -42.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -42.0f, 81.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -42.0f, 82.0f ) ) );

    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -41.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -41.0f, 81.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -41.0f, 82.0f ) ) );

    synapse.put( translator.createObject( "BigCrate", Vec3( 44.0f, -43.0f, 80.0f ) ) );

    synapse.put( translator.createObject( "BigCrate", Vec3( 40.0f, -55.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "BigCrate", Vec3( 40.0f, -55.0f, 82.0f ) ) );
    synapse.put( translator.createObject( "BigCrate", Vec3( 40.0f, -55.0f, 84.0f ) ) );

    synapse.put( new Structure( Vec3( 40.0f, -60.0f, 75.0f ), translator.bspIndex( "pool" ), Structure::R0 ) );
    synapse.put( new Structure( Vec3( 50.0f, -60.0f, 77.0f ), translator.bspIndex( "test" ), Structure::R0 ) );

    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -61.0f, 80.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -61.0f, 81.0f ) ) );
    synapse.put( translator.createObject( "SmallCrate", Vec3( 42.0f, -61.0f, 82.0f ) ) );

//     Buffer buffer;
//     buffer.load( "saved.world" );
//     InputStream istream = buffer.inputStream();
//     world.read( &istream );
//     buffer.free();

    logFile.unindent();
    logFile.println( "}" );
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

        if( part->lifeTime <= 0.0f || part->velocity.sqL() > Physics::MAX_VELOCITY2 ) {
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

          if( dynObj->velocity.sqL() > Physics::MAX_VELOCITY2 ) {
            synapse.remove( obj );
            continue;
          }
        }
        if( obj->damage <= 0.0f ) {
          synapse.remove( obj );
        }
      }
    }
    world.commit();
  }

  void Matrix::free()
  {
    logFile.print( "Shutting down Matrix ..." );

//     Buffer buffer( 1024 * 1024 );
//     OutputStream ostream = buffer.outputStream();
//     world.write( &ostream );
//     buffer.write( "saved.world" );

    world.free();
    translator.free();

    SDL_DestroySemaphore( semaphore );

    logFile.printEnd( " OK" );
  }

}
