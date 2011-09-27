/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Matrix.hpp"

#include "matrix/Library.hpp"
#include "matrix/NamePool.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Vehicle.hpp"

#include "matrix/modules/FloraModule.hpp"

#define OZ_REGISTER_MODULE( module ) \
  modules.add( &module##Module ); \
  module##Module.init()

namespace oz
{

  Matrix matrix;

  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 1000000.0f;

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

      // clear inventory of dead references
      for( int j = 0; j < obj->items.length(); ) {
        if( orbis.objects[ obj->items[j] ] == null ) {
          obj->items.remove( j );
        }
        else {
          ++j;
        }
      }

      obj->update();

      // object might have removed itself within onUpdate()
      if( orbis.objects[i] == null ) {
        continue;
      }
      else if( obj->life <= 0.0f ) {
        obj->destroy();
      }
      else if( obj->flags & Object::DYNAMIC_BIT ) {
        Dynamic* dyn = static_cast<Dynamic*>( obj );

        hard_assert( ( dyn->parent != -1 ) == ( dyn->cell == null ) );

        if( dyn->cell == null ) {
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

    for( int i = 0; i < modules.length(); ++i ) {
      modules[i]->update();
    }

    // rotate freeing/waiting/available indices
    orbis.update();
  }

  void Matrix::read( InputStream* istream )
  {
    log.println( "Reading Matrix {" );
    log.indent();

    if( istream != null ) {
      // to create the variable when running for the first time
      config.getSet( "matrix.onCreate", "matrix_onCreate" );

      orbis.read( istream );

      for( int i = 0; i < modules.length(); ++i ) {
        modules[i]->read( istream );
      }
    }
    else {
      lua.staticCall( config.getSet( "matrix.onCreate", "matrix_onCreate" ) );

      if( orbis.terra.id == -1 || orbis.caelum.id == -1 ) {
        throw Exception( "Terrain and Caelum must both be loaded in the matrix.onCreate method" );
      }
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::write( OutputStream* ostream ) const
  {
    log.println( "Writing Matrix {" );
    log.indent();

    orbis.write( ostream );

    for( int i = 0; i < modules.length(); ++i ) {
      modules[i]->write( ostream );
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::load()
  {
    log.println( "Loading Matrix {" );
    log.indent();

    orbis.load();
    synapse.load();

    for( int i = 0; i < modules.length(); ++i ) {
      modules[i]->load();
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::unload()
  {
    log.println( "Unloading Matrix {" );
    log.indent();


    for( int i = modules.length() - 1; i >= 0; --i ) {
      modules[i]->unload();
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

    lua.init();
    namePool.init();
    orbis.init();

    OZ_REGISTER_MODULE( flora );

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Freeing Matrix {" );
    log.indent();

    for( int i = modules.length() - 1; i >= 0; --i ) {
      modules[i]->free();
    }
    modules.clear();
    modules.dealloc();

    orbis.free();
    namePool.free();
    lua.free();

    log.unindent();
    log.println( "}" );
  }

}
