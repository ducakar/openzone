/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file matrix/Weapon.cc
 */

#include "stable.hh"

#include "matrix/Weapon.hh"

#include "matrix/Bot.hh"
#include "matrix/Lua.hh"
#include "matrix/Synapse.hh"

namespace oz
{
namespace matrix
{

Pool<Weapon, 2048> Weapon::pool;

void Weapon::onUpdate()
{
  if( shotTime > 0.0f ) {
    shotTime = max( shotTime - Timer::TICK_TIME, 0.0f );
  }

  if( ( flags & LUA_BIT ) && !clazz->onUpdate.isEmpty() ) {
    lua.objectCall( clazz->onUpdate, this );
  }

  if( !( flags & Object::UPDATE_FUNC_BIT ) ) {
    // actually a hack, if Lua handler disables update
    shotTime = 0.0f;
  }
}

bool Weapon::onUse( Bot* user )
{
  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  if( !user->clazz->name.beginsWith( clazz->userBase ) ) {
    hard_assert( user->weapon != index );
    return false;
  }

  if( parent == user->index ) {
    user->weapon = user->weapon == index  ? -1 : index;
    return true;
  }
  else if( user->items.length() < user->clazz->nItems ) {
    user->items.add( index );
    user->weapon = index;

    if( parent < 0 ) {
      parent = user->index;
      synapse.cut( this );
    }
    else {
      Object* container = orbis.objects[parent];

      hard_assert( container->items.contains( index ) );

      parent = user->index;
      container->items.exclude( index );
    }
    return true;
  }
  return false;
}

void Weapon::trigger( Bot* user )
{
  hard_assert( user != null );

  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  if( shotTime == 0.0f ) {
    shotTime = clazz->shotInterval;

    if( nRounds == 0 ) {
      addEvent( EVENT_SHOT_EMPTY, 1.0f );
    }
    else {
      nRounds = max( -1, nRounds - 1 );

      addEvent( EVENT_SHOT, 1.0f );
      lua.objectCall( clazz->onShot, this, user );
    }
  }
}

Weapon::Weapon( const WeaponClass* clazz_, int index_, const Point& p_, Heading heading ) :
  Dynamic( clazz_, index_, p_, heading )
{
  nRounds  = clazz_->nRounds;
  shotTime = 0.0f;
}

Weapon::Weapon( const WeaponClass* clazz_, InputStream* istream ) :
  Dynamic( clazz_, istream )
{
  nRounds  = istream->readInt();
  shotTime = istream->readFloat();
}

void Weapon::write( BufferStream* ostream ) const
{
  Dynamic::write( ostream );

  ostream->writeInt( nRounds );
  ostream->writeFloat( shotTime );
}

void Weapon::readUpdate( InputStream* )
{}

void Weapon::writeUpdate( BufferStream* ) const
{}

}
}
