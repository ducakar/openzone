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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/Mesh.hpp
 */

#pragma once

#include "client/Mesh.hpp"

#include "build/common.hpp"

namespace oz
{
namespace build
{

class MeshData
{
  friend class Compiler;

  public:

    struct Part
    {
      int    component;
      uint   mode;

      String texture;
      float  alpha;
      float  specular;

      int    nIndices;
      int    firstIndex;
    };

    Vector<Part>   parts;

    DArray<ushort> indices;
    DArray<Vertex> vertices;

    void write( BufferStream* stream, bool embedTextures = true ) const;

};

}
}
