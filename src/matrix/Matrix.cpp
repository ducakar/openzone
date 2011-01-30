/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Matrix.hpp"

#include "matrix/Names.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Translator.hpp"
#include "matrix/Physics.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Vehicle.hpp"
#include "matrix/FloraManager.hpp"

namespace oz
{

  Matrix matrix;

  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 100000.0f;

  void Matrix::loadStressTest()
  {
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Goblin", Point3( x, y, z ) );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Knight", Point3( x, y, z ) );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Beast", Point3( x, y, z ) );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "Droid", Point3( x, y, z ) );
      }
    }
    for( int i = 0; i < 5000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 1.0f;

      if( z > 0.0f ) {
        synapse.addObject( "MetalBarrel", Point3( x, y, z ) );
      }
    }
    for( int i = 0; i < 5000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y ) + 2.0f;

      if( z > 0.0f ) {
        synapse.addObject( "SmallCrate", Point3( x, y, z ) );
      }
    }
  }

  void Matrix::loadSample()
  {
    orbis.sky.set( 205.0f, 1440.0f, 180.0f );

    int index = synapse.addObject( "Lord", Point3( 52, -44, 37 ) );
    static_cast<Bot*>( orbis.objects[index] )->h = 270.0f;

    synapse.addObject( "Knight", Point3( 50, -35, 37 ) );
    synapse.addObject( "Goblin", Point3( 51, -35, 37 ) );

    synapse.addObject( "Wine", Point3( 49, -36, 37 ) );
    synapse.addObject( "FirstAid", Point3( 50, -36, 37 ) );
    synapse.addObject( "Rifle", Point3( 51, -36, 37 ) );

    synapse.addObject( "Raptor", Point3( 70, -60, 38 ) );

    synapse.genParts( 1000, Point3( 50, -36, 40 ), Vec3( 0, 0, 10 ), 15.0f,
                      Vec3( 0.4f, 0.4f, 0.4f ), 0.2f, 1.95f, 0.1f, 5.0f );

    synapse.addObject( "MetalBarrel", Point3( 61, -44, 36 ) );
    synapse.addObject( "MetalBarrel", Point3( 61, -44, 38 ) );
    synapse.addObject( "MetalBarrel", Point3( 61, -44, 40 ) );

    synapse.addObject( "Tree3", Point3( 77.7f, -40.0f, orbis.terra.height( 78.0f, -40.0f ) + 6.5f ) );

    synapse.addObject( "SmallCrate", Point3( 61, -22, 36 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -22, 37 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -22, 38 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -22, 39 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -22, 40 ) );

    synapse.addObject( "SmallCrate", Point3( 61, -21, 36 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -21, 37 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -21, 38 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -21, 39 ) );
    synapse.addObject( "SmallCrate", Point3( 61, -21, 40 ) );

    synapse.addObject( "SmallCrate", Point3( 62, -22, 36 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -22, 37 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -22, 38 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -22, 39 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -22, 40 ) );

    synapse.addObject( "SmallCrate", Point3( 62, -21, 36 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -21, 37 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -21, 38 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -21, 39 ) );
    synapse.addObject( "SmallCrate", Point3( 62, -21, 40 ) );

    synapse.addObject( "BigCrate", Point3( 54, -43, 36 ) );

    synapse.addObject( "BigCrate", Point3( 50, -55, 36 ) );
    synapse.addObject( "BigCrate", Point3( 50, -55, 38 ) );
    synapse.addObject( "BigCrate", Point3( 50, -55, 40 ) );

    synapse.addStruct( "castle", Point3( 57, -33, 43 ), Structure::R0 );
    synapse.addStruct( "pool", Point3( 50, -60, 37 ), Structure::R0 );
    synapse.addStruct( "test", Point3( 60, -60, 38 ), Structure::R0 );
    synapse.addStruct( "door", Point3( 60, -60, 40 ), Structure::R270 );

    synapse.addObject( "SmallCrate", Point3( 52, -61, 40 ) );
    synapse.addObject( "SmallCrate", Point3( 52, -61, 41 ) );
    synapse.addObject( "SmallCrate", Point3( 52, -61, 42 ) );
  }

  void Matrix::update()
  {
    for( int i = 0; i < orbis.objects.length(); ++i ) {
      Object* obj = orbis.objects[i];

      if( obj != null ) {
        // If this is cleared on the object's update, we may also remove effects that were added
        // by other objects, updated before it.
        obj->events.free();

        // We don't remove objects as they get destroyed but on the next update, so the destroy
        // sound and ohter effects can be played on an object's destruction.
        if( ( obj->flags & Object::DESTROYED_BIT ) && obj->cell != null ) {
          synapse.remove( obj );
        }
      }
    }

    for( int i = 0; i < orbis.parts.length(); ++i ) {
      Particle* part = orbis.parts[i];

      if( part != null ) {
        part->update();
        physics.updatePart( part );

        if( part->lifeTime <= 0.0f || part->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
          synapse.remove( part );
        }
      }
    }

    for( int i = 0; i < orbis.objects.length(); ++i ) {
      Object* obj = orbis.objects[i];

      if( obj == null ) {
        continue;
      }

      obj->update();

      // object might have removed itself within onUpdate()
      if( obj->index == -1 ) {
        continue;
      }

      if( obj->flags & Object::DYNAMIC_BIT ) {
        Dynamic* dyn = static_cast<Dynamic*>( obj );

        if( dyn->cell == null ) {
          assert( dyn->parent != -1 );

          // remove if its container has been removed
          if( orbis.objects[dyn->parent] == null ) {
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

    for( int i = 0; i < orbis.structs.length(); ++i ) {
      Structure* str = orbis.structs[i];

      if( str == null ) {
        continue;
      }

      str->update();

      if( str->life <= 0.0f ) {
        str->destroy();
      }
    }

    // rotate freeing/waiting/available indices
    orbis.update();
  }

  void Matrix::load( InputStream* istream )
  {
    log.println( "Loading Matrix {" );
    log.indent();

    orbis.terra.load( "heightmap" );
    orbis.load();

    if( istream != null ) {
      orbis.read( istream );
    }
    else {
      loadSample();
      loadStressTest();
      floraManager.seed();
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::unload( OutputStream* ostream )
  {
    log.println( "Unloading Matrix {" );
    log.indent();

    if( ostream != null ) {
      orbis.write( ostream );
    }
    orbis.unload();

    Particle::pool.free();

    Object::Event::pool.free();
    Object::pool.free();
    Dynamic::pool.free();
    Weapon::pool.free();
    Bot::pool.free();
    Vehicle::pool.free();

    Structure::pool.free();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::init()
  {
    log.println( "Initialising Matrix {" );
    log.indent();

    translator.init();
    names.init();
    lua.init();
    orbis.init();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Freeing Matrix {" );
    log.indent();

    orbis.free();
    lua.free();
    names.free();
    translator.free();

    log.unindent();
    log.println( "}" );
  }

}
