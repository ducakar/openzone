/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Weapon.cpp
 */

#include "stable.hpp"

#include "matrix/Weapon.hpp"

#include "matrix/WeaponClass.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Synapse.hpp"

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
  hard_assert( parent == -1 || parent == user->index );

  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  if( !clazz->allowedUsers.contains( user->clazz ) ) {
    hard_assert( user->weapon != index );
    return false;
  }

  if( parent == -1 && user->items.length() < user->clazz->nItems ) {
    user->items.add( index );
    parent = user->index;
    synapse.cut( this );

    user->weapon = index;

    return true;
  }
  else if( parent == user->index ) {
    user->weapon = user->weapon == index  ? -1 : index;

    return true;
  }
  return false;
}

Weapon::Weapon()
{}

void Weapon::trigger( Bot* user )
{
  hard_assert( user != null );

  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  if( clazz->onShot.isEmpty() ) {
    return;
  }

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

void Weapon::readFull( InputStream* istream )
{
  Dynamic::readFull( istream );

  nRounds  = istream->readInt();
  shotTime = istream->readFloat();
}

void Weapon::writeFull( BufferStream* ostream ) const
{
  Dynamic::writeFull( ostream );

  ostream->writeInt( nRounds );
  ostream->writeFloat( shotTime );
}

void Weapon::readUpdate( InputStream* istream )
{
  Dynamic::readUpdate( istream );

  nRounds = istream->readInt();
}

void Weapon::writeUpdate( BufferStream* ostream ) const
{
  Dynamic::writeUpdate( ostream );

  ostream->writeInt( nRounds );
}

}
}
