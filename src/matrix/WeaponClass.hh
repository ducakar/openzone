/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file matrix/WeaponClass.hh
 */

#pragma once

#include "matrix/DynamicClass.hh"

namespace oz
{
namespace matrix
{

class WeaponClass : public DynamicClass
{
  public:

    Map<const ObjectClass*> allowedUsers;

    String onShot;

    int    nRounds;
    float  shotInterval;

    static ObjectClass* createClass();

    virtual void initClass( const Config* config, const Lingua* classLingua );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( InputStream* istream ) const;
};

}
}
