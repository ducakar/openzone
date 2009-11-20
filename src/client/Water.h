/*
 *  Water.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

    void init();
    void update();
  };

  extern Water water;

}
}
