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

#include "Bot.h"

namespace oz
{

  void Weapon::onUse( Bot *user )
  {
    assert( parent == -1 || parent == user->index );

    user->weaponItem = user->weaponItem == index ? -1 : index;
  }

}
