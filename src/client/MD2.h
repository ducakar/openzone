/*
 *  MD2.h
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{
namespace client
{

  struct MD2
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

      static float anorms[][3];
      static Vec3  vertList[MAX_VERTS];

      String       name;

      int          nFrames;
      int          nVerts;

      Vec3*        verts;
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

      if( anim->nextFrame < anim->endFrame ) {
        anim->nextFrame++;
      }
      else if( anim->repeat ) {
        anim->nextFrame = anim->startFrame;
      }
    }
  }

}
}
