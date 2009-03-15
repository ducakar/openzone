/*
 *  MD3.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.hpp"
#include "matrix/Timer.hpp"
#include "Context.hpp"

namespace oz
{
namespace Client
{

  struct MD3Header
  {
    int  id;
    int  version;

    char fileName[68];
    int  nFrames;
    int  nTags;
    int  nMeshes;
    int  nSkins;
    int  meshHeaderSize;

    int  offTagStart;
    int  offTagEnd;
    int  fileSize;
  };

  struct MD3Mesh
  {
    char id[4];
    char name[68];

    int  nFrames;
    int  nSkins;
    int  nVertices;
    int  nTriangles;

    int  offTriangles;
    int  headerSize;
    int  offUV;
    int  offVertices;
    int  meshSize;
  };

  struct MD3Tag
  {
    char  name[64];
    Vec3  p;
    Mat33 rot;
  };

  struct MD3Bone : Bounds
  {
    Vec3  p;
    float scale;
    char  creator[16];
  };

  struct MD3Triangle
  {
    short vertex[3];
    ubyte normal[2];
  };

  struct MD3Face
  {
    int vertIndices[3];
  };

  struct MD3TexCoord
  {
    float texCoord[2];
  };

  struct MD3Skin
  {
    char name[68];
  };

  class MD3
  {
    private:

      MD3Skin     *skins;
      MD3TexCoord *texCoords;
      MD3Face     *faces;
      MD3Triangle *triangles;
      MD3Bone     *bones;

    public:

      MD3();
      ~MD3();

      bool load( const char *path );
      void free();

      void draw();

  };

}
}
