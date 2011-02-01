/*
 *  MD2Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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
  Pool<MD2WeaponModel, 256> MD2WeaponModel::pool;

  Model* MD2WeaponModel::create( const Object* obj )
  {
    assert( obj->flags & Object::DYNAMIC_BIT );
    assert( obj->flags & Object::WEAPON_BIT );

    MD2WeaponModel* model = new MD2WeaponModel();

    model->obj = obj;
    model->md2 = context.loadMD2( obj->clazz->modelName );

    model->setAnim( Bot::ANIM_STAND );
    model->anim.nextFrame = model->anim.endFrame;
    model->anim.currFrame = model->anim.endFrame;

    return model;
  }

  void MD2WeaponModel::draw( const Model* parent )
  {
    if( parent == null ) {
      glTranslatef( md2->weaponTransl.x, md2->weaponTransl.y, md2->weaponTransl.z );

      md2->drawFrame( 0 );
    }
    else if( parent->flags & Model::MD2MODEL_BIT ) {
      const MD2Model* parentModel = static_cast<const MD2Model*>( parent );

      md2->draw( &parentModel->anim );
    }
  }

}
}
