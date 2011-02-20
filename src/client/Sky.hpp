/*
 *  Sky.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  class Sky
  {
    private:

      static const float DAY_BIAS;

      static const float AMBIENT_COEF;

      static const float RED_COEF;
      static const float GREEN_COEF;
      static const float BLUE_COEF;

      static const float DAY_COLOUR[4];
      static const float NIGHT_COLOUR[4];
      static const float WATER_COLOUR[4];
      static const float STAR_COLOUR[4];

      static const int   MAX_STARS = 512;

      uint   sunTexId;
      uint   moonTexId;
      uint   sunList;
      uint   moonList;

      int    starArray;

      Point3 stars[MAX_STARS];

    public:

      float  angle;
      float  ratio;
      Vec3   originalLightDir;
      Vec3   axis;

      float  lightDir[4];

      void load();
      void unload();

      void update();
      void draw();
  };

  extern Sky sky;

}
}
