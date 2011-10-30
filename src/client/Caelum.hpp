/*
 *  Caelum.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/common.hpp"

namespace oz
{
namespace client
{

class Caelum
{
  public:

    static const int   MAX_STARS = 4096;

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

    uint   vao;
    uint   vbo;

    uint   sunTexId;
    uint   moonTexId;

    int    starShaderId;
    int    celestialShaderId;

  public:

    float  angle;
    float  ratio;
    Vec3   originalLightDir;
    Vec3   axis;

    Vec3   lightDir;

    int    id;

    Caelum();

    void update();
    void draw();

    void load();
    void unload();

};

extern Caelum caelum;

}
}
