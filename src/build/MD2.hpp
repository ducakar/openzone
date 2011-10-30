/*
 *  MD2.hpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Library.hpp"

#include "build/common.hpp"
#include "client/Mesh.hpp"

namespace oz
{
namespace build
{

class MD2
{
  private:

    static const int MAX_VERTS = 2048;

  public:

    struct AnimInfo
    {
      int   firstFrame;
      int   lastFrame;
      int   repeat;
      float fps;
    };

    struct AnimState
    {
      Anim::Type type;
      int   repeat;

      int   startFrame;
      int   endFrame;
      int   currFrame;
      int   nextFrame;

      float fps;
      float frameTime;
      float currTime;
    };

  private:

    struct MD2Header
    {
      char id[4];
      int  version;

      int  skinWidth;
      int  skinHeight;
      int  frameSize;

      int  nSkins;
      int  nFramePositions;
      int  nTexCoords;
      int  nTriangles;
      int  nGlCmds;
      int  nFrames;

      int  offSkins;
      int  offTexCoords;
      int  offTriangles;
      int  offFrames;
      int  offGLCmds;
      int  offEnd;
    };

    struct MD2Vertex
    {
      ubyte p[3];
      ubyte normal;
    };

    struct MD2TexCoord
    {
      short s;
      short t;
    };

    struct MD2Frame
    {
      float     scale[3];
      float     translate[3];
      char      name[16];
      MD2Vertex verts[1];
    };

    struct MD2Triangle
    {
      short vertices[3];
      short texCoords[3];
    };

    static const Vec3 NORMALS[];

  public:

    static void build( const char* path );

};

}
}
