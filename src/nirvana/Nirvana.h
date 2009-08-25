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

      static const int SOULS_MAX = 32;

      DList<Mind, 0> minds;
      Vector<Mind*>  pendingMinds;

      void update();
      void run();

      static int runThread( void *data );

    public:

      SDL_semaphore *semaphore;
      SDL_Thread    *thread;

      volatile bool isAlive;
      volatile bool requestSuspend;

      void add( Mind *mind )
      {
        pendingMinds << mind;
      }

      void load();
      void free();

      void start();
      void stop();

      void sync();
  };

  extern Nirvana nirvana;

}
