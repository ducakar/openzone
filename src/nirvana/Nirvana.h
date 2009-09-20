/*
 *  Nirvana.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Soul.h"
#include "Mind.h"

#include "matrix/Matrix.h"

#include "RandomMind.h"

namespace oz
{

  class Nirvana
  {
    protected:

      DList<Mind, 0> minds;

      void sync();
      void run();

      static int runThread( void *data );

    public:

      SDL_semaphore *semaphore;
      SDL_Thread    *thread;

      volatile bool isAlive;

      void start();
      void stop();

      void load();
      void free();

  };

  extern Nirvana nirvana;

}
