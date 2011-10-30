/*
 *  MD2Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
Pool<MD2WeaponImago> MD2WeaponImago::pool;

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
