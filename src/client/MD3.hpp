/*
 *  MD3.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

  struct MD3Tag;

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

#ifndef OZ_TOOLS


      AnimInfo legsAnimList[LEGS_ANIM_MAX];
      AnimInfo torsoAnimList[TORSO_ANIM_MAX];
      Joint    joints[MAX_FRAMES][JOINTS_MAX];

      int id;

    public:

      Mat44    weaponTransf;
      bool     isLoaded;

      explicit MD3( int id );
      ~MD3();

      void load();

//       void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
//       void draw( const AnimState* anim ) const;

#else // OZ_TOOLS

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

      static void prebuild( const char* path );

#endif // OZ_TOOLS

  };

}
}
