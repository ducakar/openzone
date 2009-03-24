/*
 *  Sky.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Sky.h"

#include "matrix/Timer.h"
#include "matrix/World.h"

namespace oz
{
namespace client
{

  Sky sky;

  const float Sky::AMBIENT_COEF = 0.40f;

  const float Sky::RED_COEF = +0.05f;
  const float Sky::GREEN_COEF = -0.05f;
  const float Sky::BLUE_COEF = -0.10f;

  const float Sky::DAY_COLOR[] = { 0.45f, 0.60f, 0.90f };
  const float Sky::NIGHT_COLOR[] = { 0.02f, 0.03f, 0.06f };

  void Sky::init()
  {
    float heading = Math::rad( world.sky.heading );

    axis = Vec3( Math::cos( heading ), Math::sin( -heading ), 0.0f );
    originalLightDir = Vec3( Math::sin( heading ), Math::cos( heading ), 0.0f );

    update();
  }

  void Sky::update()
  {
    float angle = 2.0f * Math::PI * ( world.sky.time / world.sky.period );
    Vec3  dir = Quat::rotAxis( axis, angle ).rotate( originalLightDir );

    float ratio = bound( dir.z, 0.0f, 1.0f );
    float ratio1 = 1.0f - ratio;
    float ratioDiff = ( 1.0f - abs( ratio - ratio1 ) );

    color[0] = ratio * DAY_COLOR[0] + ratio1 * NIGHT_COLOR[0] + RED_COEF * ratioDiff;
    color[1] = ratio * DAY_COLOR[1] + ratio1 * NIGHT_COLOR[1] + GREEN_COEF * ratioDiff;
    color[2] = ratio * DAY_COLOR[2] + ratio1 * NIGHT_COLOR[2] + BLUE_COEF * ratioDiff;
    color[3] = 1.0f;

    lightDir[0] = dir.x;
    lightDir[1] = dir.y;
    lightDir[2] = dir.z;
    lightDir[3] = 0.0f;

    diffuseColor[0] = ratio + RED_COEF * ratioDiff;
    diffuseColor[1] = ratio + GREEN_COEF * ratioDiff;
    diffuseColor[2] = ratio + BLUE_COEF * ratioDiff;
    diffuseColor[3] = 1.0f;

    ambientColor[0] = AMBIENT_COEF * diffuseColor[0];
    ambientColor[1] = AMBIENT_COEF * diffuseColor[1];
    ambientColor[2] = AMBIENT_COEF * diffuseColor[2];
    ambientColor[3] = 1.0f;
  }

}
}
