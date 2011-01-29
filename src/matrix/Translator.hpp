/*
 *  Translator.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Structure.hpp"
#include "matrix/ObjectClass.hpp"

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

        explicit Resource( const String& name, const String& path );
      };

    private:

      HashString<int, 512> textureIndices;
      HashString<int, 512> soundIndices;
      HashString<int, 128> bspIndices;

    public:

      Vector<Resource> textures;
      Vector<Resource> sounds;
      Vector<Resource> bsps;

      Vector<Resource> models;
      Vector<Resource> terras;

      Vector<Resource> matrixScripts;
      Vector<Resource> nirvanaScripts;

      Vector<Resource> names;

      HashString<const ObjectClass::InitFunc, 8> baseClasses;
      HashString<const ObjectClass*, 64> classes;

      int textureIndex( const char* name ) const;
      int soundIndex( const char* name ) const;
      int bspIndex( const char* name ) const;

      Structure* createStruct( int index, const char* name, const Point3& p,
                               Structure::Rotation rot ) const;
      Structure* createStruct( int index, const char* name, InputStream* istream ) const;
      Object* createObject( int index, const char* name, const Point3& p ) const;
      Object* createObject( int index, const char* name, InputStream* istream ) const;

      void init();
      void free();
  };

  extern Translator translator;

}
