/*
 *  Shader.hpp
 *
 *  Shader utilities
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

  struct Param
  {
    static int oz_TextureEnabled;
    static int oz_Textures;
    static int oz_TextureScale;

    static int oz_DiffuseMaterial;
    static int oz_SpecularMaterial;

    static int oz_AmbientLight;
    static int oz_SkyLight;
    static int oz_PointLights;
  };

  struct Attr
  {
    enum Type : int
    {
      POSITION,
      NORMAL,
      TEXCOORD
    };
  };

  class Shader
  {
    public:

      enum Program : int
      {
        UI,
        DEFAULT,
        STARS,
        MAX
      };

      struct Light
      {
        static const Light NONE;

        float pos[3];
        float colour[3];

        explicit Light()
        {}

        explicit Light( const Point3& pos_, const Quat& colour_ )
        {
          pos[0] = pos_.x;
          pos[1] = pos_.y;
          pos[2] = pos_.z;

          colour[0] = colour_.x;
          colour[1] = colour_.y;
          colour[2] = colour_.z;
        }

        explicit Light( const Vec3& dir_, const Quat& colour_ )
        {
          pos[0] = dir_.x;
          pos[1] = dir_.y;
          pos[2] = dir_.z;

          colour[0] = colour_.x;
          colour[1] = colour_.y;
          colour[2] = colour_.z;
        }

        operator const float* () const
        {
          return pos;
        }
      };

    private:

      static const int   BUFFER_SIZE = 8192;
      static const char* PROGRAM_NAMES[MAX];

      uint vertShaders[MAX];
      uint fragShaders[MAX];
      uint programs[MAX];

      Program activeProgram;

      void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
      void loadProgram( Program prog, const char** sources, int* lengths );

    public:

      void bindTextures( uint texture0, uint texture1 = 0 ) const;
      void use( Program prog );

      void load();
      void unload();

      void init();
      void free();

  };

  extern Shader shader;

}
}
