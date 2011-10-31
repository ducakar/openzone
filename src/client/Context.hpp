/*
 *  Context.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Library.hpp"

#include "client/Shader.hpp"
#include "client/BSP.hpp"
#include "client/Imago.hpp"
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
  friend class Loader;
  friend class BSP;
  friend class Audio;
  friend class Render;
  friend class Sound;

  private:

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

    HashString<Imago::CreateFunc, 16> imagoClasses;
    HashString<Audio::CreateFunc, 8>  audioClasses;

    Resource<uint>*                   textures;
    Resource<uint>*                   sounds;

    // non-looping sources
    List<Source>                      sources;
    // looping sources
    HashIndex<ContSource, 64>         bspSources;
    HashIndex<ContSource, 64>         objSources;

    Resource<BSP*>*                   bsps;
    Resource<SMM*>*                   smms;
    Resource<MD2*>*                   md2s;
    Resource<MD3*>*                   md3s;

    HashIndex<Imago*, 8191>           imagines; // currently loaded graphics models
    HashIndex<Audio*, 2039>           audios;   // currently loaded audio models

    int                               maxImagines;
    int                               maxAudios;
    int                               maxSources;
    int                               maxBSPSources;
    int                               maxObjSources;

    int                               maxSMMImagines;
    int                               maxSMMVehicleImagines;
    int                               maxExplosionImagines;
    int                               maxMD2Imagines;
    int                               maxMD2WeaponImagines;
    int                               maxMD3Imagines;

    int                               maxBasicAudios;
    int                               maxBotAudios;
    int                               maxVehicleAudios;

    void addSource( uint srcId, int sample );
    void addBSPSource( uint srcId, int sample, int key );
    void addObjSource( uint srcId, int sample, int key );

    void removeSource( Source* source, Source* prev );
    void removeBSPSource( ContSource* contSource, int key );
    void removeObjSource( ContSource* contSource, int key );

  public:

    Context();

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

    void drawImago( const Object* obj, const Imago* parent, int mask );
    void playAudio( const Object* obj, const Audio* parent );

    void updateLoad();

    void load();
    void unload();

    void init();
    void free();

};

extern Context context;

}
}
