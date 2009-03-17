/*
 *  Sky.hpp
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

      static const float AMBIENT_COEF;

      static const float RED_COEF;
      static const float GREEN_COEF;
      static const float BLUE_COEF;

      static const float DAY_COLOR[3];
      static const float NIGHT_COLOR[3];

      Vec3  originalLightDir;
      Vec3  axis;

    public:

      // sky color
      float color[4];

      float lightDir[4];
      float diffuseColor[4];
      float ambientColor[4];

      void init();

      void update();
      void set( float time );
  };

  extern Sky sky;

}
}
