/*
 *  Translator.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "ObjectClass.h"

namespace oz
{

  enum AnimEnum
  {
    ANIM_STAND,
    ANIM_RUN,
    ANIM_ATTACK,
    ANIM_PAIN_A,
    ANIM_PAIN_B,
    ANIM_PAIN_C,
    ANIM_JUMP,
    ANIM_FLIP,
    ANIM_SALUTE,
    ANIM_FALLBACK,
    ANIM_WAVE,
    ANIM_POINT,
    ANIM_CROUCH_STAND,
    ANIM_CROUCH_WALK,
    ANIM_CROUCH_ATTACK,
    ANIM_CROUCH_PAIN,
    ANIM_CROUCH_DEATH,
    ANIM_DEATH_FALLBACK,
    ANIM_DEATH_FALLFORWARD,
    ANIM_DEATH_FALLBACKSLOW,
    ANIM_MAX
  };

  class Translator
  {
    private:

      HashString<int, 1021> textureIndices;
      HashString<int, 1021> soundIndices;

    public:

      Vector<String> textures;
      Vector<String> sounds;

      HashString<ObjectClass::InitFunc, 31> baseClasses;
      HashString<ObjectClass*, 251> classes;

      int textureIndex( const char *file );
      int soundIndex( const char *file );

      bool init();
      void free();
  };

  extern Translator translator;

}
