/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Shader.hh
 *
 * `Shader` class.
 */

#pragma once

#include "GL.hh"

namespace oz
{

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

class Shader
{
  public:

    enum Mode
    {
      UI,
      SCENE
    };

    enum Type
    {
      POSITION,
      TEXCOORD,
      NORMAL
    };

  public:

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

    SList<int, 8>     programStack;

    float             lightingDistance;
    CaelumLight       caelumLight;

    static bool readFile( const File& file, OutputStream* ostream, List<int>* fileLengths );
    static bool loadShader( const File& file, GLenum type );
    void compileShader( uint id, const char* path, const char** sources, int* lengths ) const;
    void loadProgram( const char* name );

  public:

    Mode  mode;

    int   plain;
    int   mesh;
    int   postprocess;

    int   activeProgram;

    Vec4  fogColour;

    uint  defaultTexture;
    uint  defaultMasks;
    uint  defaultNormals;

    int   medium;
    bool  hasS3TC;
    bool  hasVertexTexture;
    bool  setSamplerMap;
    bool  doPostprocess;
    bool  isLowDetail;

    explicit Shader();

    void program( int id );

    void setLightingDistance( float distance );
    void setAmbientLight( const Vec4& colour );
    void setCaelumLight( const Vec3& dir, const Vec4& colour );

    void setColour( const Mat44& colourTransform );
    void setColour( const Vec4& colour );

    void updateLights();

    void init();
    void destroy();

};

}
