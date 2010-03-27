/*
 *  Translator.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/Structure.h"
#include "matrix/ObjectClass.h"

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

        Resource( const String& name_, const String& path_ ) : name( name_ ), path( path_ ) {}
      };

    private:

      HashString<int, 512> textureIndices;
      HashString<int, 512> soundIndices;
      HashString<int, 128>  bspIndices;

    public:

      Vector<Resource> textures;
      Vector<Resource> sounds;
      Vector<Resource> bsps;

      Vector<Resource> matrixScripts;
      Vector<Resource> nirvanaScripts;

      HashString<ObjectClass::InitFunc, 8> baseClasses;
      HashString<ObjectClass*, 64> classes;

      int textureIndex( const char* file )
      {
        if( textureIndices.contains( file ) ) {
          return textureIndices.cachedValue();
        }
        else {
          log.println( "W: invalid texture file index requested: %s", file );
          return -1;
        }
      }

      int soundIndex( const char* file )
      {
        if( soundIndices.contains( file ) ) {
          return soundIndices.cachedValue();
        }
        else {
          log.println( "W: invalid sound file index requested: %s", file );
          return -1;
        }
      }

      int bspIndex( const char* file )
      {
        if( bspIndices.contains( file ) ) {
          return bspIndices.cachedValue();
        }
        else {
          throw Exception( "Invalid BSP index requested" );
        }
      }

      Structure* createStruct( int index, const char* name, const Vec3& p, Structure::Rotation rot )
      {
        if( bspIndices.contains( name ) ) {
          return new Structure( index, bspIndices.cachedValue(), p, rot );
        }
        else {
          throw Exception( "Invalid Structure class requested" );
        }
      }

      Structure* createStruct( int index, const char* name, InputStream* istream )
      {
        if( bspIndices.contains( name ) ) {
          return new Structure( index, bspIndices.cachedValue(), istream );
        }
        else {
          throw Exception( "Invalid Structure class requested" );
        }
      }

      Object* createObject( int index, const char* name, const Vec3& p )
      {
        if( classes.contains( name ) ) {
          return classes.cachedValue()->create( index, p );
        }
        else {
          throw Exception( "Invalid Object class requested" );
        }
      }

      Object* createObject( int index, const char* name, InputStream* istream )
      {
        if( classes.contains( name ) ) {
          return classes.cachedValue()->create( index, istream );
        }
        else {
          throw Exception( "Invalid Object class requested" );
        }
      }

      void init();
      void free();
  };

  extern Translator translator;

}
