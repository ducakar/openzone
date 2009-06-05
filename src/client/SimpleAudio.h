/*
 *  SimpleAudio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Audio.h"

namespace oz
{
namespace client
{

  class SimpleAudio : public Audio
  {
    private:

      static const int SAMPLE_HIT       = 0;
      static const int SAMPLE_LAND      = 1;
      static const int SAMPLE_FRICTION  = 2;
      static const int SAMPLE_SPLASH    = 3;
      static const int SAMPLE_BIGSPLASH = 4;

    public:

      SimpleAudio( const Object *obj ) : Audio( obj )
      {}

      void update();

  };

}
}
