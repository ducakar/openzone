/*
 *  BasicAudio.h
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

  struct BasicAudio : Audio
  {
    /*
     * STANDARD SOUND SLOTS
     */
    static const int SND_DESTROY  = Object::EVENT_DESTROY;
    static const int SND_DAMAGE   = Object::EVENT_DAMAGE;
    static const int SND_HIT      = Object::EVENT_HIT;
    static const int SND_SPLASH   = Object::EVENT_SPLASH;
    static const int SND_FRICTING = Object::EVENT_FRICTING;
    static const int SND_USE      = Object::EVENT_USE;

    BasicAudio( const Object *obj ) : Audio( obj ) {}

    static Audio *create( const Object *obj );

    void update();
  };

}
}
