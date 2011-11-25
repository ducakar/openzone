/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Library.hpp
 */

#pragma once

#include "matrix/Struct.hpp"
#include "matrix/ObjectClass.hpp"
#include "matrix/FragPool.hpp"

namespace oz
{
namespace matrix
{

struct Anim
{
  enum Type
  {
    STAND,
    RUN,
    ATTACK,
    PAIN_A,
    PAIN_B,
    PAIN_C,
    JUMP,
    FLIP,
    SALUTE,
    FALLBACK,
    WAVE,
    POINT,
    CROUCH_STAND,
    CROUCH_WALK,
    CROUCH_ATTACK,
    CROUCH_PAIN,
    CROUCH_DEATH,
    DEATH_FALLBACK,
    DEATH_FALLFORWARD,
    DEATH_FALLBACKSLOW,
    MAX
  };
};

/**
 * Mapping of all resources, object types, scripts etc.
 */
class Library
{
  public:

    struct Resource
    {
      String name;
      String path;

      Resource() = default;

      explicit Resource( const String& name, const String& path );
    };

  private:

    HashString<const ObjectClass::CreateFunc, 8> baseClasses;
    HashString<ObjectClass*, 128> objClasses;
    HashString<BSP, 64>           bsps;
    HashString<FragPool, 32>      fragPools;

    HashString<int, 64>  shaderIndices;
    HashString<int, 256> textureIndices;
    HashString<int, 256> soundIndices;
    HashString<int, 16>  caelumIndices;
    HashString<int, 16>  terraIndices;
    HashString<int, 256> modelIndices;
    HashString<int, 16>  nameListIndices;

    HashString<int, 8>   deviceIndices;
    HashString<int, 16>  imagoIndices;
    HashString<int, 8>   audioIndices;

  public:

    Vector<Resource> shaders;
    Vector<Resource> textures;
    Vector<Resource> sounds;
    Vector<Resource> caela;
    Vector<Resource> terras;
    Vector<Resource> models;
    Vector<Resource> nameLists;
    Vector<Resource> musics;

    int nBSPs;
    int nDeviceClasses;
    int nImagoClasses;
    int nAudioClasses;

    const BSP*         bsp( const char* name ) const;
    const ObjectClass* objClass( const char* name ) const;
    const FragPool*    fragPool( const char* name ) const;

    int shaderIndex( const char* name ) const;
    int textureIndex( const char* name ) const;
    int soundIndex( const char* name ) const;
    int caelumIndex( const char* name ) const;
    int terraIndex( const char* name ) const;
    int modelIndex( const char* name ) const;
    int nameListIndex( const char* name ) const;

    int deviceIndex( const char* name ) const;
    int imagoIndex( const char* name ) const;
    int audioIndex( const char* name ) const;

    void freeBSPs();

  private:

    void initShaders();
    void initTextures();
    void initBuildTextures();
    void initSounds();
    void initCaela();
    void initBuildCaela();
    void initTerrae();
    void initBuildTerrae();
    void initBSPs();
    void initBuildBSPs();
    void initModels();
    void initBuildModels();
    void initMusic();
    void initNameLists();
    void initFragPools();
    void initClasses();

  public:

    void init();
    void buildInit();
    void free();

};

extern Library library;

}
}
