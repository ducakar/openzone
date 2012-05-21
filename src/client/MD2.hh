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
 * @file client/MD2.hh
 */

#pragma once

#include "matrix/Bot.hh"

#include "client/Mesh.hh"

namespace oz
{
namespace client
{

class MD2
{
  public:

    static const int MAX_VERTS = 2048;

    enum AnimType
    {
      ANIM_NONE = -1,
      ANIM_STAND,
      ANIM_RUN,
      ANIM_ATTACK,
      ANIM_PAIN_A,
      ANIM_PAIN_B,
      ANIM_PAIN_C,
      ANIM_JUMP,
      ANIM_FLIP,
      ANIM_SALUTE,
      ANIM_WAVE,
      ANIM_FALLBACK,
      ANIM_POINT,
      ANIM_CROUCH_STAND,
      ANIM_CROUCH_WALK,
      ANIM_CROUCH_ATTACK,
      ANIM_CROUCH_PAIN,
      ANIM_CROUCH_DEATH,
      ANIM_DEATH_FALLBACK,
      ANIM_DEATH_FALLFORWARD,
      ANIM_DEATH_FALLBACKSLOW
    };

    struct AnimInfo
    {
      int      firstFrame;
      int      lastFrame;
      float    frameTime;
      AnimType nextType;
    };

    class AnimState
    {
      friend class MD2;

      private:

        const Bot* bot;

        AnimType   type;
        AnimType   nextType;

        int        firstFrame;
        int        lastFrame;
        int        currFrame;
        int        nextFrame;

        float      frameTime;
        float      currTime;

        AnimType extractAnim() const;
        void set( AnimType type );

      public:

        AnimState( const Bot* bot );

        void advance();
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
    Point*  positions;
    Vec3*   normals;

    Mesh    mesh;

    PFile   file;

  public:

    Mat44    weaponTransf;

    bool     isPreloaded;
    bool     isLoaded;

    explicit MD2( int id );
    ~MD2();

    void preload();
    void load();

    void drawFrame( int frame ) const;
    void draw( const AnimState* anim ) const;

};

}
}
