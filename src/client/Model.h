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

  class Model
  {
    public:

      typedef Model *( *CreateFunc )( const Object *object );

    protected:

      Model() : obj( null ), isUpdated( false ) {}

    public:

      const Object *obj;
      bint         isUpdated;

      virtual ~Model();

      virtual void draw( const Model *parent ) = 0;

  };

}
}
