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
    typedef Model *( *InitFunc )( const Object *object );

    const Object *obj;
    bool         isUpdated;

    // there should not be any references to object in the destructor as the object may have been
    // deleted already
    virtual ~Model();

    virtual void draw() = 0;
  };

}
}
