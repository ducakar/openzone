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

    public:

      static const int DEFAULT_MAG_FILTER = GL_LINEAR;
      static const int DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

    private:

      // texture reading buffer
      static const int BUFFER_SIZE          = 256 * 1024;
      // default audio format
      static const int DEFAULT_AUDIO_FREQ   = 44100;
      static const int DEFAULT_AUDIO_FORMAT = AUDIO_S16LSB;

      template <typename Type>
      struct Resource
      {
        Type id;
        int  nUsers;
      };

      template <typename Type>
      struct Resource<Type*>
      {
        Type* object;
        int   nUsers;
      };

      struct Source
      {
        uint    id;
        int     sample;
        Source* next[1];

        explicit Source( uint sourceId, int sample_ ) : id( sourceId ), sample( sample_ )
        {}

        static Pool<Source> pool;

        OZ_STATIC_POOL_ALLOC( pool )
      };

      struct ContSource
      {
        uint id;
        int  sample;
        bool isUpdated;

        explicit ContSource( uint sourceId, int sample_ ) :
            id( sourceId ), sample( sample_ ), isUpdated( true )
        {}
      };

      HashString<Model::CreateFunc, 16> modelClasses;
      HashString<Audio::CreateFunc, 8>  audioClasses;

      Resource<uint>*                   textures;
      Resource<uint>*                   sounds;

      // non-looping sources
      List<Source>                      sources;
      // looping sources
      HashIndex<ContSource, 64>         bspSources;
      HashIndex<ContSource, 64>         objSources;

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

      void addSource( uint srcId, int sample );
      void addBSPSource( uint srcId, int sample, int key );
      void addObjSource( uint srcId, int sample, int key );

      void removeSource( Source* source, Source* prev );
      void removeBSPSource( ContSource* contSource, int key );
      void removeObjSource( ContSource* contSource, int key );

    public:

      Context();

#ifdef OZ_BUILD_TOOLS
      static uint createTexture( const void* data, int width, int height, int bytesPerPixel,
                          bool wrap = true, int magFilter = DEFAULT_MAG_FILTER,
                          int minFilter = DEFAULT_MIN_FILTER );

      static uint loadRawTexture( const char* path, bool wrap = true,
                                  int magFilter = DEFAULT_MAG_FILTER,
                                  int minFilter = DEFAULT_MIN_FILTER );
      static void writeTexture( uint id, OutputStream* stream );
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

#ifndef NDEBUG
      void updateLoad();
      void printLoad();
#endif

      void load();
      void unload();

      void init();
      void free();

  };

  extern Context context;

}
}
