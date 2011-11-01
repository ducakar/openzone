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
 * @file matrix/Weapon.hpp
 */

#pragma once

#include "matrix/Dynamic.hpp"
#include "matrix/WeaponClass.hpp"

namespace oz
{
namespace matrix
{

class Weapon : public Dynamic
{
  protected:

    virtual void onUpdate();
    virtual bool onUse( Bot* user );

  public:

    static const int EVENT_SHOT       = 7;
    static const int EVENT_SHOT_EMPTY = 8;

    static Pool<Weapon, 2048> pool;

    // -1: unlimited
    int   nRounds;
    float shotTime;

    Weapon();

    void trigger( Bot* user );

    virtual void readFull( InputStream* istream );
    virtual void writeFull( OutputStream* ostream ) const;
    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( OutputStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
