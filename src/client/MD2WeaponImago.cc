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
 * @file client/MD2WeaponImago.cc
 */

#include <stable.hh>
#include <client/MD2WeaponImago.hh>

#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/OpenGL.hh>

namespace oz
{
namespace client
{

Pool<MD2WeaponImago, 256> MD2WeaponImago::pool;

MD2WeaponImago::~MD2WeaponImago()
{
  context.releaseMD2( clazz->imagoModel );
}

Imago* MD2WeaponImago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::DYNAMIC_BIT );
  hard_assert( obj->flags & Object::WEAPON_BIT );

  MD2WeaponImago* imago = new MD2WeaponImago( obj );

  imago->md2 = context.requestMD2( obj->clazz->imagoModel );

  return imago;
}

void MD2WeaponImago::draw( const Imago* parent )
{
  flags |= UPDATED_BIT;

  if( !md2->isLoaded ) {
    return;
  }

  if( parent == nullptr ) {
    if( shader.mode == Shader::SCENE ) {
      tf.model = Mat44::translation( obj->p - Point::ORIGIN );
      tf.model.rotateZ( float( obj->flags & Object::HEADING_MASK ) * Math::TAU / 4.0f );
    }

    tf.push();
    tf.model = tf.model * md2->weaponTransf;

    md2->scheduleFrame( 0 );

    tf.pop();
  }
  else if( parent->flags & Imago::MD2MODEL_BIT ) {
    const MD2Imago* parentImago = static_cast<const MD2Imago*>( parent );

    md2->scheduleAnim( &parentImago->anim );
  }
}

}
}
