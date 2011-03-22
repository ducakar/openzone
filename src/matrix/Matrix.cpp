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
#include "matrix/Collider.hpp"
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
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "Goblin" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "Goblin", aabb.p );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "Knight" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "Knight", aabb.p );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "Beast" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "Beast", aabb.p );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "Droid" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "Droid", aabb.p );
      }
    }
    for( int i = 0; i < 5000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "MetalBarrel" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "MetalBarrel", aabb.p );
      }
    }
    for( int i = 0; i < 5000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "SmallCrate" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "SmallCrate", aabb.p );
      }
    }
  }

  void Matrix::loadSample()
  {
    orbis.sky.heading = Math::TAU / 16;
    orbis.sky.period = 1440.0f;
    orbis.sky.time = 180.0f;

    int index = synapse.addObject( "Lord", Point3( 52, -44, 37 ) );
    static_cast<Bot*>( orbis.objects[index] )->h = 0.75f * Math::TAU;

    index = synapse.addObject( "DroidCommander", Point3( 54, -44, 37 ) );
    static_cast<Bot*>( orbis.objects[index] )->h = 0.25f * Math::TAU;

    synapse.addObject( "Knight", Point3( 50, -35, 37 ) );
    synapse.addObject( "Goblin", Point3( 51, -35, 37 ) );

    synapse.addObject( "Wine", Point3( 49, -36, 37 ) );
    synapse.addObject( "FirstAid", Point3( 50, -36, 37 ) );
    synapse.addObject( "DroidRifle", Point3( 51, -36, 37 ) );

    synapse.addObject( "Raptor", Point3( 70, -60, 38 ) );
    synapse.addObject( "Tank", Point3( 64, -60, 38 ) );

    synapse.genParts( 1000, Point3( 50, -36, 35 ), Vec3( 0, 0, 10 ), 15.0f,
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

    synapse.addObject( "BigCrate", Point3( 54, -40, 36 ) );

    synapse.addObject( "BigCrate", Point3( 50, -55, 36 ) );
    synapse.addObject( "BigCrate", Point3( 50, -55, 38 ) );
    synapse.addObject( "BigCrate", Point3( 50, -55, 40 ) );

    synapse.addStruct( "castle", Point3( 57, -33, 43 ), Struct::R0 );
    synapse.addStruct( "pool", Point3( 26, -40, 35 ), Struct::R0 );
    synapse.addStruct( "bunker", Point3( 20, -40, 40 ), Struct::R90 );

    synapse.addObject( "SmallCrate", Point3( 22, -39, 45 ) );
    synapse.addObject( "SmallCrate", Point3( 22, -39, 46 ) );
    synapse.addObject( "SmallCrate", Point3( 22, -39, 47 ) );
    synapse.addObject( "SmallCrate", Point3( 22, -41, 45 ) );
    synapse.addObject( "SmallCrate", Point3( 22, -41, 46 ) );
    synapse.addObject( "SmallCrate", Point3( 22, -41, 47 ) );
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
        // sound and other effects can be played on an object's destruction.
        if( obj->flags & Object::DESTROYED_BIT ) {
          if( obj->cell == null ) {
            hard_assert( obj->flags & Object::DYNAMIC_BIT );

            synapse.removeCut( static_cast<Dynamic*>( obj ) );
          }
          else {
            synapse.remove( obj );
          }
        }
      }
    }

    for( int i = 0; i < orbis.structs.length(); ++i ) {
      Struct* str = orbis.structs[i];

      if( str == null ) {
        continue;
      }

      str->update();

      if( str->life <= 0.0f ) {
        str->destroy();
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
          hard_assert( dyn->parent != -1 );

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

    // rotate freeing/waiting/available indices
    orbis.update();
  }

  void Matrix::load( InputStream* istream )
  {
    log.println( "Loading Matrix {" );
    log.indent();

    orbis.terra.load( 0 );
    orbis.load();
    synapse.load();

    if( istream != null ) {
      orbis.read( istream );
    }
    else {
      loadSample();
      floraManager.seed();
      loadStressTest();
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

    synapse.unload();
    orbis.unload();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::init()
  {
    log.println( "Initialising Matrix {" );
    log.indent();

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

    log.unindent();
    log.println( "}" );
  }

}
