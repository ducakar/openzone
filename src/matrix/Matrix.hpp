/*
 *  Matrix.h
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "World.hpp"
#include "Bot.hpp"

namespace oz
{

  class Matrix
  {
    public:

      static const float G_ACCEL;

      // thread synchronization
      SDL_semaphore *semaphore;

      void load();
      void update();
      void free();
  };

  extern Matrix matrix;

}
