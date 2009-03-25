/*
 *  Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/ObjectClass.h"

namespace oz
{
namespace client
{

  struct Model
  {
    typedef Model *( *InitFunc )( const ObjectClass *clazz );

    enum State
    {
      NOT_UPDATED,
      UPDATED
    };

    State state;

    virtual void draw() = 0;
  };

}
}
