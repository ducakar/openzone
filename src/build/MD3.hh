/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file build/MD3.hh
 */

#pragma once

#include "client/MD3.hh"

#include "build/common.hh"

namespace oz
{
namespace build
{

struct MD3Tag;

/**
 * MD3 class.
 *
 * <a href="http://www.wikipedia.org/MD3_(file_format)">Wiki</a>.
 */
class MD3
{
  private:

    struct MD3Header
    {
      char id[4];
      int  version;
      char fileName[64];
      int  flags;

      int  nFrames;
      int  nTags;
      int  nSurfaces;
      int  nSkins;

      int  offFrames;
      int  offTags;
      int  offSurfaces;
      int  offEnd;
    };

    struct MD3Frame
    {
      float mins[3];
      float maxs[3];
      float pos[3];
      float radius;
      char  name[16];
    };

    struct MD3Tag
    {
      char  name[64];
      float pos[3];
      float rot[9];
    };

    struct MD3Surface
    {
      int  id;
      char name[64];
      int  flags;

      int  nFrames;
      int  nShaders;
      int  nVertices;
      int  nTriangles;

      int  offTriangles;
      int  offShaders;
      int  offTexCoords;
      int  offVertices;
      int  offEnd;
    };

    struct MD3Triangle
    {
      int vertices[3];
    };

    struct MD3Shader
    {
      char name[64];
      int  index;
    };

    struct MD3TexCoord
    {
      float s;
      float t;
    };

    struct MD3Vertex
    {
      short pos[3];
      ubyte normal[2];
    };

    enum Part
    {
      LEGS,
      TORSO,
      HEAD
    };

    client::MD3::AnimInfo legsAnimList[client::MD3::LEGS_ANIM_MAX];
    client::MD3::AnimInfo torsoAnimList[client::MD3::TORSO_ANIM_MAX];
    client::MD3::Joint    joints[client::MD3::MAX_FRAMES][client::MD3::JOINTS_MAX];

    String   sPath;

    float    scale;
    String   skin;
    String   masks;
    String   model;
    int      frame;
    int      lowerFrame;
    int      upperFrame;
    String   shaderName;
    float    specular;

    int      nLowerFrames;
    int      nUpperFrames;
    Mat44    meshTransf;

    void readAnimData();
    void buildMesh( const char* name, int frame );

    void load();
    void save();

    explicit MD3( const char* path );

  public:

    static void build( const char* path );

};

}
}
