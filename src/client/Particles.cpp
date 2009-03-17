/*
 *  Particles.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Particles.hpp"

#include "Context.hpp"

namespace oz
{
namespace client
{

  Particles particles;

  void Particles::init()
  {
    uint first = context.genList();
  }

  void Particles::free()
  {
  }

}
}
