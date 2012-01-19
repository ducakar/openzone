/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "matrix/Library.hh"

#include "client/Shader.hh"
#include "client/BSP.hh"
#include "client/Imago.hh"
#include "client/Audio.hh"
#include "client/FragPool.hh"

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

    Imago::CreateFunc*                imagoClasses;
    Audio::CreateFunc*                audioClasses;

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

    HashIndex<Imago*, 10223>          imagines;  // currently loaded graphics models
    HashIndex<Audio*, 3067>           audios;    // currently loaded audio models
    HashIndex<FragPool*, 32>          fragPools; // currently loaded frag pools representations

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

    int                               maxFragPools;

    void addSource( uint srcId, int sound );
    void addBSPSource( uint srcId, int sound, int key );
    void addObjSource( uint srcId, int sound, int key );

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
    void freeSound( int id );

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

    void drawFrag( const Frag* frag );

    void updateLoad();

    void load();
    void unload();

    void init();
    void free();

};

extern Context context;

}
}