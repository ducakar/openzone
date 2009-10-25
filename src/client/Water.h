/*
 *  Water.h
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

  struct Water
  {
    static const float ALPHA;
    static const float TEX_BIAS;

    float phi;
    float alpha1;
    float alpha2;

    Water();

    void update();
  };

  extern Water water;

}
}
