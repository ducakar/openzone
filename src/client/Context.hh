/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Context.hh
 */

#pragma once

#include <client/Shader.hh>
#include <client/BSP.hh>
#include <client/BSPAudio.hh>
#include <client/Imago.hh>
#include <client/Audio.hh>
#include <client/FragPool.hh>

namespace oz
{
namespace client
{

class Context
{
  friend class Loader;
  friend class BSP;
  friend class BSPAudio;
  friend class Audio;
  friend class Render;
  friend class Sound;

  private:

    // default audio format
    static const uint INVALID_SOURCE = ~0u;

    template <typename Type>
    struct Resource
    {
      Type handle;
      int  nUsers; ///< Number of users or -1 if not loaded.
    };

    struct Source
    {
      uint    id;
      int     sound;
      Source* next[1];

      explicit Source( uint sourceId, int sound_ ) :
        id( sourceId ), sound( sound_ )
      {}

      static Pool<Source> pool;

      OZ_STATIC_POOL_ALLOC( pool )
    };

    struct ContSource
    {
      uint id;
      int  sound;
      bool isUpdated;

      explicit ContSource( uint sourceId, int sound_ ) :
        id( sourceId ), sound( sound_ ), isUpdated( true )
      {}
    };

    struct SpeakSource
    {
      static const int BUFFER_SIZE = 22050;

      uint          id;
      uint          bufferIds[2];
      int           nQueuedBuffers;
      int           nSamples;

      volatile int  owner;
      volatile bool isAlive;                  ///< Set to false to terminate source prematurely.
      Mutex         mutex;
      Thread        thread;

      String        text;
      short         samples[2 * BUFFER_SIZE];
    };

  private:

    Imago::CreateFunc**        imagoClasses;
    Audio::CreateFunc**        audioClasses;

    Resource<Texture>*         textures;
    Resource<uint>*            sounds;

    Chain<Source>              sources;         // non-looping sources
    HashMap<int, ContSource>   contSources;     // looping sources

    Resource<BSP*>*            bsps;
    Resource<BSPAudio*>*       bspAudios;

    Resource<SMM*>*            smms;

    HashMap<int, Imago*, 8192> imagines;        // currently loaded graphics models
    HashMap<int, Audio*, 4096> audios;          // currently loaded audio models
    FragPool**                 fragPools;       // frag pool representations

    int                        maxImagines;
    int                        maxAudios;
    int                        maxSources;
    int                        maxContSources;

    int                        maxSMMImagines;
    int                        maxSMMVehicleImagines;
    int                        maxExplosionImagines;
    int                        maxMD2Imagines;
    int                        maxMD2WeaponImagines;

    int                        maxBasicAudios;
    int                        maxBotAudios;
    int                        maxVehicleAudios;

    int                        maxFragPools;

    static int                 speakSampleRate; // Set from Sound class.
    static SpeakSource         speakSource;

  public:

    int                        textureLod;

  private:

    static int speakCallback( short int* samples, int nSamples, void* );
    static void speakMain( void* );

    uint addSource( int sound );
    void removeSource( Source* source, Source* prev );

    uint addContSource( int sound, int key );
    void removeContSource( ContSource* contSource, int key );

    uint requestSpeakSource( const char* text, int owner );
    void releaseSpeakSource();

  public:

    explicit Context();

    static Texture loadTexture( const File& diffuseFile, const File& masksFile,
                                const File& normalsFile );
    static Texture loadTexture( const char* basePath );
    static void unloadTexture( const Texture* texture );

    Texture requestTexture( int id );
    void releaseTexture( int id );

    uint requestSound( int id );
    void releaseSound( int id );
    void freeSound( int id );

    // Play sample without 3D effects.
    void playSample( int id );

    BSP* getBSP( const oz::BSP* bsp );
    BSP* requestBSP( const oz::BSP* bsp );

    void drawBSP( const Struct* str );
    void playBSP( const Struct* str );

    SMM* requestModel( int id );
    void releaseModel( int id );

    void drawImago( const Object* obj, const Imago* parent );
    void playAudio( const Object* obj, const Object* parent );

    void drawFrag( const Frag* frag );

    void updateLoad();

    void load();
    void unload();

    // Used to remove UI sounds and sources.
    void clearSounds();

    void init();
    void destroy();

};

extern Context context;

}
}
