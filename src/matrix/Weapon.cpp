/*
 *  Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Weapon.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Bot.hpp"
#include "matrix/WeaponClass.hpp"

namespace oz
{

  Pool<Weapon> Weapon::pool;

  void Weapon::onUpdate()
  {
    if( shotTime > 0.0f ) {
      shotTime = Math::max( shotTime - Timer::TICK_TIME, 0.0f );
    }
  }

  void Weapon::onUse( Bot* user )
  {
    assert( parent == -1 || parent == user->index );

    if( parent == user->index ) {
      user->weaponItem = user->weaponItem == index  ? -1 : index;
    }
  }

  void Weapon::onShot( Bot* user )
  {
    const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

    if( !clazz->onShot.isEmpty() ) {
      lua.call( clazz->onShot, this, user );
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
