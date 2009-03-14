/*
 *  Camera.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Matrix.hpp"

namespace oz
{
namespace Client
{

  class Camera
  {
    private:

      float smoothCoef;
      float smoothCoef_1;

      Vec3  oldP;

    public:

      Vec3  p;
      float h, v, r;

      Vec3  at;
      Vec3  up;

      Mat44 rotMat;
      Mat44 rotTMat;

      Bot     *player;
      World   *world;
      Matrix  *matrix;

      Camera();

      void init();
      void update();
  };

  extern Camera camera;

}
}
