/*
 *  OBJModel.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Model.h"

namespace oz
{
namespace client
{

  struct OBJModel : Model
  {
    uint list;

    static Model *create( const Object *obj );

    virtual ~OBJModel();

    virtual void draw();
  };

}
}
