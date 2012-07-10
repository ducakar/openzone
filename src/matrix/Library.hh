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
 * @file matrix/Library.hh
 */

#pragma once

#include "matrix/Struct.hh"
#include "matrix/FragPool.hh"

namespace oz
{
namespace matrix
{

/**
 * Mapping of all resources, object types, scripts etc.
 *
 * @ingroup matrix
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

    HashString<BSP, 64>                     bsps;
    HashString<ObjectClass::CreateFunc*, 8> baseClasses;
    HashString<ObjectClass*, 128>           objClasses;
    HashString<FragPool, 32>                fragPools;

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

    List<Resource> shaders;
    List<Resource> textures;
    List<Resource> sounds;
    List<Resource> caela;
    List<Resource> terrae;
    List<Resource> models;
    List<Resource> nameLists;
    List<Resource> musics;

    int nBSPs;
    int nFragPools;
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
    void initSounds();
    void initCaela();
    void initTerrae();
    void initBSPs();
    void initModels();
    void initMusicRecurse( const char* path );
    void initMusic( const char* userMusicPath );
    void initNameLists();
    void initFragPools();
    void initClasses();

  public:

    void init( const char* userMusicPath );
    void free();

};

extern Library library;

}
}
