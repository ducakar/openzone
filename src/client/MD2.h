/*
 *  MD2.h
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/Timer.h"

namespace oz
{
namespace client
{

  class MD2
  {
    friend class MD2Model;

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

    private:

      static float anorms[][3];
      static Anim  animList[];
      static Vec3  vertList[MAX_VERTS];

      String       name;

      int          nFrames;
      int          nVerts;

      Vec3         *verts;
      int          *glCmds;
      int          *lightNormals;

      uint         texId;

      void interpolate( AnimState *anim, float dt ) const;

    public:

      uint         list;

      static void init();

      MD2( const char *name );
      ~MD2();

      void scale( float scale );
      void translate( const Vec3 &t );
      void translate( int animType, const Vec3 &t );

      void drawFrame( int frame ) const;
      void draw( AnimState *anim ) const;
      void genList();

  };

}
}
