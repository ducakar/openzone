/*
 *  Translator.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

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

      HashString<const ObjectClass::InitFunc, 8> baseClasses;
      HashString<const ObjectClass*, 64> classes;

      int textureIndex( const char* file ) const
      {
        const int* value = textureIndices.find( file );
        if( value != null ) {
          return *value;
        }
        else {
          log.println( "W: invalid texture file index requested: %s", file );
          return -1;
        }
      }

      int soundIndex( const char* file ) const
      {
        const int* value = soundIndices.find( file );
        if( value != null ) {
          return *value;
        }
        else {
          log.println( "W: invalid sound file index requested: %s", file );
          return -1;
        }
      }

      int bspIndex( const char* file ) const
      {
        const int* value = bspIndices.find( file );
        if( value != null ) {
          return *value;
        }
        else {
          throw Exception( "Invalid BSP index requested" );
        }
      }

      Structure* createStruct( int index, const char* name, const Vec3& p,
                               Structure::Rotation rot ) const
      {
        const int* value = bspIndices.find( name );
        if( value != null ) {
          return new Structure( index, *value, p, rot );
        }
        else {
          throw Exception( "Invalid Structure class requested" );
        }
      }

      Structure* createStruct( int index, const char* name, InputStream* istream ) const
      {
        const int* value = bspIndices.find( name );
        if( value != null ) {
          return new Structure( index, *value, istream );
        }
        else {
          throw Exception( "Invalid Structure class requested" );
        }
      }

      Object* createObject( int index, const char* name, const Vec3& p ) const
      {
        const ObjectClass* const* value = classes.find( name );
        if( value != null ) {
          return ( *value )->create( index, p );
        }
        else {
          throw Exception( "Invalid Object class requested" );
        }
      }

      Object* createObject( int index, const char* name, InputStream* istream ) const
      {
        const ObjectClass* const* value = classes.find( name );
        if( value != null ) {
          return ( *value )->create( index, istream );
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
