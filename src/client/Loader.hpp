/*
 *  Loader.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  class Loader
  {
    private:

      static const int TICK_CLEAR_PERIOD         = 60 * 20;
      static const int MODEL_CLEAR_INTERVAL      = 30 * 20;
      static const int MODEL_CLEAR_LAG           = 0;
      static const int AUDIO_CLEAR_INTERVAL      = 10 * 20;
      static const int AUDIO_CLEAR_LAG           = 5;
      static const int SOURCE_CLEAR_INTERVAL     = 1 * 20;      // 1 s
      static const int SOURCE_CLEAR_LAG          = 10;
      static const int CONTSOURCE_CLEAR_INTERVAL = 1 * 20;      // 1 s
      static const int CONTSOURCE_CLEAR_LAG      = 15;

      int tick;

      // clean up unused models, handle screenshots
      void cleanupRender();
      // stop playing stopped continuous sounds, clean up unused audio models
      void cleanupSound();

  public:

      void makeScreenshot();

      void cleanup();
      void update();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Loader loader;

}
}
