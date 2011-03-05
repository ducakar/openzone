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
    int oz_IsTextureEnabled;
    int oz_Textures;
    int oz_TextureScales;

    int oz_DiffuseMaterial;
    int oz_SpecularMaterial;
    int oz_AmbientLight;
    int oz_SkyLight;
    int oz_PointLights;

    int oz_FogDistance;
    int oz_FogColour;

    int oz_IsHighlightEnabled;
  };

  extern Param param;

  struct Attrib
  {
    enum Type : int
    {
      POSITION,
      TEXCOORD,
      NORMAL
//       TANGENT,
//       BINORMAL
    };
  };

  class Shader
  {
    public:

      enum Program : int
      {
        UI,
        MESH_ITEMVIEW,
        MESH_NEAR,
        MESH_FAR,
        MESH_WATER,
        TERRA,
        TERRA_WATER,
        STARS,
        PARTICLES,
        MAX
      };

    private:

      struct SkyLight
      {
        static const SkyLight NONE;

        float dir[3];
        float diffuse[3];
        float ambient[3];
      };

      struct Light
      {
        static const Light NONE;

        float pos[3];
        float diffuse[3];

        explicit Light( const Point3& pos, const Vec3& diffuse );
      };

      static const int   BUFFER_SIZE = 8192;
      static const char* PROGRAM_NAMES[MAX];

      uint          vertShaders[MAX];
      uint          fragShaders[MAX];
      uint          programs[MAX];
      Param         progParams[MAX];

      Program       activeProgram;

      uint          textures[2];

      float         lightingDistance;
      SkyLight      skyLight;
      Sparse<Light> lights;

      void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
      void loadProgram( Program prog, const char** sources, int* lengths );

    public:

      void use( Program prog );

      void setLightingDistance( float distance );
      void setAmbientLight( const Vec3& colour );
      void setSkyLight( const Vec3& dir, const Vec3& colour );

      int  addLight( const Point3& pos, const Vec3& colour );
      void removeLight( int id );
      void setLight( int id, const Point3& pos, const Vec3& colour );

      void updateLights();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Shader shader;

}
}
