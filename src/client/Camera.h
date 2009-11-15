/*
 *  Camera.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

      // leave this much space between obstacle and camera, if camera is brought closer to the eyes
      // because of an obstacle
      static const float THIRD_PERSON_CLIP_DIST;

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
      // how far behind the eyes the camera should be
      float thirdPersonDist;

      bool  isThirdPerson;
      bool  isFreeLook;

      Camera();

      void init();

      void update();
  };

  extern Camera camera;

}
}
