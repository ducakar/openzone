/*
 *  Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "MD2.hpp"

namespace oz
{
namespace Client
{

  struct Model
  {
    enum State
    {
      NOT_UPDATED,
      UPDATED
    };

    State state;
    Model *prev[1];
    Model *next[1];

    virtual void load() = 0;

    virtual void draw() = 0;
  };

}
}
