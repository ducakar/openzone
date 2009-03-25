/*
 *  D_BigCrate.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObject.h"

namespace oz
{

  class D_BigCrate : public DynObject
  {
    protected:

      void onDestroy();
      void onFrictBegin();
      void onFrictEnd();

    public:

      static const char *NAME;
      static const int TYPE;

      Object *content;

      static Object *build( const Vec3 &p, Object *content = null );

      explicit D_BigCrate( const Vec3 &p = Vec3::zero(), Object *content = null );
      ~D_BigCrate();

//       void createModel();
  };

}
