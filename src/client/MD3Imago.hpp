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
 * @file client/MD3Imago.hpp
 */

#pragma once

#include "client/Imago.hpp"
#include "client/MD3.hpp"

namespace oz
{
namespace client
{

class MD3Imago : public Imago
{
  public:

    static const float TURN_SMOOTHING_COEF;

    MD3*           md3;
    float          h;
//       MD3::AnimState anim;

    static Pool<MD3Imago, 256> pool;

  protected:

    MD3Imago()
    {
      flags |= Imago::MD2MODEL_BIT;
    }

    virtual ~MD3Imago();

  public:

    static Imago* create( const Object* obj );

//       void setAnim( Anim::Type anim );
    virtual void draw( const Imago* parent, int mask );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
