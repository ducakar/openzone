/*
 *  MD2Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD2WeaponModel.hpp"

#include "matrix/Bot.hpp"
#include "matrix/Orbis.hpp"
#include "client/Context.hpp"
#include "client/Colours.hpp"

#include <typeinfo>

namespace oz
{
namespace client
{
  Pool<MD2WeaponModel> MD2WeaponModel::pool;

  Model* MD2WeaponModel::create( const Object* obj )
  {
    hard_assert( obj->flags & Object::DYNAMIC_BIT );
    hard_assert( obj->flags & Object::WEAPON_BIT );

    MD2WeaponModel* model = new MD2WeaponModel();

    model->obj   = obj;
    model->clazz = obj->clazz;
    model->md2   = context.requestMD2( obj->clazz->modelIndex );

    model->setAnim( Anim::STAND );
    model->anim.nextFrame = model->anim.endFrame;
    model->anim.currFrame = model->anim.endFrame;

    return model;
  }

  void MD2WeaponModel::draw( const Model* parent, int )
  {
    if( !md2->isLoaded ) {
      return;
    }

    if( parent == null ) {
      tf.model = tf.model * md2->weaponTransf;

      md2->drawFrame( 0 );
    }
    else if( parent->flags & Model::MD2MODEL_BIT ) {
      const MD2Model* parentModel = static_cast<const MD2Model*>( parent );

      md2->draw( &parentModel->anim );
    }
  }

}
}
