/*
 *  BasicAudio.h
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

  struct BasicAudio : Audio
  {
    /*
     * STANDARD SOUND SLOTS
     */
    static const int SND_HIT_SOFT    = ObjectClass::AUDIO_SAMPLES - 5;
    static const int SND_HIT_HARD    = ObjectClass::AUDIO_SAMPLES - 4;
    static const int SND_SPLASH_SOFT = ObjectClass::AUDIO_SAMPLES - 3;
    static const int SND_SPLASH_HARD = ObjectClass::AUDIO_SAMPLES - 2;
    static const int SND_FRICTING    = ObjectClass::AUDIO_SAMPLES - 1;

    static const float MOMENTUM_TRESHOLD;
    static const float MOMENTUM_INTENSITY_RATIO;

    BasicAudio( const Object *obj ) : Audio( obj ) {}

    static Audio *create( const Object *obj );

    void update();
  };

}
}
