/*
 *  Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Class.h"
#include "MD2.h"

namespace oz
{
namespace client
{

  struct Model
  {
    typedef Model *( *InitFunc )( const Class *clazz );

    enum State
    {
      NOT_UPDATED,
      UPDATED
    };

    State state;

    virtual void load() = 0;
    virtual void draw() = 0;
  };

}
}
