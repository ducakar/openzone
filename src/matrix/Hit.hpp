/*
 *  Hit.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Object;

  struct Hit
  {
    float  ratio;

    Vec3   normal;
    Object *obj;
    Object *sObj;
    bool   isInWater;
  };

}
