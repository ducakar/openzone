/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
  int projCamera;
  int model;
  int modelRot;
  int cameraPos;
  int bones;
  int meshAnimation;

  int colour;
  int texture;
  int masks;
  int normals;
  int envMap;
  int vertexAnim;
  int shininess;
  int nLights;

  int caelumLight_dir;
  int caelumLight_ambient;
  int caelumLight_colour;

  int fog_colour;
  int fog_distance2;

  int caelumColour;
  int caelumLuminance;
  int waveBias;
  int wind;
};

extern Uniform uniform;

class Transform
{
  friend class Shader;

private:

  SList<Mat4, 8> stack;

public:

  Mat4 proj;
  Mat4 camera;
  Mat4 model;

  Mat4 colour;

  OZ_ALWAYS_INLINE
  void push()
  {
    stack.pushLast(model);
  }

  OZ_ALWAYS_INLINE
  void pop()
  {
    model = stack.popLast();
  }

  void ortho(int width, int height);
  void projection();

  void applyCamera();
  void apply() const;

  void applyColour() const;

  void setColour(const Mat4& colour) const;
  void setColour(const Vec4& colour) const;
  void setColour(float r, float g, float b, float a = 1.0f) const;

};

extern Transform tf;

struct Attrib
{

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
    Vec4 ambient;
    Vec4 colour;
  };

  struct Light
  {
    Point pos;
    Vec4  colour;

    Light() = default;
    explicit Light(const Point& pos, const Vec4& colour);
  };

public:

  enum Attrib
  {
    POSITION,
    TEXCOORD,
    NORMAL,
    TANGENT,
    BINORMAL
  };

  enum Sampler
  {
    DIFFUSE     = GL_TEXTURE0,
    MASKS       = GL_TEXTURE1,
    NORMALS     = GL_TEXTURE2,
    ENV_MAP     = GL_TEXTURE3,
    VERTEX_ANIM = GL_TEXTURE4
  };

private:

  Map<String, uint> vertShaders;
  Map<String, uint> fragShaders;
  List<Program>     programs;

  float             lightingDistance;
  CaelumLight       caelumLight;

  void compileShader(uint shaderId, const String& defines, const File& file) const;
  void loadProgram(int id);

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
  bool hasFBO;
  bool hasVTF;
  bool hasS3TC;
  bool doVertexEffects;
  bool doEnvMap;
  bool doBumpMap;
  bool doPostprocess;
  bool nLights;

  Shader();

  void program(int id);

  void setLightingDistance(float distance);
  void setAmbientLight(const Vec4& colour);
  void setCaelumLight(const Vec3& dir, const Vec4& colour);

  void updateLights();

  void init();
  void destroy();

};

extern Shader shader;

}
}
