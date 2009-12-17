/*
 *  Weapon.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Dynamic.h"

namespace oz
{

  class Weapon : public Dynamic
  {
    friend class Bot;

    protected:

      virtual void onUse( Bot *user );

    public:

      explicit Weapon() : Dynamic() {}

  };

}
