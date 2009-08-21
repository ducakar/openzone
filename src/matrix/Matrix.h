/*
 *  Matrix.h
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "World.h"
#include "Synapse.h"
#include "Bot.h"

namespace oz
{

  class Matrix
  {
    public:

      static const float G_ACCEL;
      static const float MAX_VELOCITY2;

      // thread synchronization
      SDL_semaphore *semaphore;

      void load();
      void free();

      void update();
      void sync();
  };

  extern Matrix matrix;

}
