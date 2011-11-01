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
 * @file client/MD2WeaponImago.cpp
 */

#include "stable.hpp"

#include "client/MD2WeaponImago.hpp"

#include "matrix/Bot.hpp"
#include "matrix/Orbis.hpp"
#include "client/Context.hpp"
#include "client/Colours.hpp"

#include <typeinfo>

namespace oz
{
namespace client
{
Pool<MD2WeaponImago, 256> MD2WeaponImago::pool;

Imago* MD2WeaponImago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::DYNAMIC_BIT );
  hard_assert( obj->flags & Object::WEAPON_BIT );

  MD2WeaponImago* imago = new MD2WeaponImago();

  imago->obj   = obj;
  imago->clazz = obj->clazz;
  imago->md2   = context.requestMD2( obj->clazz->imagoModel );

  imago->setAnim( Anim::STAND );
  imago->anim.nextFrame = imago->anim.endFrame;
  imago->anim.currFrame = imago->anim.endFrame;

  return imago;
}

void MD2WeaponImago::draw( const Imago* parent, int )
{
  if( !md2->isLoaded ) {
    return;
  }

  if( parent == null ) {
    tf.model = tf.model * md2->weaponTransf;

    md2->drawFrame( 0 );
  }
  else if( parent->flags & Imago::MD2MODEL_BIT ) {
    const MD2Imago* parentImago = static_cast<const MD2Imago*>( parent );

    md2->draw( &parentImago->anim );
  }
}

}
}
