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
#include "Water.h"

#include "O_Tree1.h"
#include "O_Tree2.h"

#include "D_SmallCrate.h"
#include "D_BigCrate.h"
#include "D_WoodBarrel.h"
#include "D_MetalBarrel.h"

#include "B_Spirit.h"
#include "B_Human.h"
#include "B_Goblin.h"

namespace oz
{

  Matrix matrix;

  const float Matrix::G_ACCEL = -9.81f;

  void Matrix::load()
  {
    semaphore = SDL_CreateSemaphore( 0 );

    translator.init();

    physics.init( G_ACCEL );

    world.sky.init( 70.0f, 600.0f );
    world.sky.time = 200.0f;
    world.terrain.loadIMG( "terra/heightmap.png" );

    world.add( new BSP() );
    world.add( new BSP() );
    world.add( new BSP() );
    world.add( new BSP() );

    world.bsps[0]->loadQBSP( "bsp/center.bsp", BSP::BSP_SCALE, 4.0f );
    world.bsps[1]->loadQBSP( "bsp/house.bsp", BSP::BSP_SCALE, 4.0f );
    world.bsps[2]->loadQBSP( "bsp/temple.bsp", BSP::BSP_SCALE, 4.0f );
    world.bsps[3]->loadQBSP( "bsp/pool.bsp", BSP::BSP_SCALE, 4.0f );

    world.add( new Structure( Vec3( 47.0f, -33.0f, 75.5f ), 0 ) );
    world.add( new Structure( Vec3( 33.0f, -34.0f, 75.5f ), 1 ) );
    world.add( new Structure( Vec3( 40.0f, -40.0f, 75.8f ), 2 ) );

    world.add( new B_Goblin( Vec3( 40, -70, 90 ), 0.0f, 0.0f ) );
    //world.add( new B_Spirit( Vec3( 40, -70, 90 ), 0.0f, 0.0f ) );

    world.add( new B_Goblin( Vec3( 40, -35, 85 ) ) );

    world.add( new B_Goblin( Vec3( 41, -35, 85 ) ) );

    world.genParticles( 1000, Vec3( 40, -42, 74 ), Vec3( 0, 0, 10 ), 15.0f, 1.95f, 0.1f, 5.0f,
                       0.1f, Vec3( 0.4f, 0.4f, 0.4f ), 0.2f );
    world.add( new D_MetalBarrel( Vec3( 51.0f, -42.0f, 80.0f ) ) );
    world.add( new D_MetalBarrel( Vec3( 51.0f, -42.0f, 82.0f ) ) );
    world.add( new D_MetalBarrel( Vec3( 51.0f, -42.0f, 84.0f ) ) );

    world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 80.0f ) ) );
    world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 82.0f ) ) );
    world.add( new D_WoodBarrel( Vec3( 51.0f, -38.0f, 84.0f ) ) );

    world.add( new D_SmallCrate( Vec3( 41.0f, -41.0f, 80.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 41.0f, -41.0f, 81.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 41.0f, -41.0f, 82.0f ) ) );

    world.add( new D_SmallCrate( Vec3( 41.0f, -42.0f, 80.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 41.0f, -42.0f, 81.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 41.0f, -42.0f, 82.0f ) ) );

    world.add( new D_SmallCrate( Vec3( 42.0f, -42.0f, 80.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -42.0f, 81.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -42.0f, 82.0f ) ) );

    world.add( new D_SmallCrate( Vec3( 42.0f, -41.0f, 80.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -41.0f, 81.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -40.0f, 82.0f ) ) );

    world.add( new D_BigCrate( Vec3( 40.0f, -55.0f, 80.0f ), new D_SmallCrate() ) );
    world.add( new D_BigCrate( Vec3( 40.0f, -55.0f, 82.0f ), new D_SmallCrate() ) );
    world.add( new D_BigCrate( Vec3( 40.0f, -55.0f, 84.0f ), new D_SmallCrate() ) );

    world.add( new O_Tree1( 0.0f, -30.0f ) );
    world.add( new O_Tree2( 0.0f, -42.0f ) );

    world.add( new Water( Vec3( -80.0f, 112.0f, 33.0f ), Vec3( 3.0f, 3.0f, 1.0f ) ) );
    world.add( new Water( Vec3( -86.0f, 112.0f, 33.0f ), Vec3( 3.0f, 3.0f, 1.0f ) ) );
    world.add( new Water( Vec3( -80.0f, 118.0f, 33.0f ), Vec3( 3.0f, 3.0f, 1.0f ) ) );

    world.add( new Structure( Vec3( 40.0f, -60.0f, 75.0f ), 3 ) );
    world.add( new Water( Vec3( 40.0f, -60.0f, 75.0f ), Vec3( 3.0f, 3.0f, 3.0f ) ) );

    world.add( new D_SmallCrate( Vec3( 42.0f, -61.0f, 81.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -61.0f, 82.0f ) ) );
    world.add( new D_SmallCrate( Vec3( 42.0f, -61.0f, 83.0f ) ) );
  }

  void Matrix::update()
  {
    world.beginUpdate();
    physics.beginUpdate();

    const int iMax = world.particles.length();
    for( int i = 0; i < iMax; i++ ) {
      Particle *part = world.particles[i];

      if( part != null ) {
        part->update();
        physics.updatePart( part );

        if( part->lifeTime <= 0.0f || part->velocity.sqL() > Physics::MAX_VELOCITY2 ) {
          world.remove( part );
        }
      }
    }

    const int jMax = world.objects.length();
    for( int j = 0; j < jMax; j++ ) {
      Object *obj = world.objects[j];

      if( obj != null ) {
        obj->update();

        if( ( obj->flags & Object::DYNAMIC_BIT ) && obj->sector != null ) {
          DynObject *dynObj = (DynObject*) obj;

          physics.updateObj( dynObj );

          if( dynObj->newVelocity.sqL() > Physics::MAX_VELOCITY2 ) {
            world.remove( obj );
            continue;
          }
        }
        if( obj->damage <= 0.0f ) {
          obj->destroy();
        }
      }
    }

    physics.endUpdate();
    world.endUpdate();
  }

  void Matrix::free()
  {
    world.free();
    PoolAlloc<Effect, 0>::pool.free();
    PoolAlloc<Event, 0>::pool.free();
    translator.free();

    SDL_DestroySemaphore( semaphore );
  }

}
