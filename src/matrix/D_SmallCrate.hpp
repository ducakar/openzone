/*
 *  D_SmallCrate.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObject.hpp"

namespace oz
{

  class  D_SmallCrate : public DynObject
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

      explicit D_SmallCrate( const Vec3 &p = Vec3::zero(), Object *content = null );
      ~D_SmallCrate();

//       void createModel();
  };

}
