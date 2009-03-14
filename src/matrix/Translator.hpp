/*
 *  Translator.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

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

      HashString<int, 1024> textureIndices;
      HashString<int, 1024> soundIndices;
      HashString<int, 1024> modelIndices;

    public:

      Vector<String> textures;
      Vector<String> sounds;
      Vector<String> models;

      int getTexture( const char *name );
      int getSound( const char *name );
      int getModel( const char *name );

      bool init();
      void free();
  };

  extern Translator translator;

}
