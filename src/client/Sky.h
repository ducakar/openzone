/*
 *  Sky.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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
      uint  sunList;
      uint  moonList;

      Vec3  stars[MAX_STARS];

    public:

      float angle;
      float ratio;
      float ratio_1;
      Vec3  originalLightDir;
      Vec3  axis;

      float lightDir[4];

      void load();
      void unload();

      void update();
      void draw();
  };

  extern Sky sky;

}
}
