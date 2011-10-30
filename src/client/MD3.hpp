/*
 *  MD3.hpp
 *
 *  http://www.wikipedia.org/MD3_(file_format)
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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

};

}
}
