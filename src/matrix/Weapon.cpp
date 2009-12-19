/*
 *  Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Weapon.h"

#include "Timer.h"
#include "Lua.h"
#include "Bot.h"
#include "WeaponClass.h"

namespace oz
{

  void Weapon::onUpdate()
  {
    if( shotTime > 0.0f ) {
      shotTime = max( 0.0f, shotTime - timer.TICK_TIME );
    }
  }

  void Weapon::onUse( Bot *user )
  {
    assert( parent == -1 || parent == user->index );

    user->weaponItem = user->weaponItem == index ? -1 : index;
  }

  void Weapon::onShot( Bot *user )
  {
    const WeaponClass *clazz = static_cast<const WeaponClass*>( type );

    if( !clazz->onShot.isEmpty() ) {
      lua.call( clazz->onShot, this, user );
    }
  }

  void Weapon::readFull( InputStream *istream )
  {
    Dynamic::readFull( istream );

    nShots   = istream->readInt();
    shotTime = istream->readFloat();
  }

  void Weapon::writeFull( OutputStream *ostream ) const
  {
    Dynamic::writeFull( ostream );

    ostream->writeInt( nShots );
    ostream->writeFloat( shotTime );
  }

  void Weapon::readUpdate( InputStream *istream )
  {
    Dynamic::readUpdate( istream );

    nShots = istream->readInt();
  }

  void Weapon::writeUpdate( OutputStream *ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeInt( nShots );
  }

}
