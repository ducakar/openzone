/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
#include <client/BSPImago.hh>
#include <client/BSPAudio.hh>
#include <client/Imago.hh>
#include <client/Audio.hh>
#include <client/FragPool.hh>
#include <client/PartGen.hh>

namespace oz::client
{

class Context
{
  friend class Loader;
  friend class BSPImago;
  friend class BSPAudio;
  friend class Model;
  friend class Audio;
  friend class Render;
  friend class Sound;

private:

  template <typename Type>
  struct Resource
  {
    Type handle = Type();
    int  nUsers = -1; ///< Number of users or -1 if not loaded.
  };

  struct TextureResource : Resource<Texture>
  {
    struct PreloadData;

    PreloadData* preloadData = nullptr;
  };

  struct SoundResource : Resource<uint>
  {
    struct PreloadData;

    PreloadData* preloadData = nullptr;
  };

  struct Source : ChainNode<Source>
  {
    uint    id;
    int     sound;

    explicit Source(uint sourceId, int sound_)
      : id(sourceId), sound(sound_)
    {}

    static Pool<Source> pool;

    OZ_STATIC_POOL_ALLOC(pool)
  };

  struct ContSource
  {
    uint id;
    int  sound;
    bool isUpdated;
  };

private:

  Imago::CreateFunc**      imagoClasses = nullptr;
  Audio::CreateFunc**      audioClasses = nullptr;
  FragPool**               fragPools    = nullptr;

  TextureResource*         textures     = nullptr;
  SoundResource*           sounds       = nullptr;

  Chain<Source>            sources;               // Non-looping sources.
  HashMap<int, ContSource> contSources;           // Looping sources.

  Chain<PartGen>           partGens;

  Resource<Model*>*        models       = nullptr;
  Resource<PartClass>*     partClasses  = nullptr;

  Resource<BSPImago*>*     bspImagines  = nullptr;
  Resource<BSPAudio*>*     bspAudios    = nullptr;

  HashMap<int, Imago*>     imagines;              // Currently loaded graphics models.
  HashMap<int, Audio*>     audios;                // Currently loaded audio models.

  int                      maxImagines;
  int                      maxAudios;
  int                      maxSources;
  int                      maxContSources;
  int                      maxPartGens;

  int                      maxSMMImagines;
  int                      maxSMMVehicleImagines;
  int                      maxExplosionImagines;
  int                      maxMD2Imagines;
  int                      maxMD2WeaponImagines;

  int                      maxBasicAudios;
  int                      maxBotAudios;
  int                      maxVehicleAudios;

public:

  int                      textureLod;
  bool                     dynamicLoading;

private:

  Source* addSource(int sound);
  void removeSource(Source* source, Source* prev);

  ContSource* addContSource(int sound, int key);
  void removeContSource(ContSource* contSource, int key);

  PartGen* addPartGen();
  void removePartGen(PartGen* partGen);

public:

  static Texture loadTexture(const File& albedoFile, const File& masksFile, const File& normalsFile);
  static Texture loadTexture(const String& basePath);
  static void unloadTexture(const Texture* texture);

  void prepareTexture(int id);
  Texture requestTexture(int id);
  void releaseTexture(int id);

  void prepareSound(int id);
  uint requestSound(int id);
  void releaseSound(int id);

  // Play sample without 3D effects.
  void playSample(int id);

  Model* getModel(int id);
  Model* requestModel(int id);
  void releaseModel(int id);

  PartClass* getPartClass(int id);
  PartClass* requestPartClass(int id);
  void releasePartClass(int id);

  BSPImago* getBSP(const BSP* bsp);
  BSPImago* requestBSP(const BSP* bsp);

  void drawBSP(const Struct* str);
  void playBSP(const Struct* str);

  void drawImago(const Object* obj, const Imago* parent);
  void playAudio(const Object* obj, const Object* parent);
  void drawFrag(const Frag* frag);

  void updateLoad();

  void loadResources();
  void unloadResources();

  void load();
  void unload();

  // Used to remove UI sounds and sources.
  void clearSounds();

  void init();
  void destroy();

};

extern Context context;

}
