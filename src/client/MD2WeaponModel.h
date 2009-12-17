/*
 *  MD2WeaponModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/Bot.h"
#include "Model.h"
#include "MD2.h"

namespace oz
{
namespace client
{

  struct MD2WeaponModel : Model
  {
    MD2            *md2;
    MD2::AnimState anim;

    static Model *create( const Object *obj );

    virtual ~MD2WeaponModel();

    void setAnim( int type );
    virtual void draw();
  };

}
}
