/*
 *  ExplosionModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Model.h"

class GLUquadric;

namespace oz
{
namespace client
{

  struct ExplosionModel : Model
  {
    uint       texId;
    GLUquadric *quadric;
    int        startMillis;

    static Model *create( const Object *obj );

    virtual ~ExplosionModel();

    virtual void draw();
  };

}
}
