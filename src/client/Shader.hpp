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

#include "client/common.hpp"

namespace oz
{
namespace client
{

  class Shader;

  struct Param
  {
    int oz_Transform_proj;
    int oz_Transform_camera;
    int oz_Transform_projCamera;
    int oz_Transform_model;
    int oz_Transform_complete;

    int oz_Colour;

    int oz_IsTextureEnabled;
    int oz_Textures;
    int oz_TextureScales;

    int oz_SpecularMaterial;

    int oz_SkyLight_dir;
    int oz_SkyLight_diffuse;
    int oz_SkyLight_ambient;

    int oz_PointLights;

    int oz_NearDistance;

    int oz_FogDistance;
    int oz_FogColour;

    int oz_MD2Anim;
  };

  extern Param param;

  class Transform
  {
    friend class Shader;

    private:

      SVector<Mat44, 8> stack;

    public:

      Mat44 proj;
      Mat44 camera;
      Mat44 model;

      Mat44 projCamera;

      OZ_ALWAYS_INLINE
      void push()
      {
        stack.pushLast( model );
      }

      OZ_ALWAYS_INLINE
      void pop()
      {
        model = stack.popLast();
      }

      void ortho();
      void projection();

      void applyCamera();
      void applyModel() const;
      void apply() const;

  };

  extern Transform tf;

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
        TEXT,
        SIMPLE,
        MESH_ITEMVIEW,
        MESH_NEAR,
        MESH_FAR,
        MESH_WATER,
        TERRA,
        TERRA_WATER,
        STARS,
        PARTICLES,
        MD2,
        MAX
      };

    private:

      struct SkyLight
      {
        static const SkyLight NONE;

        Vec3 dir;
        Vec4 diffuse;
        Vec4 ambient;
      };

      struct Light
      {
        static const Light NONE;

        Point3 pos;
        Vec4   diffuse;

        explicit Light( const Point3& pos, const Vec4& diffuse );
      };

      static const int   BUFFER_SIZE = 8192;
      static const char* PROGRAM_NAMES[MAX];

      uint                vertShaders[MAX];
      uint                fragShaders[MAX];
      uint                programs[MAX];
      Param               progParams[MAX];

      Program             activeProgram;
      SVector<Program, 8> programStack;

      float               lightingDistance;
      SkyLight            skyLight;
      Sparse<Light>       lights;

      void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
      void loadProgram( Program prog, const char** sources, int* lengths );

    public:

      Vec4 colour;

      void use( Program prog );
      void push();
      void pop();

      void setLightingDistance( float distance );
      void setAmbientLight( const Vec4& colour );
      void setSkyLight( const Vec3& dir, const Vec4& colour );

      int  addLight( const Point3& pos, const Vec4& colour );
      void removeLight( int id );
      void setLight( int id, const Point3& pos, const Vec4& colour );

      void updateLights();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Shader shader;

}
}
