/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file matrix/Matrix.cc
 */

#include <stable.hh>
#include <matrix/Matrix.hh>

#include <common/Timer.hh>
#include <matrix/Lua.hh>
#include <matrix/NamePool.hh>
#include <matrix/Physics.hh>
#include <matrix/Synapse.hh>
#include <matrix/Vehicle.hh>

namespace oz
{
namespace matrix
{

// default 10000.0f: 100 m/s
const float Matrix::MAX_VELOCITY2 = 1000000.0f;

void Matrix::update()
{
  maxStructs  = max( maxStructs,  Struct::pool.length() );
  maxEvents   = max( maxEvents,   Object::Event::pool.length() );
  maxObjects  = max( maxObjects,  Object::pool.length() );
  maxDynamics = max( maxDynamics, Dynamic::pool.length() );
  maxWeapons  = max( maxWeapons,  Weapon::pool.length() );
  maxBots     = max( maxBots,     Bot::pool.length() );
  maxVehicles = max( maxVehicles, Vehicle::pool.length() );
  maxFrags    = max( maxFrags,    Frag::mpool.length() );

  for( int i = 0; i < orbis.objects.length(); ++i ) {
    Object* obj = orbis.objects[i];

    if( obj != nullptr ) {
      // If this is cleared on the object's update, we may also remove effects that were added
      // by other objects, updated before it.
      obj->events.free();

      // We don't remove objects as they get destroyed but on the next update, so the destruction
      // sound and other effects can be played on an object's destruction.
      if( obj->flags & Object::DESTROYED_BIT ) {
        synapse.remove( obj );
      }
    }
  }

  for( int i = 0; i < orbis.structs.length(); ++i ) {
    Struct* str = orbis.structs[i];

    if( str == nullptr ) {
      continue;
    }

    if( str->demolishing >= 1.0f ) {
      synapse.remove( str );
    }
    else if( str->life <= 0.0f && str->demolishing == 0.0f ) {
      str->destroy();
    }
    else {
      str->update();
    }
  }

  for( int i = 0; i < orbis.objects.length(); ++i ) {
    Object* obj = orbis.objects[i];

    if( obj == nullptr ) {
      continue;
    }

    if( obj->life <= 0.0f ) {
      obj->destroy();
    }
    else {
      // clear inventory of invalid references
      if( !obj->items.isEmpty() ) {
        for( int j = 0; j < obj->items.length(); ) {
          if( orbis.objects[ obj->items[j] ] == nullptr ) {
            obj->items.erase( j );
          }
          else {
            ++j;
          }
        }
      }

      obj->update();

      // objects should not remove themselves within onUpdate()
      hard_assert( orbis.objects[i] != nullptr );

      if( obj->flags & Object::DYNAMIC_BIT ) {
        Dynamic* dyn = static_cast<Dynamic*>( obj );

        hard_assert( ( dyn->parent >= 0 ) == ( dyn->cell == nullptr ) );
        hard_assert( dyn->parent == -1 || orbis.objects[dyn->parent] != nullptr );

        if( dyn->cell != nullptr ) {
          physics.updateObj( dyn );

          // remove on velocity overflow
          if( dyn->velocity.sqN() > Matrix::MAX_VELOCITY2 ) {
            synapse.remove( obj );
          }
        }
      }
    }
  }

  for( int i = 0; i < orbis.frags.length(); ++i ) {
    Frag* frag = orbis.frags[i];

    if( frag == nullptr ) {
      continue;
    }

    if( frag->life <= 0.0f || frag->velocity.sqN() > Matrix::MAX_VELOCITY2 ) {
      synapse.remove( frag );
    }
    else {
      frag->life -= Timer::TICK_TIME;
      physics.updateFrag( frag );
    }
  }

  // rotate freeing/waiting/available indices
  orbis.update();
}

void Matrix::read( InputStream* istream )
{
  Log::println( "Reading Matrix {" );
  Log::indent();

  orbis.read( istream );
  physics.gravity = istream->readFloat();

  Log::unindent();
  Log::println( "}" );
}

void Matrix::write( OutputStream* ostream ) const
{
  Log::print( "Writing Matrix ..." );

  orbis.write( ostream );
  ostream->writeFloat( physics.gravity );

  Log::printEnd( " OK" );
}

void Matrix::load()
{
  Log::print( "Loading Matrix ..." );

  maxStructs  = 0;
  maxEvents   = 0;
  maxObjects  = 0;
  maxDynamics = 0;
  maxWeapons  = 0;
  maxBots     = 0;
  maxVehicles = 0;
  maxFrags    = 0;

  orbis.load();
  synapse.load();

  physics.gravity = -9.81f;

  Log::printEnd( " OK" );
}

void Matrix::unload()
{
  Log::println( "Unloading Matrix {" );
  Log::indent();

  Log::println( "Static memory usage  %.2f MiB", float( sizeof( orbis ) ) / ( 1024.0f * 1024.0f ) );

  Log::println( "Peak instances {" );
  Log::indent();
  Log::println( "%6d  structures",      maxStructs );
  Log::println( "%6d  object events",   maxEvents );
  Log::println( "%6d  static objects",  maxObjects );
  Log::println( "%6d  dynamic objects", maxDynamics );
  Log::println( "%6d  weapon objects",  maxWeapons );
  Log::println( "%6d  bot objects",     maxBots );
  Log::println( "%6d  vehicle objects", maxVehicles );
  Log::println( "%6d  fragments",       maxFrags );
  Log::unindent();
  Log::println( "}" );

  synapse.unload();
  orbis.unload();

  Log::unindent();
  Log::println( "}" );
}

void Matrix::init()
{
  Log::println( "Initialising Matrix {" );
  Log::indent();

  lua.init();
  namePool.init();
  orbis.init();

  Log::unindent();
  Log::println( "}" );
}

void Matrix::destroy()
{
  Log::println( "Destroying Matrix {" );
  Log::indent();

  orbis.destroy();
  namePool.destroy();
  lua.destroy();

  Log::unindent();
  Log::println( "}" );
}

Matrix matrix;

}
}
