/*
 *  Sky.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Timer.h"

namespace oz
{

  struct Sky
  {
    float time;
    float period;

    float heading;

    // heading of sun in degrees and day time in seconds
    void init( float heading, float period );

    void update()
    {
//       time = Math::mod( time + timer.frameTime, period );
    }
  };

}
