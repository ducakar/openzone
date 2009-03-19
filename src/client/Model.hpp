/*
 *  Model.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "MD2.hpp"

namespace oz
{
namespace client
{

  struct Model
  {
    enum State
    {
      NOT_UPDATED,
      UPDATED
    };

    State state;

    static Model *create( Config *config );

    virtual void load() = 0;
    virtual void draw() = 0;
  };

}
}
