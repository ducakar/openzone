/*
 *  BotAudio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Audio.h"

namespace oz
{
namespace client
{

  struct BotAudio : Audio
  {
    static const int SND_FRICTING = Object::EVENT_FRICTING;

    BotAudio( const Object *obj ) : Audio( obj ) {}

    static Audio *create( const Object *obj );

    void update();
  };

}
}
