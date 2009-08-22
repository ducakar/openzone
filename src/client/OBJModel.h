/*
 *  OBJModel.h
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

  struct OBJModel : Model
  {
    const char *name;
    uint list;

    static Model *create( const Object *object );

    virtual ~OBJModel();

    virtual void draw();
  };

}
}
