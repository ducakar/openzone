/*
 *  Camera.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Matrix.h"

namespace oz
{
namespace client
{

  class Camera
  {
    private:

      float smoothCoef;
      float smoothCoef_1;

    public:

      Vec3  p;
      Vec3  oldP;

      // relative to the object the camera is bound to
      float h;
      float v;

      Quat  relRot;
      Quat  rot;

      // global rotation matrix and it's inverse
      Mat44 rotMat;
      Mat44 rotTMat;

      Vec3  right;
      Vec3  at;
      Vec3  up;

      int   botIndex;
      Bot   *bot;

      Camera();

      void init();

      void update();
  };

  extern Camera camera;

}
}
