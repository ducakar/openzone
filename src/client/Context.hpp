/*
 *  Context.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Translator.hpp"

#include "client/BSP.hpp"
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
    friend class Render;
    friend class Sound;
    friend class Audio;
    friend class Loader;

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

      struct Source
      {
        uint    source;
        Source* next[1];

        explicit Source( uint sourceId ) : source( sourceId )
        {}

        static Pool<Source> pool;

        OZ_STATIC_POOL_ALLOC( pool )
      };

      struct ContSource
      {
        uint source;
        bool isUpdated;

        explicit ContSource( uint sourceId ) : source( sourceId ), isUpdated( true )
        {}
      };

      template <typename Type>
      struct Resource
      {
        Type id;
        union
        {
          // for sounds:
          //  0: loaded, but no source needs it
          // -1: not loaded
          // -2: scheduled for removal
          int  nUsers;
          bool isUpdated;
        };
      };

      template <typename Type>
      struct Resource<Type*>
      {
        Type* object;
        union
        {
          int  nUsers;
          bool isUpdated;
        };
      };

      char                              vorbisBuffer[VORBIS_BUFFER_SIZE];

    private:

      HashString<Model::CreateFunc, 16> modelClasses;
      HashString<Audio::CreateFunc, 8>  audioClasses;

      Resource<uint>*                   textures;
      Resource<uint>*                   sounds;

      Sparse<Lists>                     lists;

      ContSource*                       cachedSource;
      List<Source>                      sources;
      HashIndex<ContSource, 256>        contSources;

      Resource<BSP*>*                   bsps;

      HashString< Resource<OBJ*>, 64 >  objs;
      HashString< Resource<MD2*>, 64 >  staticMd2s;
      HashString< Resource<MD2*>, 64 >  md2s;
      HashString< Resource<MD3*>, 64 >  staticMd3s;
      HashString< Resource<MD3*>, 64 >  md3s;

      HashIndex<Model*, 8191>           models;   // currently loaded models
      HashIndex<Audio*, 1021>           audios;   // currently loaded audio models

      int                               maxModels;
      int                               maxAudios;
      int                               maxSources;
      int                               maxContSources;

      static uint buildTexture( const void* data, int width, int height, int bytesPerPixel,
                                bool wrap, int magFilter, int minFilter );
      static uint buildNormalmap( void* data, const Vec3& lightNormal, int width,int height,
                                  int bytesPerPixel, bool wrap, int magFilter, int minFilter );

    public:

      uint createTexture( const void* data,
                          int width,
                          int height,
                          int bytesPerPixel,
                          bool wrap = true,
                          int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      uint createNormalmap( void* data,
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

      BSP* loadBSP( int resource );
      void releaseBSP( int resource );

      uint genList();
      uint genLists( int count );
      void freeLists( uint listId );

      OBJ* loadOBJ( const char* name );
      void releaseOBJ( const char* name );

      uint loadStaticMD2( const char* name );
      void releaseStaticMD2( const char* name );

      MD2* loadMD2( const char* name );
      void releaseMD2( const char* name );

      uint loadStaticMD3( const char* name );
      void releaseStaticMD3( const char* name );

      MD3* loadMD3( const char* name );
      void releaseMD3( const char* name );

      int  drawBSP( const Struct* str );
      void drawBSPWater( const Struct* str );

      void drawModel( const Object* obj, const Model* parent );
      void playAudio( const Object* obj, const Audio* parent );

      void updateLoad();
      void printLoad();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Context context;

}
}
