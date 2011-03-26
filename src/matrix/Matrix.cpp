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
  const float Matrix::MAX_VELOCITY2 = 1000000.0f;

  void Matrix::loadStressTest()
  {
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "goblin" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "goblin", aabb.p );
      }
    }
    for( int i = 0; i < 500; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "knight" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "knight", aabb.p );
      }
    }
    for( int i = 0; i < 1000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "beast" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "beast", aabb.p );
      }
    }
    for( int i = 0; i < 10000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "metalBarrel" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "metalBarrel", aabb.p );
      }
    }
    for( int i = 0; i < 10000; ++i ) {
      float x = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float y = -Orbis::DIM + 2.0f * Orbis::DIM * Math::frand();
      float z = orbis.terra.height( x, y );

      const ObjectClass* const* value = translator.classes.find( "smallCrate" );
      Point3 p = Point3( x, y, z );
      AABB aabb = AABB( p, ( *value )->dim );

      aabb.p.z += aabb.dim.z + 0.1f;

      if( z > 0.0f && !collider.overlaps( aabb ) ) {
        synapse.addObject( "smallCrate", aabb.p );
      }
    }
  }

  void Matrix::loadSample()
  {
    orbis.sky.heading = Math::TAU / 16;
    orbis.sky.period = 1440.0f;
    orbis.sky.time = 180.0f;

    int index = synapse.addObject( "lord", Point3( 138, -11, 73 ) );
    static_cast<Bot*>( orbis.objects[index] )->h = 0.75f * Math::TAU;

    index = synapse.addObject( "droidCommander", Point3( 140, -11, 73 ) );
    static_cast<Bot*>( orbis.objects[index] )->h = 0.25f * Math::TAU;

    synapse.addObject( "cvicek", Point3( 135, -3, 73 ) );
    synapse.addObject( "firstAid", Point3( 136, -3, 73 ) );
    synapse.addObject( "droidRifle", Point3( 137, -3, 73 ) );

    synapse.addObject( "raptor", Point3( 155, -30, 70 ) );
    synapse.addObject( "tank", Point3( 145, -30, 71 ) );

    synapse.genParts( 1000, Point3( 136, -3, 71 ), Vec3( 0, 0, 10 ), 15.0f,
                      Vec3( 0.4f, 0.4f, 0.4f ), 0.2f, 1.95f, 0.1f, 5.0f );

    synapse.addObject( "metalBarrel", Point3( 147, -11, 72 ) );
    synapse.addObject( "metalBarrel", Point3( 147, -11, 74 ) );
    synapse.addObject( "metalBarrel", Point3( 147, -11, 76 ) );

    synapse.addObject( "smallCrate", Point3( 147, 11, 72 ) );
    synapse.addObject( "smallCrate", Point3( 147, 11, 73 ) );
    synapse.addObject( "smallCrate", Point3( 147, 11, 74 ) );
    synapse.addObject( "smallCrate", Point3( 147, 11, 75 ) );
    synapse.addObject( "smallCrate", Point3( 147, 11, 76 ) );

    synapse.addObject( "smallCrate", Point3( 147, 12, 72 ) );
    synapse.addObject( "smallCrate", Point3( 147, 12, 73 ) );
    synapse.addObject( "smallCrate", Point3( 147, 12, 74 ) );
    synapse.addObject( "smallCrate", Point3( 147, 12, 75 ) );
    synapse.addObject( "smallCrate", Point3( 147, 12, 76 ) );

    synapse.addObject( "smallCrate", Point3( 148, 11, 72 ) );
    synapse.addObject( "smallCrate", Point3( 148, 11, 73 ) );
    synapse.addObject( "smallCrate", Point3( 148, 11, 74 ) );
    synapse.addObject( "smallCrate", Point3( 148, 11, 75 ) );
    synapse.addObject( "smallCrate", Point3( 148, 11, 76 ) );

    synapse.addObject( "smallCrate", Point3( 148, 12, 72 ) );
    synapse.addObject( "smallCrate", Point3( 148, 12, 73 ) );
    synapse.addObject( "smallCrate", Point3( 148, 12, 74 ) );
    synapse.addObject( "smallCrate", Point3( 148, 12, 75 ) );
    synapse.addObject( "smallCrate", Point3( 148, 12, 76 ) );

    synapse.addObject( "bigCrate", Point3( 140, -7, 72 ) );

    synapse.addStruct( "castle", Point3( 143, 0, 79 ), Struct::R0 );
    synapse.addStruct( "pool", Point3( 112, -7, 70 ), Struct::R0 );
    synapse.addStruct( "bunker", Point3( 106, -7, 75 ), Struct::R90 );

    synapse.addObject( "serviceStation", Point3( 110, -25, 68.8f ) );

    synapse.addObject( "smallCrate", Point3( 108, -6, 81 ) );
    synapse.addObject( "smallCrate", Point3( 108, -6, 82 ) );
    synapse.addObject( "smallCrate", Point3( 108, -6, 83 ) );
    synapse.addObject( "smallCrate", Point3( 108, -8, 81 ) );
    synapse.addObject( "smallCrate", Point3( 108, -8, 82 ) );
    synapse.addObject( "smallCrate", Point3( 108, -8, 83 ) );
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
