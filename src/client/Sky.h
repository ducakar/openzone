/*
 *  Sky.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

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

      static const float DAY_COLOR[4];
      static const float NIGHT_COLOR[4];
      static const float WATER_COLOR[4];
      static const float STAR_COLOR[4];

      static const int MAX_STARS = 512;

      uint  sunTexId;
      uint  moonTexId;

      Vec3  stars[MAX_STARS];

    public:

      float angle;
      float ratio;
      float ratio_1;
      Vec3  originalLightDir;
      Vec3  axis;

      float skyColor[4];
      float waterColor[4];

      float lightDir[4];
      float diffuseColor[4];
      float ambientColor[4];

      void init();
      void free();

      void update();
      void draw();
  };

  extern Sky sky;

}
}
