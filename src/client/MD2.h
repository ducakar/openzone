/*
 *  MD2.h
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Timer.h"

namespace oz
{
namespace client
{

  class MD2
  {
    private:

      static const int SHADEDOT_QUANT = 8;
      static const int MAX_VERTS      = 2048;

    public:

      struct Anim
      {
        int   firstFrame;
        int   lastFrame;
        float fps;
      };

      struct AnimState
      {
        int   startFrame;
        int   endFrame;

        float fps;
        float frameTime;
        float currTime;
        float oldTime;

        int   type;

        int   currFrame;
        int   nextFrame;
      };

    private:

      static float anorms[][3];
      static Vec3  vertList[MAX_VERTS];

      int          nFrames;
      int          nVerts;

      DArray<Vec3> verts;
      DArray<int>  glCmds;
      DArray<int>  lightNormals;

      uint         texId;

      void interpolate( AnimState *anim, float time ) const;

    public:

      static Anim animList[];

      MD2( const char *name );

      void scale( float scale );
      void translate( const Vec3 &t );
      void translate( int animType, const Vec3 &t );

      void drawFrame( int frame ) const;
      void draw( AnimState *anim ) const;

      static uint genList( const char *name );

  };

}
}
