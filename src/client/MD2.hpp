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

#include "client/Mesh.hpp"

namespace oz
{
namespace client
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

    static const AnimInfo ANIM_LIST[];

  private:

    static Vertex animBuffer[MAX_VERTS];

    int     id;

    int     nFrames;
    int     nFrameVertices;
    int     nFramePositions;

    uint    vertexTexId;
    uint    normalTexId;
    int     shaderId;

    Vertex* vertices;
    Vec4*   positions;
    Vec4*   normals;

    Mesh    mesh;

  public:

    Mat44   weaponTransf;
    bool    isLoaded;

    explicit MD2( int id );
    ~MD2();

    void load();

    void advance( AnimState* anim, float dt ) const;

    void drawFrame( int frame ) const;
    void draw( const AnimState* anim ) const;

};

}
}
