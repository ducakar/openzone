/*
 *  Camera.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Camera.h"

namespace oz
{
namespace client
{

  Camera camera;

  Camera::Camera() : h( 0.0f ), v( 0.0f ), r( 0.0f )
  {
    p.setZero();
    at = Vec3( 0.0f, 1.0f, 0.0f );
    up = Vec3( 0.0f, 0.0f, 1.0f );

    rotMat.setId();
    rotTMat.setId();
  }

  void Camera::init()
  {
    sscanf( config["render.camera.smoothCoef"], "%f", &smoothCoef );
    smoothCoef_1 = 1.0f - smoothCoef;
  }

  void Camera::update()
  {
    h = player->h;
    v = player->v;
    p = ( player->p + player->camPos ) * smoothCoef_1 + oldP * smoothCoef;
    oldP = p;

    float hRad = Math::rad( h );
    float vRad = Math::rad( v );
    float hSine, hCosine, vSine, vCosine;

    Math::sincos( hRad, &hSine, &hCosine );
    Math::sincos( vRad, &vSine, &vCosine );

    at = Vec3( -hSine * vCosine,  hCosine * vCosine, vSine );
    up = Vec3(  hSine * vSine,   -hCosine * vSine,   vCosine );

    rotTMat = Mat44( hCosine, -hSine * vCosine,    hSine * vSine,   0.0f,
                     hSine,    hCosine * vCosine, -hCosine * vSine, 0.0f,
                     0.0f,     vSine,              vCosine,         0.0f,
                     0.0f,     0.0f,               0.0f,            1.0f );

    rotMat = ~rotTMat;
  }

}
}
