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

  struct SimpleAudio : Audio
  {
    static Audio *create( const Object *object );

    void update();
  };

}
}
