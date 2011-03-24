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

#include "client/Shader.hpp"
#include "client/Mesh.hpp"
#include "client/BSP.hpp"
#include "client/Model.hpp"
#include "client/Audio.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{

  class SMM;
  class MD2;
  class MD3;

  class Context
  {
    friend class Render;
    friend class Sound;
    friend class Audio;
    friend class BSP;
    friend class Loader;

    private:

      static const int DEFAULT_MAG_FILTER = GL_LINEAR;
      static const int DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;
      static const int BUFFER_SIZE        = 256 * 1024;

      template <typename Type>
      struct Resource
      {
        Type id;
        // for sounds:
        //  0: loaded, but no source needs it
        // -1: not loaded
        // -2: scheduled for removal
        int  nUsers;
      };

      template <typename Type>
      struct Resource<Type*>
      {
        Type* object;
        int  nUsers;
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

    private:

      HashString<Model::CreateFunc, 16> modelClasses;
      HashString<Audio::CreateFunc, 8>  audioClasses;

      Resource<uint>*                   textures;
      Resource<uint>*                   sounds;

      // non-looping sources
      List<Source>                      sources;
      // looping sources
      HashIndex<ContSource, 256>        bspSources;
      HashIndex<ContSource, 256>        objSources;

      Resource<BSP*>*                   bsps;

      HashIndex< Resource<SMM*>, 61 >   smms;
      HashIndex< Resource<MD2*>, 61 >   md2s;
      HashIndex< Resource<MD3*>, 61 >   md3s;

      HashIndex<Model*, 8191>           models;   // currently loaded models
      HashIndex<Audio*, 2039>           audios;   // currently loaded audio models

      static Buffer                     buffer;

      int                               maxModels;
      int                               maxAudios;
      int                               maxSources;
      int                               maxBSPSources;
      int                               maxObjSources;

#ifdef OZ_BUILD_TOOLS
      static uint buildTexture( const void* data, int width, int height, int bytesPerPixel,
                                bool wrap, int magFilter, int minFilter );
#endif
      void deleteSound( int resource );

    public:

#ifdef OZ_BUILD_TOOLS
      static uint createTexture( const void* data, int width, int height, int bytesPerPixel,
                          bool wrap = true, int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      static uint loadRawTexture( const char* path, int* nMipmaps = null, bool wrap = true,
                                  int magFilter = DEFAULT_MAG_FILTER,
                                  int minFilter = DEFAULT_MIN_FILTER );
      static void writeTexture( uint id, int nMipmaps, OutputStream* stream );
#endif

      static uint loadTexture( const char* path );
      static uint readTexture( InputStream* stream );

      uint requestTexture( int id );
      void releaseTexture( int id );

      uint requestSound( int id );
      void releaseSound( int id );

      SMM* requestSMM( int id );
      void releaseSMM( int id );

      MD2* requestMD2( int id );
      void releaseMD2( int id );

      MD3* requestMD3( int id );
      void releaseMD3( int id );

      void drawBSP( const Struct* str, int mask );
      void playBSP( const Struct* str );

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
