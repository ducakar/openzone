/*
 *  MD2StaticModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Model.h"

namespace oz
{
namespace client
{

  struct MD2StaticModel : Model
  {
    uint list;

    static Model *create( const Object *object );

    virtual ~MD2StaticModel();

    virtual void draw();
  };

}
}
