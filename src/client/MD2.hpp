/*
 *  MD2.hpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Timer.hpp"

namespace oz
{
namespace Client
{

  class MD2
  {
    public:

      enum AnimEnum
      {
        ANIM_STAND,
        ANIM_RUN,
        ANIM_ATTACK,
        ANIM_PAIN_A,
        ANIM_PAIN_B,
        ANIM_PAIN_C,
        ANIM_JUMP,
        ANIM_FLIP,
        ANIM_SALUTE,
        ANIM_FALLBACK,
        ANIM_WAVE,
        ANIM_POINT,
        ANIM_CROUCH_STAND,
        ANIM_CROUCH_WALK,
        ANIM_CROUCH_ATTACK,
        ANIM_CROUCH_PAIN,
        ANIM_CROUCH_DEATH,
        ANIM_DEATH_FALLBACK,
        ANIM_DEATH_FALLFORWARD,
        ANIM_DEATH_FALLBACKSLOW,
        ANIM_MAX
      };

      struct Anim
      {
        int firstFrame;
        int lastFrame;
        int fps;
      };

      struct AnimState
      {
        int   startFrame;
        int   endFrame;

        int   fps;
        float frameTime;
        float currTime;
        float oldTime;

        int   type;

        int   currFrame;
        int   nextFrame;
      };

    private:

      static float anorms[][3];

      int   nFrames;
      int   nVerts;
      int   nGlCmds;

      Vec3  *verts;
      int   *glCmds;
      int   *lightNormals;

      uint  texId;

      float animInterpol;

      void animate( AnimState *anim, float time );
      void interpolate( AnimState *anim, Vec3 *vertList );
      void renderFrame();

    public:

      static Anim animList[];

      MD2();
      ~MD2();

      bool load( const char *path );

      void drawFrame( int frame );
      void draw( AnimState *anim );

      void scale( float scale );
      void translate( const Vec3 &t );
      void translate( int animType, const Vec3 &t );

      static uint genList( const char *path, float scale = 1.0f, const Vec3 &t = Vec3::zero() );

      void free();
  };

}
}
