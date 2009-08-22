/*
 *  MD3StaticModel.h
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

  struct MD3StaticModel : Model
  {
    const char *name;
    uint list;

    static Model *create( const Object *object );

    virtual ~MD3StaticModel();

    virtual void draw();
  };

}
}
