/*
 *  Nirvana.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Soul.hpp"
#include "Mind.hpp"

#include "matrix/Matrix.hpp"

#include "M_Walker.hpp"

namespace oz
{

  class Nirvana
  {
    protected:

      static const int SOULS_MAX = 32;

      Soul souls[SOULS_MAX];
      DList<Mind, 0> minds;

      void think();
      void synchronize();
      void run();

      static int runThread( void *data );

    public:

      volatile bool isAlive;
      volatile bool requestSuspend;
      SDL_semaphore *semaphore;
      SDL_Thread    *thread;

      void add( Mind *mind );

      void load();
      void start();
      void stop();
      void free();
  };

  extern Nirvana nirvana;

}
