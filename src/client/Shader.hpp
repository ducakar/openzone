/*
 *  Shader.hpp
 *
 *  Shader utilities
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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
    int oz_Transform_model;
    int oz_Transform_complete;

    int oz_CameraPosition;

    int oz_Colour;
    int oz_Textures;

    int oz_CaelumLight_dir;
    int oz_CaelumLight_diffuse;
    int oz_CaelumLight_ambient;

    int oz_Specular;

    int oz_Fog_start;
    int oz_Fog_end;
    int oz_Fog_colour;

    int oz_WaveBias;
    int oz_Wind;
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
    enum Type
    {
      POSITION,
      TEXCOORD,
#ifdef OZ_BUMPMAP
      NORMAL,
      TANGENT,
      BINORMAL
#else
      NORMAL
#endif
    };
  };

  class Shader
  {
    private:

      struct Program
      {
        uint  vertShader;
        uint  fragShader;
        uint  program;
        Param param;
      };

      struct CaelumLight
      {
        static const CaelumLight NONE;

        Vec3 dir;
        Vec4 diffuse;
        Vec4 ambient;
      };

      struct Light
      {
        static const Light NONE;

        Point3 pos;
        Vec4   diffuse;

        Light()
        {}

        explicit Light( const Point3& pos, const Vec4& diffuse );
      };

      static const int BUFFER_SIZE = 8192;

      static String   defines;

      DArray<Program> programs;
      SVector<int, 8> programStack;

      float           lightingDistance;
      CaelumLight     caelumLight;

      void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
      void loadProgram( int id, const char** sources, int* lengths );

    public:

      int  plain;
      int  mesh;
      int  combine;

      int  activeProgram;

      Vec4 colour;

      bool isInWater;
      bool isLoaded;
      bool hasVertexTexture;

      Shader();

      void use( int id );
      void push();
      void pop();

      void setLightingDistance( float distance );
      void setAmbientLight( const Vec4& colour );
      void setCaelumLight( const Vec3& dir, const Vec4& colour );

      void updateLights();

      void load();
      void unload();

      void init();
      void free();

  };

  extern Shader shader;

}
}
