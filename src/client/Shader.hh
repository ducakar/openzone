/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <client/common.hh>

namespace oz
{
namespace client
{

struct Uniform
{
  int projModelTransform;
  int modelTransform;
  int boneTransforms;
  int meshAnimation;

  int colourTransform;
  int textures;
  int environment;

  int caelumLight_dir;
  int caelumLight_diffuse;
  int caelumLight_ambient;
  int cameraPosition;

  int fog_dist;
  int fog_colour;

  int starsColour;
  int waveBias;
  int wind;
};

extern Uniform uniform;

class Transform
{
  friend class Shader;

private:

  SList<Mat44, 8> stack;

public:

  Mat44 proj;
  Mat44 camera;
  Mat44 model;

  Mat44 projCamera;

  Mat44 colour;

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

  void applyColour() const;

  void setColour( const Mat44& colour ) const;
  void setColour( const Vec4& colour ) const;
  void setColour( float r, float g, float b, float a = 1.0f ) const;

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
    COLOUR
  };
};

class Shader
{
private:

  struct Program
  {
    uint    vertShader;
    uint    fragShader;
    uint    program;
    Uniform uniform;
  };

  struct CaelumLight
  {
    Vec3 dir;
    Vec4 diffuse;
    Vec4 ambient;
  };

  struct Light
  {
    Point pos;
    Vec4  diffuse;

    explicit Light() = default;
    explicit Light( const Point& pos, const Vec4& diffuse );
  };

  static const int  LOG_BUFFER_SIZE = 8192;
  static const int  SAMPLER_MAP[];

  static char       logBuffer[LOG_BUFFER_SIZE];

  static String     defines;

  Map<String, uint> vertShaders;
  Map<String, uint> fragShaders;
  DArray<Program>   programs;
  SList<int, 8>     programStack;

  float             lightingDistance;
  CaelumLight       caelumLight;

  void compileShader( uint shaderId, const char* defines, const char* path ) const;
  void loadProgram( int id );

public:

  int  plain;
  int  mesh;
  int  postprocess;

  int  activeProgram;

  Vec4 fogColour;

  uint defaultTexture;
  uint defaultMasks;
  uint defaultNormals;
  uint noiseTexture;

  int  medium;
  bool hasS3TC;
  bool hasVertexTexture;
  bool setSamplerMap;
  bool isLowDetail;
  bool doEnvMap;
  bool doBumpMap;
  bool doPostprocess;
  bool nLights;

  explicit Shader();

  void program( int id );

  void setLightingDistance( float distance );
  void setAmbientLight( const Vec4& colour );
  void setCaelumLight( const Vec3& dir, const Vec4& colour );

  void updateLights();

  void init();
  void destroy();

};

extern Shader shader;

}
}
