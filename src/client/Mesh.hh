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
 * @file client/Mesh.hh
 */

#pragma once

#include <client/Shader.hh>

namespace oz
{
namespace client
{

struct Vertex
{
  float pos[3];
  float texCoord[2];
  float normal[3];

  static void setFormat();
};

struct Texture
{
  int  id;      ///< -2: external, -1: default, 0+ BSP texture index.
  uint diffuse;
  uint masks;
  uint normals;

  explicit Texture() :
    id( -1 ), diffuse( shader.defaultTexture ), masks( shader.defaultMasks ),
    normals( shader.defaultNormals )
  {}
};

class Mesh
{
  public:

    static const int COMPONENT_MASK  = 0x00ff; ///< Mask to get number of mesh components.
    static const int EMBEDED_TEX_BIT = 0x0100; ///< Textures are embedded into SMM file.
    static const int SOLID_BIT       = 0x0200; ///< Mesh component is not transparent.
    static const int ALPHA_BIT       = 0x0400; ///< Mesh component is transparent.

    static const int DIFFUSE_BIT     = 0x1000; ///< Texture has base per-pixel colours.
    static const int MASKS_BIT       = 0x2000; ///< Texture has specular and emission masks.
    static const int NORMALS_BIT     = 0x4000; ///< Texture has normal map.

  private:

    struct Part
    {
      uint mode;
      int  flags;
      int  texture;

      int  nIndices;
      int  firstIndex;
    };

    struct Instance
    {
      Mat44 transform;
      float alpha;
      int   component;
      int   firstFrame;
      int   secondFrame;
      float interpolation;

      OZ_ALWAYS_INLINE
      explicit Instance() = default;

      OZ_ALWAYS_INLINE
      explicit Instance( const Mat44& transform_, float alpha_, int component_ ) :
        transform( transform_ ), alpha( alpha_ ), component( component_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Instance( const Mat44& transform_, float alpha_, int component_,
                         int firstFrame_, int secondFrame_, float interpolation_ ) :
        transform( transform_ ), alpha( alpha_ ), component( component_ ),
        firstFrame( firstFrame_ ), secondFrame( secondFrame_ ), interpolation( interpolation_ )
      {}
    };

    struct PreloadData;

  private:

    static Set<Mesh*> loadedMeshes;

    static Vertex*    vertexAnimBuffer;
    static int        vertexAnimBufferLength;

    uint              vbo;
    uint              ibo;
    int               shaderId;

    int               flags;
    DArray<Texture>   textures;
    DArray<Part>      parts;
    DArray<int>       componentIndices;

    uint              animationTexId;

    int               nTextures;
    int               nVertices;
    int               nIndices;
    int               nFrames;
    int               nFramePositions;

    Vertex*           vertices;
    Point*            positions;
    Vec3*             normals;

    List<Instance>    instances;
    PreloadData*      preloadData;

    void animate( const Instance* instance );
    void draw( const Instance* instance, int mask );

  public:

    static void drawScheduled( int mask );
    static void clearScheduled();

    static void deallocate();

    explicit Mesh();
    ~Mesh();

    void schedule( int component )
    {
      instances.add( Instance( tf.model, tf.colour.w.w, component ) );
    }

    void scheduleFrame( int component, int frame )
    {
      instances.add( Instance( tf.model, tf.colour.w.w, component, frame, 0, 0.0f ) );
    }

    void scheduleAnimated( int component, int firstFrame, int secondFrame, float interpolation )
    {
      instances.add( Instance( tf.model, tf.colour.w.w, component,
                               firstFrame, secondFrame, interpolation ) );
    }

    const File* preload( const char* path );
    void upload( const Vertex* vertices, int nVertices, uint usage ) const;
    void load( uint usage );
    void unload();

};

}
}
