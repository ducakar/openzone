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

#include "matrix/Translator.hpp"

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
        float fps;
        int   repeat;
      };

      struct AnimState
      {
        Anim  type;
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

      int     id;

      int     nFrames;
      int     nFrameVerts;

      Mesh    mesh;
      Vertex* frameVerts;
      Point3* vertices;

      void setFrame( int frame ) const;
      void interpolate( const AnimState* anim ) const;

    public:

      static const AnimInfo ANIM_LIST[];

      Vec3    weaponTransl;
      bool    isLoaded;

      static void prebuild( const char* name );

      explicit MD2( int id );
      ~MD2();

      void load();

      void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
      void draw( const AnimState* anim ) const;

  };

}
}
