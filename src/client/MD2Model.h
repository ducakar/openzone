/*
 *  MD2Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Bot.h"
#include "Model.h"
#include "MD2.h"

namespace oz
{
namespace client
{

  struct MD2Model : Model
  {
    MD2            *md2;
    MD2::AnimState anim;

    static Model *create( const Object *object );

    virtual ~MD2Model();

    void setAnim( int type );
    virtual void draw();
  };

}
}
