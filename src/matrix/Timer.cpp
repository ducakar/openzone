/*
 *  Timer.cpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Timer.hpp"

namespace oz
{

  const float Timer::TICK_TIME = float( TICK_MILLIS ) / 1000.0f;

  Timer timer;

  Timer::Timer()
  {
    reset();
  }

  void Timer::reset()
  {
    ticks                   = 0;
    millis                  = 0;
    time                    = 0.0f;

    matrixMillis            = 0;
    nirvanaMillis           = 0;

    sleepMillis             = 0;
    loaderMillis            = 0;
    soundMillis             = 0;
    renderMillis            = 0;
    renderPrepareMillis     = 0;
    renderCaelumMillis      = 0;
    renderTerraMillis       = 0;
    renderStructsMillis     = 0;
    renderObjectsMillis     = 0;
    renderParticlesMillis   = 0;
    renderMiscMillis        = 0;
    renderPostprocessMillis = 0;
    renderUiMillis          = 0;
    renderSyncMillis        = 0;
    uiMillis                = 0;

    nFrames                 = 0;
    frameTicks              = 0;
    frameMillis             = 0;
    frameTime               = 0.0f;
  }

  void Timer::tick()
  {
    ++ticks;
    millis      += TICK_MILLIS;
    time        = float( millis ) / 1000.0f;

    ++frameTicks;
    frameMillis += TICK_MILLIS;
    frameTime   += TICK_TIME;
  }

  void Timer::frame()
  {
    ++nFrames;

    frameTicks  = 0;
    frameMillis = 0;
    frameTime   = 0.0f;
  }

}
