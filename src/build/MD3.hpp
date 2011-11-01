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
 * @file build/MD3.hpp
 */

#pragma once

#include "build/common.hpp"

namespace oz
{
namespace build
{

struct MD3Tag;

/**
 * MD3 class.
 *
 * http://www.wikipedia.org/MD3_(file_format)
 */
class MD3
{
  private:

    static const int MAX_FRAMES = 256;

  public:

    enum LegsAnim
    {
      LEGS_DEATH1,
      LEGS_DEAD1,
      LEGS_DEATH2,
      LEGS_DEAD2,
      LEGS_DEATH3,
      LEGS_DEAD3,

      LEGS_WALKCR,
      LEGS_WALK,
      LEGS_RUN,
      LEGS_BACK,
      LEGS_SWIM,
      LEGS_JUMP,
      LEGS_LAND,
      LEGS_JUMPB,
      LEGS_LANDB,
      LEGS_IDLE,
      LEGS_IDLECR,
      LEGS_TURN,

      LEGS_ANIM_MAX
    };

    enum TorsoAnim
    {
      TORSO_DEATH1,
      TORSO_DEAD1,
      TORSO_DEATH2,
      TORSO_DEAD2,
      TORSO_DEATH3,
      TORSO_DEAD3,

      TORSO_GESTURE,
      TORSO_ATTACK,
      TORSO_ATTACK2,
      TORSO_DROP,
      TORSO_RAISE,
      TORSO_STAND,
      TORSO_STAND2,

      TORSO_ANIM_MAX
    };

    struct AnimInfo
    {
      int   firstFrame;
      int   lastFrame;
      int   repeat;
      float fps;
    };

    struct AnimState
    {
      int   type;
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

    enum JointType
    {
      JOINT_HIP,
      JOINT_NECK,
      JOINT_WEAPON,

      JOINTS_MAX
    };

    struct Joint
    {
      Quat rot;
      Vec3 transl;
    };

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

    static AnimInfo legsAnimList[LEGS_ANIM_MAX];
    static AnimInfo torsoAnimList[TORSO_ANIM_MAX];
    static Joint    joints[MAX_FRAMES][JOINTS_MAX];

    static String   sPath;
    static Config   config;

    static float    scale;
    static Mat44    meshTransf;
    static int      nTags;

    static DArray<MD3Tag> tags;

    static void  readAnimData();
    static Joint toJoint( const MD3Tag* tag );
    static void  buildMesh( const char* name, int frame );

  public:

    static void build( const char* path );

};

}
}
