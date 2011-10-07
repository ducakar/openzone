/*
 *  Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Weapon.hpp"

#include "matrix/WeaponClass.hpp"
#include "matrix/Timer.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{

  Pool<Weapon, 1024> Weapon::pool;

  void Weapon::onUpdate()
  {
    if( shotTime > 0.0f ) {
      shotTime = max( shotTime - Timer::TICK_TIME, 0.0f );
    }

    if( ( flags & LUA_BIT ) && !clazz->onUpdate.isEmpty() ) {
      lua.objectCall( clazz->onUpdate, this );
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

  void Weapon::writeFull( OutputStream* ostream ) const
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

  void Weapon::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeInt( nRounds );
  }

}
