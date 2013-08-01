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
 * @file matrix/Liber.hh
 */

#pragma once

#include <matrix/BSP.hh>
#include <matrix/FragPool.hh>

namespace oz
{

/**
 * Mapping of all resources, object types, scripts etc.
 */
class Liber
{
  public:

    struct Resource
    {
      String name;
      String path;
    };

  private:

    HashMap<String, BSP, 64>                     bsps;
    HashMap<String, ObjectClass::CreateFunc*, 8> baseClasses;
    HashMap<String, ObjectClass*, 128>           objClasses;
    HashMap<String, FragPool, 32>                fragPools;

    HashMap<String, int, 64>  shaderIndices;
    HashMap<String, int, 256> textureIndices;
    HashMap<String, int, 256> soundIndices;
    HashMap<String, int, 16>  caelumIndices;
    HashMap<String, int, 16>  terraIndices;
    HashMap<String, int, 256> modelIndices;

    HashMap<String, int, 16>  nameListIndices;
    HashMap<String, int, 64>  musicTrackIndices;

    HashMap<String, int, 8>   deviceIndices;
    HashMap<String, int, 16>  imagoIndices;
    HashMap<String, int, 8>   audioIndices;

  public:

    DArray<Resource> shaders;
    DArray<Resource> textures;
    DArray<Resource> sounds;
    DArray<Resource> caela;
    DArray<Resource> terrae;
    DArray<Resource> models;
    DArray<Resource> nameLists;
    DArray<Resource> musicTracks;

    int              nBSPs;
    int              nFragPools;
    int              nDeviceClasses;
    int              nImagoClasses;
    int              nAudioClasses;

    bool             mapMP3s;
    bool             mapAACs;

    const FragPool*    fragPool( const char* name ) const;
    const ObjectClass* objClass( const char* name ) const;
    const BSP*         bsp( const char* name ) const;

    int shaderIndex( const char* name ) const;
    int textureIndex( const char* name ) const;
    int soundIndex( const char* name ) const;
    int caelumIndex( const char* name ) const;
    int terraIndex( const char* name ) const;
    int modelIndex( const char* name ) const;

    int nameListIndex( const char* name ) const;
    int musicTrackIndex( const char* name ) const;

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
    void initModels();
    void initNameLists();
    void initFragPools();
    void initClasses();
    void initBSPs();
    void initMusicRecurse( const char* path, List<Resource>* musicTracksList );
    void initMusic( const char* userMusicPath );

  public:

    void init( const char* userMusicPath );
    void destroy();

};

extern Liber liber;

}
