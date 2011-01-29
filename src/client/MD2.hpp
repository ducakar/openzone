/*
 *  MD2.hpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  class MD2
  {
    private:

      static const int MAX_VERTS = 2048;

    public:

      struct Anim
      {
        int   firstFrame;
        int   lastFrame;
        float fps;
        int   repeat;
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

      static Anim  animList[];

    private:

      static Vec3   anorms[];
      static Point3 vertList[MAX_VERTS];

      String       name;

      int          nFrames;
      int          nVerts;

      Point3*      verts;
      int*         glCmds;
      int*         lightNormals;

      uint         texId;

      void interpolate( const AnimState* anim ) const;

    public:

      uint         list;
      Vec3         weaponTransl;

      static void init();

      explicit MD2( const char* name );
      ~MD2();

      void optimise();

      void scale( float scale );
      void translate( const Vec3& t );
      void translate( int animType, const Vec3& t );

      void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
      void draw( const AnimState* anim ) const;
      void genList();

  };

  inline void MD2::advance( AnimState* anim, float dt ) const
  {
    anim->currTime += dt;

    while( anim->currTime > anim->frameTime ) {
      anim->currTime -= anim->frameTime;
      anim->currFrame = anim->nextFrame;
      ++anim->nextFrame;

      if( anim->nextFrame > anim->endFrame ) {
        anim->nextFrame = anim->repeat ? anim->startFrame : anim->endFrame;
      }
    }
  }

}
}
