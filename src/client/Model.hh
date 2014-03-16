/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/Model.hh
 */

#pragma once

#include <client/Shader.hh>
#include <client/MD2.hh>

namespace oz
{
namespace client
{

struct Vertex
{
  float pos[3];
  float texCoord[2];
  byte  normal[4];
  byte  tangent[4];
  byte  binormal[4];

  static void setFormat();
};

struct Texture
{
  int  id;      ///< -2: external, -1: default, 0+ BSP texture index.
  uint albedo;
  uint masks;
  uint normals;

  explicit Texture() :
    id( -1 ), albedo( shader.defaultTexture ), masks( shader.defaultMasks ),
    normals( shader.defaultNormals )
  {}
};

struct Light
{
  enum Type
  {
    DIRECTIONAL,
    POINT,
    SPOT
  };

  int   node;
  Type  type;

  Point pos;
  Vec3  dir;
  Vec3  colour;
  float attenuation[3];
  float coneCoeff[2];
};

class Model
{
public:

  static const int EMBEDED_TEX_BIT = 0x01; ///< Textures are embedded into model file.

  static const int SOLID_BIT       = 0x04; ///< Mesh is opaque.
  static const int ALPHA_BIT       = 0x08; ///< Mesh is transparent.

  static const int DIFFUSE_BIT     = 0x10; ///< Texture has base per-pixel colours.
  static const int MASKS_BIT       = 0x20; ///< Texture has specular and emission masks.
  static const int NORMALS_BIT     = 0x40; ///< Texture has normal map.

  enum Collation
  {
    DEPTH_MAJOR,
    MODEL_MAJOR
  };

  enum QueueType
  {
    SCENE_QUEUE,
    OVERLAY_QUEUE
  };

private:

  struct Mesh
  {
    int   flags;
    int   texture;
    float shininess;

    int nIndices;
    int firstIndex;
  };

  struct Node
  {
    Mat4   transf;
    int    parent;
    int    firstChild;
    int    nChildren;
    int    mesh;
    String name;
  };

  struct Instance
  {
    Model* model;
    Mat4   transf;
    Mat4   colour;

    int    node;
    int    firstFrame;
    int    secondFrame;
    float  interpolation;
  };

  struct LightEntry;
  struct PreloadData;

private:

  static Set<Model*>      loadedModels;
  static List<Instance>   instances[2];
  static List<LightEntry> sceneLights;

  static Vertex*          vertexAnimBuffer;
  static int              vertexAnimBufferLength;
  static Collation        collation;

  String                  path;
  int                     flags;
  uint                    vbo;
  uint                    ibo;
  int                     shaderId;
  uint                    animationTexId;

  DArray<Texture>         textures;
  DArray<Mesh>            meshes;
  DArray<Light>           lights;
  DArray<Node>            nodes;

  int                     nTextures;
  int                     nVertices;
  int                     nIndices;
  int                     nFrames;
  int                     nFramePositions;

  Vertex*                 vertices;
  Point*                  positions;
  Vec3*                   normals;

  List<Instance>          modelInstances[2];
  PreloadData*            preloadData;

public:

  Vec3                    dim;

private:

  void addSceneLights();

  void animate( const Instance* instance );
  void drawNode( const Node* node, int mask );
  void draw( const Instance* instance, int mask );

public:

  static void setCollation( Collation collation );

  static void drawScheduled( QueueType queue, int mask );
  static void clearScheduled( QueueType queue );

  static void deallocate();

  explicit Model( const String& path );
  ~Model();

  bool isPreloaded() const
  {
    return preloadData != nullptr;
  }

  bool isLoaded() const
  {
    return !meshes.isEmpty();
  }

  int findNode( const char* name ) const;

  void schedule( int mesh, QueueType queue );
  void scheduleFrame( int mesh, int frame, QueueType queue );
  void scheduleAnimated( int mesh, int firstFrame, int secondFrame, float interpolation,
                         QueueType queue );

  void scheduleMD2Anim( const MD2::AnimState* anim, Model::QueueType queue )
  {
    scheduleAnimated( 1, anim->currFrame, anim->nextFrame, anim->frameRatio, queue );
  }

  const File* preload();
  void upload( const Vertex* vertices, int nVertices, uint usage ) const;
  void load();
  void unload();

};

}
}
