/*
 *  Water.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Object.h"

namespace oz
{

  class Water : public Object
  {
    protected:

      void onUpdate();

    public:

      static const char *NAME;
      static const int TYPE;

      Water( const Vec3 &p, const Vec3 &dim );
  };

}
