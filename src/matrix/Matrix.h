/*
 *  Matrix.h
 *
 *  World model
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Synapse.h"
#include "Bot.h"

namespace oz
{

  struct Matrix
  {
    private:

      void loadStressTest();
      void loadSample();

    public:

      static const float MAX_VELOCITY2;

      // thread synchronization
      SDL_semaphore* semaphore;

      void init();
      void free();

      void load( InputStream* istream );
      void unload( OutputStream* ostream );

      void cleanObjects();
      void update();

  };

  extern Matrix matrix;

}
