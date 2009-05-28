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
    public:

      SimpleAudio( const Object *obj ) : Audio( obj )
      {}

      void update();

  };

}
}
