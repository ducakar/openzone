/*
 *  Timer.hpp
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
    protected:

      Vector<float*> timers;

    public:

      int   frameMillis;
      int   millis;

      float frameTime;
      float time;

      void update( int frameMillis );
  };

  extern Timer timer;

}
