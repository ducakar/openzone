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

      static const int UPDATED_BIT  = 0x00000001;
      static const int MD2MODEL_BIT = 0x00000002;

    protected:

      Model() : obj( null ), flags( 0 ) {}

    public:

      const Object *obj;
      int flags;

      virtual ~Model();

      virtual void draw( const Model *parent ) = 0;

  };

}
}
