/*
 *  Context.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/Translator.hpp"

#include "client/Model.hpp"
#include "client/Audio.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  class MD2;
  class MD3;
  class OBJ;

  class Context
  {
    private:

      static const int DEFAULT_MAG_FILTER = GL_LINEAR;
      static const int DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;
      static const int VORBIS_BUFFER_SIZE = 1024 * 1024;

      struct Lists
      {
        uint base;

        // we can use union here, those two members are never in use at the same time
        union
        {
          int count;
          int nextSlot[1];
        };
      };

    public:

      template <typename Type>
      struct Resource
      {
        Type id;
        // for sounds:
        // -1: not loaded
        // -2: scheduled for removal
        int  nUsers;

        explicit Resource() : nUsers( 0 ) {}
      };

      template <typename Type>
      struct Resource<Type*>
      {
        Type* object;
        int   nUsers;

        explicit Resource() : nUsers( 0 ) {}
      };

      Resource<uint>* textures;
      Resource<uint>* sounds;

    private:

      char           vorbisBuffer[VORBIS_BUFFER_SIZE];
      Sparse<Lists>  lists;

      HashString< Resource<OBJ*>, 64 > objs;
      HashString< Resource<MD2*>, 64 > staticMd2s;
      HashString< Resource<MD2*>, 64 > md2s;
      HashString< Resource<MD3*>, 64 > staticMd3s;
      HashString< Resource<MD3*>, 64 > md3s;

      HashString<Model::CreateFunc, 16> modelClasses;
      HashString<Audio::CreateFunc, 8> audioClasses;

      static uint buildTexture( const ubyte* data, int width, int height, int bytesPerPixel,
                                bool wrap, int magFilter, int minFilter );
      static uint buildNormalmap( ubyte* data, const Vec3& lightNormal, int width,int height,
                                  int bytesPerPixel, bool wrap, int magFilter, int minFilter );

    public:

      uint createTexture( const ubyte* data,
                          int width,
                          int height,
                          int bytesPerPixel,
                          bool wrap = true,
                          int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      uint createNormalmap( ubyte* data,
                            const Vec3& lightNormal,
                            int width,
                            int height,
                            int bytesPerPixel,
                            bool wrap = true,
                            int magFilter = DEFAULT_MAG_FILTER,
                            int minFilter = DEFAULT_MIN_FILTER );

      uint requestTexture( int resource,
                           bool wrap = true,
                           int magFilter = DEFAULT_MAG_FILTER,
                           int minFilter = DEFAULT_MIN_FILTER );

      uint requestNormalmap( int resource,
                             const Vec3& lightNormal,
                             bool wrap = true,
                             int magFilter = DEFAULT_MAG_FILTER,
                             int minFilter = DEFAULT_MIN_FILTER );

      void releaseTexture( int resource );

      uint loadTexture( const char* path,
                        bool wrap = true,
                        int magFilter = DEFAULT_MAG_FILTER,
                        int minFilter = DEFAULT_MIN_FILTER );

      uint loadNormalmap( const char* path,
                          const Vec3& lightNormal,
                          bool wrap = true,
                          int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      void freeTexture( uint texId );

      uint requestSound( int resource );
      void releaseSound( int resource );
      void freeSound( int resource );

      uint genList();
      uint genLists( int count );
      void freeLists( uint listId );

      uint loadOBJ( const char* name );
      void releaseOBJ( const char* name );

      uint loadStaticMD2( const char* name );
      void releaseStaticMD2( const char* name );

      MD2*  loadMD2( const char* name );
      void releaseMD2( const char* name );

      uint loadStaticMD3( const char* name );
      void releaseStaticMD3( const char* name );

      MD3*  loadMD3( const char* name );
      void releaseMD3( const char* name );

      Model* createModel( const Object* obj )
      {
        if( obj->flags & Object::MODEL_BIT ) {
          assert( !obj->type->modelType.isEmpty() );

          const Model::CreateFunc* value = modelClasses.find( obj->type->modelType );
          if( value == null ) {
            throw Exception( "Invalid Model" );
          }
          return ( *value )( obj );
        }
        else {
          assert( obj->type->modelType.isEmpty() );

          return null;
        }
      }

      Audio* createAudio( const Object* obj )
      {
        if( obj->flags & Object::AUDIO_BIT ) {
          assert( !obj->type->audioType.isEmpty() );

          const Audio::CreateFunc* value = audioClasses.find( obj->type->audioType );
          if( value == null ) {
            throw Exception( "Invalid Audio" );
          }
          return ( *value )( obj );
        }
        else {
          assert( obj->type->audioType.isEmpty() );

          return null;
        }
      }

      explicit Context();

      void init();
      void free();

  };

  extern Context context;

}
}
