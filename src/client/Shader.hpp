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

    private:

      struct SkyLight
      {
        static const SkyLight NONE;

        Vec3 dir;
        Quat colour;
        Quat ambient;
      };

      struct Light
      {
        static const Light NONE;

        Point3 pos;
        Quat   colour;

        explicit Light()
        {}

        explicit Light( const Point3& pos_, const Quat& colour_ ) : pos( pos_ ), colour( colour_ )
        {}
      };

      static const int   BUFFER_SIZE = 8192;
      static const char* PROGRAM_NAMES[MAX];

      uint          vertShaders[MAX];
      uint          fragShaders[MAX];
      uint          programs[MAX];

      Program       activeProgram;

      Quat          ambientLight;
      SkyLight      skyLight;
      Sparse<Light> lights;

      float lightingDistance;

      void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
      void loadProgram( Program prog, const char** sources, int* lengths );

    public:

      void use( Program prog );
      void bindTextures( uint texture0, uint texture1 = 0 ) const;

      void setLightingDistance( float distance );
      void setAmbientLight( const Quat& colour );
      void setSkyLight( const Vec3& dir, const Quat& colour );

      int  addLight( const Point3& pos, const Quat& colour );
      void removeLight( int id );
      void setLight( int id, const Point3& pos, const Quat& colour );

      void updateLights();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Shader shader;

}
}
