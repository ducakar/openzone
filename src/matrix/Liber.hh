/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

  List<Resource> shaders;
  List<Resource> textures;
  List<Resource> sounds;
  List<Resource> caela;
  List<Resource> terrae;
  List<Resource> parts;
  List<Resource> models;
  List<Resource> nameLists;
  List<Resource> minds;
  List<Resource> musicTracks;

  int            nBSPs;
  int            nFragPools;
  int            nDeviceClasses;
  int            nImagoClasses;
  int            nAudioClasses;

  bool           mapMP3s;
  bool           mapAACs;

  const BSP*         bsp( const char* name ) const;
  const ObjectClass* objClass( const char* name ) const;
  const FragPool*    fragPool( const char* name ) const;

  int shaderIndex( const char* name ) const;
  int textureIndex( const char* name ) const;
  int soundIndex( const char* name ) const;
  int caelumIndex( const char* name ) const;
  int terraIndex( const char* name ) const;
  int partIndex( const char* name ) const;
  int modelIndex( const char* name ) const;

  int nameListIndex( const char* name ) const;
  int mindIndex( const char* name ) const;
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
  void initParticles();
  void initModels();
  void initNameLists();
  void initMinds();
  void initFragPools();
  void initClasses();
  void initBSPs();
  void initMusicRecurse( const char* path );
  void initMusic( const char* userMusicPath );

public:

  void init( const char* userMusicPath );
  void destroy();

};

extern Liber liber;

}
