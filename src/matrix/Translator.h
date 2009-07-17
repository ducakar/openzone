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
    public:

      struct Resource
      {
        String name;
        String path;

        Resource() {}
        Resource( const String &name_, const String &path_ ) : name( name_ ), path( path_ ) {}
      };

    private:

      HashString<int, 1021> textureIndices;
      HashString<int, 1021> soundIndices;
      HashString<int, 1021> bspIndices;

    public:

      Vector<Resource> textures;
      Vector<Resource> sounds;
      Vector<Resource> bsps;

      HashString<ObjectClass::InitFunc, 31> baseClasses;
      HashString<ObjectClass*, 251> classes;

      int textureIndex( const char *file );
      int soundIndex( const char *file );
      int bspIndex( const char *file );

      Object *createObject( const char *name, const Vec3 &p )
      {
        return classes[name]->create( p );
      }

      Object *createObject( const char *name, InputStream *istream )
      {
        return classes[name]->create( istream );
      }

      bool init();
      void free();
  };

  extern Translator translator;

}
