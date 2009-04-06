/*
 *  SparkGen.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Sector;

  struct SparkGen
  {
    Vec3      p;

    int       index;
    Sector    *sector;

    SparkGen  *prev[1];
    SparkGen  *next[1];

    int       number;

    Vec3      velocity;
    float     velocitySpread;

    Vec3      color;
    Vec3      colorSpread;

    Vec3      gravity;
    float     sparkDim;
    float     lifeTime;

    SparkGen( const Vec3 &p, int number,
              const Vec3 &velocity, float velocitySpread,
              const Vec3 &color, const Vec3 &colorSpread,
              const Vec3 &gravity, float sparkDim, float lifeTime );
  };

}
