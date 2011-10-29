/*
 *  Timer.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file common/Timer.hpp
 */

#include "oz/oz.hpp"

namespace oz
{

class Timer
{
  public:

    static const int   TICK_MILLIS   = 17;
    static const float TICK_TIME;
    static const int   TICKS_PER_SEC = 1000 / TICK_MILLIS;

    int   ticks;
    int   millis;
    float time;

    uint  matrixMillis;
    uint  nirvanaMillis;

    uint  sleepMillis;
    uint  loaderMillis;
    uint  soundMillis;
    uint  renderMillis;
    uint  renderPrepareMillis;
    uint  renderCaelumMillis;
    uint  renderTerraMillis;
    uint  renderStructsMillis;
    uint  renderObjectsMillis;
    uint  renderParticlesMillis;
    uint  renderMiscMillis;
    uint  renderPostprocessMillis;
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