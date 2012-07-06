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
 * @file client/ExplosionImago.hh
 */

#pragma once

#include "client/Imago.hh"
#include "client/SMM.hh"

namespace oz
{
namespace client
{

class ExplosionImago : public Imago
{
  protected:

    static int modelId;

    SMM* smm;
    uint startMicros;

    explicit ExplosionImago( const Object* obj ) :
      Imago( obj )
    {}

    ~ExplosionImago() override;

  public:

    static Pool<ExplosionImago, 64> pool;

    static Imago* create( const Object* obj );

    void draw( const Imago* parent ) override;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
}
