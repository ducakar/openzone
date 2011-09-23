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

#include "matrix/Struct.hpp"
#include "matrix/ObjectClass.hpp"

namespace oz
{

  struct Anim
  {
    enum Type
    {
      STAND,
      RUN,
      ATTACK,
      PAIN_A,
      PAIN_B,
      PAIN_C,
      JUMP,
      FLIP,
      SALUTE,
      FALLBACK,
      WAVE,
      POINT,
      CROUCH_STAND,
      CROUCH_WALK,
      CROUCH_ATTACK,
      CROUCH_PAIN,
      CROUCH_DEATH,
      DEATH_FALLBACK,
      DEATH_FALLFORWARD,
      DEATH_FALLBACKSLOW,
      MAX
    };
  };

  class Translator
  {
    public:

      struct Resource
      {
        String name;
        String path;

        Resource()
        {}

        explicit Resource( const String& name, const String& path );
      };

    private:

      HashString<int, 256> textureIndices;
      HashString<int, 256> soundIndices;
      HashString<int, 64>  shaderIndices;
      HashString<int, 16>  terraIndices;
      HashString<int, 16>  caelumIndices;
      HashString<int, 64>  bspIndices;
      HashString<int, 256> modelIndices;
      HashString<int, 16>  nameListIndices;

    public:

      Vector<Resource> textures;
      Vector<Resource> sounds;
      Vector<Resource> shaders;
      Vector<Resource> terras;
      Vector<Resource> caela;
      Vector<Resource> bsps;
      Vector<Resource> models;
      Vector<Resource> nameLists;
      Vector<Resource> musics;

      HashString<const ObjectClass::InitFunc, 8> baseClasses;
      HashString<const ObjectClass*, 64> classes;

      int textureIndex( const char* name ) const;
      int soundIndex( const char* name ) const;
      int shaderIndex( const char* name ) const;
      int terraIndex( const char* name ) const;
      int caelumIndex( const char* name ) const;
      int bspIndex( const char* name ) const;
      int modelIndex( const char* name ) const;
      int nameListIndex( const char* name ) const;

#ifndef OZ_TOOLS
      Struct* createStruct( int index, const char* name, const Point3& p,
                            Struct::Rotation rot ) const;
      Struct* createStruct( int index, const char* name, InputStream* istream ) const;
      Object* createObject( int index, const char* name, const Point3& p ) const;
      Object* createObject( int index, const char* name, InputStream* istream ) const;
#endif

      void init();
      void free();
  };

  extern Translator translator;

}
