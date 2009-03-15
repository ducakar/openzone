/*
 *  Particles.hpp
 *
 *  Loads and/or generates GL lists for common particles.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

// TODO particle renderer

namespace oz
{
namespace Client
{

  struct Particles
  {
    uint spark;

    void init();
    void free();
  };

  extern Particles particles;

}
}
