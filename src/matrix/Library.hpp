/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Library.hpp
 *
 * Map of all resources, object types, scripts etc.
 */

#pragma once

#include "matrix/Struct.hpp"
#include "matrix/ObjectClass.hpp"

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

    HashString<int, 256> textureIndices;
    HashString<int, 256> soundIndices;
    HashString<int, 64>  shaderIndices;
    HashString<int, 16>  terraIndices;
    HashString<int, 16>  caelumIndices;
    HashString<int, 64>  bspIndices;
    HashString<int, 256> modelIndices;
    HashString<int, 16>  nameListIndices;

  public:

    Vector<Resource> textures;
    Vector<Resource> sounds;
    Vector<Resource> shaders;
    Vector<Resource> terras;
    Vector<Resource> caela;
    Vector<Resource> bsps;
    Vector<Resource> models;
    Vector<Resource> nameLists;
    Vector<Resource> musics;

    Vector<BSPClass> bspClasses;
    HashString<ObjectClass::InitFunc, 8> baseClasses;
    HashString<ObjectClass*, 128> classes;

    int textureIndex( const char* name ) const;
    int soundIndex( const char* name ) const;
    int shaderIndex( const char* name ) const;
    int terraIndex( const char* name ) const;
    int caelumIndex( const char* name ) const;
    int bspIndex( const char* name ) const;
    int modelIndex( const char* name ) const;
    int nameListIndex( const char* name ) const;

    const ObjectClass* clazz( const char* name ) const;

    void init();
    void buildInit();
    void free();

};

extern Library library;

}
}
