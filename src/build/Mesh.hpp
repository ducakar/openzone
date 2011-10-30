/*
 *  Mesh.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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

    void write( OutputStream* stream, bool embedTextures = true ) const;

};

}
}
