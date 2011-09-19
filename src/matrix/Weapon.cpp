/*
 *  Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Weapon.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Bot.hpp"
#include "matrix/WeaponClass.hpp"
#include "matrix/Lua.hpp"

namespace oz
{

  Pool<Weapon, 1024> Weapon::pool;

  void Weapon::onUpdate()
  {
    if( shotTime > 0.0f ) {
      shotTime = max( shotTime - Timer::TICK_TIME, 0.0f );
    }
  }

  void Weapon::onUse( Bot* user )
  {
    hard_assert( parent == -1 || parent == user->index );

    if( parent == user->index ) {
      user->weaponItem = user->weaponItem == index  ? -1 : index;
    }
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

      if( nShots == 0 ) {
        addEvent( EVENT_SHOT_EMPTY, 1.0f );
      }
      else {
        nShots = max( -1, nShots - 1 );

        addEvent( EVENT_SHOT, 1.0f );
        lua.objectCall( clazz->onShot, this, user );
      }
    }
  }

  void Weapon::readFull( InputStream* istream )
  {
    Dynamic::readFull( istream );

    nShots   = istream->readInt();
    shotTime = istream->readFloat();
  }

  void Weapon::writeFull( OutputStream* ostream ) const
  {
    Dynamic::writeFull( ostream );

    ostream->writeInt( nShots );
    ostream->writeFloat( shotTime );
  }

  void Weapon::readUpdate( InputStream* istream )
  {
    Dynamic::readUpdate( istream );

    nShots = istream->readInt();
  }

  void Weapon::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeInt( nShots );
  }

}
