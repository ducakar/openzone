/*
 *  Model.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/ObjectClass.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

  class Model
  {
    public:

      typedef Model* ( * CreateFunc )( const Object* object );

      static const int UPDATED_BIT  = 0x00000001;
      static const int MD2MODEL_BIT = 0x00000002;

      // for Pool allocator
      Model* next[1];

    protected:

      explicit Model() : obj( null ), flags( 0 ) {}

    public:

      const Object* obj;
      int flags;

      virtual ~Model();

      virtual void draw( const Model* parent ) = 0;

  };

}
}
