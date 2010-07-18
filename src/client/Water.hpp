/*
 *  Water.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
