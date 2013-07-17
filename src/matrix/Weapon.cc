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
 * @file matrix/Weapon.cc
 */

#include <matrix/Weapon.hh>

#include <common/Timer.hh>
#include <matrix/Bot.hh>
#include <matrix/LuaMatrix.hh>
#include <matrix/Synapse.hh>

namespace oz
{

Pool<Weapon, 2048> Weapon::pool;

void Weapon::onUpdate()
{
  if( shotTime > 0.0f ) {
    shotTime = max( shotTime - Timer::TICK_TIME, 0.0f );
  }

  if( ( flags & LUA_BIT ) && !clazz->onUpdate.isEmpty() ) {
    luaMatrix.objectCall( clazz->onUpdate, this );
  }

  if( !( flags & Object::UPDATE_FUNC_BIT ) ) {
    // actually a hack, if Lua handler disables update
    shotTime = 0.0f;
  }
}

bool Weapon::onUse( Bot* user )
{
  hard_assert( user->canEquip( this ) );

  if( parent == user->index ) {
    user->weapon = user->weapon == index ? -1 : index;
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
      Object* container = orbis.obj( parent );

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
  hard_assert( user != nullptr );

  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  if( shotTime == 0.0f ) {
    bool success = false;

    shotTime = clazz->shotInterval;

    if( nRounds != 0 && luaMatrix.objectCall( clazz->onShot, this, user ) ) {
      nRounds = max( -1, nRounds - 1 );
      success = true;
    }
    addEvent( EVENT_SHOT_EMPTY + success, 1.0f );
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

Weapon::Weapon( const WeaponClass* clazz_, const JSON& json ) :
  Dynamic( clazz_, json )
{
  nRounds  = json["nRounds"].asInt();
  shotTime = json["shotTime"].asFloat();
}

void Weapon::write( OutputStream* ostream ) const
{
  Dynamic::write( ostream );

  ostream->writeInt( nRounds );
  ostream->writeFloat( shotTime );
}

JSON Weapon::write() const
{
  JSON json = Dynamic::write();

  json.add( "nRounds", nRounds );
  json.add( "shotTime", shotTime );

  return json;
}

void Weapon::readUpdate( InputStream* )
{}

void Weapon::writeUpdate( OutputStream* ) const
{}

}
