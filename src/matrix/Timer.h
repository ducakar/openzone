/*
 *  Timer.h
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Timer
  {
    public:

      int   frameMillis;
      int   millis;

      float frameTime;
      float time;

      void update( int frameMillis );
  };

  extern Timer timer;

}
