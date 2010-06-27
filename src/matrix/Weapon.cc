/*
 *  Weapon.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "matrix/Weapon.hh"

#include "matrix/Timer.hh"
#include "matrix/Lua.hh"
#include "matrix/Bot.hh"
#include "matrix/WeaponClass.hh"

namespace oz
{

  Pool<Weapon> Weapon::pool;

  void Weapon::onUpdate()
  {
    if( shotTime > 0.0f ) {
      shotTime = Math::max( 0.0f, shotTime - Timer::TICK_TIME );
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
    const WeaponClass* clazz = static_cast<const WeaponClass*>( type );

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
