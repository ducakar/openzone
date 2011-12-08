/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Caelum.hh
 */

#pragma once

#include "client/common.hh"

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
