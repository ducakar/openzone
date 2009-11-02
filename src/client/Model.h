/*
 *  Model.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/ObjectClass.h"

namespace oz
{
namespace client
{

  struct Model
  {
    typedef Model *( *CreateFunc )( const Object *object );

    const Object *obj;
    bool         isUpdated;

    virtual ~Model();

    virtual void draw() = 0;
  };

}
}
