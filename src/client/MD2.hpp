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
        Anim::Type type;
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
      int     nFramePositions;

      uint    vertexTexId;
      uint    normalTexId;
      int     shaderId;

      Mesh    mesh;

    public:

      static const AnimInfo ANIM_LIST[];

      Mat44   weaponTransf;
      bool    isLoaded;

#ifdef OZ_BUILD_TOOLS
      static void prebuild( const char* name );
#endif

      explicit MD2( int id );
      ~MD2();

      void load();

      void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
      void draw( const AnimState* anim ) const;

  };

}
}
