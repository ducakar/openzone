/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Shader.hh
 *
 * Shader utilities.
 */

#pragma once

#include "client/common.hh"

namespace oz
{
namespace client
{

struct Param
{
  int oz_ProjModelTransform;
  int oz_ModelTransform;
  int oz_BoneTransforms;

  int oz_CameraPosition;

  int oz_ColourTransform;
  int oz_Textures;

  int oz_CaelumLight_dir;
  int oz_CaelumLight_diffuse;
  int oz_CaelumLight_ambient;

  int oz_Fog_dist;
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

    void ortho( int width, int height );
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
    NORMAL,
    TANGENT,
    BINORMAL,
    BONES,
    BLEND
  };
};

class Shader
{
  public:

    enum Mode
    {
      UI,
      SCENE
    };

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

      Point pos;
      Vec4  diffuse;

      Light() = default;

      explicit Light( const Point& pos, const Vec4& diffuse );
    };

    static const int  LOG_BUFFER_SIZE = 8192;
    static const int  SAMPLER_MAP[];

    static char       logBuffer[LOG_BUFFER_SIZE];

    static String     defines;

    Map<String, uint> vertShaders;
    Map<String, uint> fragShaders;
    DArray<Program>   programs;
    SVector<int, 8>   programStack;

    float             lightingDistance;
    CaelumLight       caelumLight;

    void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
    void loadProgram( int id );

  public:

    Mode  mode;

    int   plain;
    int   mesh;
    int   postprocess;

    int   activeProgram;

    Mat44 colourTransform;
    Vec4  fogColour;

    uint  defaultMasks;
    uint  defaultNormals;

    int   medium;
    bool  hasS3TC;
    bool  hasVertexTexture;
    bool  setSamplerMap;
    bool  doPostprocess;
    bool  isLowDetail;

    Shader();

    void program( int id );
    void colour( const Vec4& colour );
    void colour( const Mat44& colourTransform );

    void setLightingDistance( float distance );
    void setAmbientLight( const Vec4& colour );
    void setCaelumLight( const Vec3& dir, const Vec4& colour );

    void updateLights();

    void init();
    void free();

};

extern Shader shader;

}
}
