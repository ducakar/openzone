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
          synapse.remove( obj );
        }
      }
    }

    for( int i = 0; i < orbis.structs.length(); ++i ) {
      Struct* str = orbis.structs[i];

      if( str == null ) {
        continue;
      }

      if( str->life <= 0.0f ) {
        str->destroy();
      }
      else {
        str->update();
      }
    }

    for( int i = 0; i < orbis.objects.length(); ++i ) {
      Object* obj = orbis.objects[i];

      if( obj == null ) {
        continue;
      }

      // clear inventory of invalid references
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
            synapse.remove( dyn );
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

      if( part == null ) {
        continue;
      }

      if( part->lifeTime <= 0.0f || part->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
        synapse.remove( part );
      }
      else {
        part->update();
        physics.updatePart( part );
      }
    }

    // rotate freeing/waiting/available indices
    orbis.update();
  }

  void Matrix::read( InputStream* istream )
  {
    log.println( "Reading Matrix {" );
    log.indent();

    orbis.read( istream );

    log.unindent();
    log.println( "}" );
  }

  void Matrix::write( OutputStream* ostream ) const
  {
    log.println( "Writing Matrix {" );
    log.indent();

    orbis.write( ostream );

    log.unindent();
    log.println( "}" );
  }

  void Matrix::load()
  {
    log.println( "Loading Matrix {" );
    log.indent();

    orbis.load();
    synapse.load();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::unload()
  {
    log.println( "Unloading Matrix {" );
    log.indent();

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

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Freeing Matrix {" );
    log.indent();

    orbis.free();
    namePool.free();
    lua.free();

    log.unindent();
    log.println( "}" );
  }

}
