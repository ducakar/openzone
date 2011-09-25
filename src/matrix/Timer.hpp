/*
 *  Timer.hpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Timer
  {
    public:

      static const int   TICK_MILLIS = 20;
      static const float TICK_TIME;

      int   ticks;
      int   millis;
      float time;

      uint  matrixMillis;
      uint  nirvanaMillis;

      uint  sleepMillis;
      uint  loaderMillis;
      uint  soundMillis;
      uint  renderMillis;
      uint  renderScheduleMillis;
      uint  renderCaelumMillis;
      uint  renderTerraMillis;
      uint  renderStructsMillis;
      uint  renderObjectsMillis;
      uint  renderParticlesMillis;
      uint  renderMiscMillis;
      uint  renderUiMillis;
      uint  renderSyncMillis;
      uint  uiMillis;

      int   nFrames;
      int   frameTicks;
      int   frameMillis;
      float frameTime;

      Timer();

      void reset();
      void tick();
      void frame();
  };

  extern Timer timer;

}
