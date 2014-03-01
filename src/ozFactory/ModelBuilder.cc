/*
 * ozFactory - OpenZone Assets Builder Library.
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
 * @file ozFactory/ModelBuilder.cc
 */

#if 0

#include "ModelBuilder.hh"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <ozEngine/GL.hh>

namespace oz
{

static const int ERROR_LENGTH = 1024;
static const Vec3 DIRS[] = {
  Vec3( +1.0f, +1.0f, +1.0f ),
  Vec3( -1.0f, +1.0f, +1.0f ),
  Vec3( +1.0f, -1.0f, +1.0f ),
  Vec3( -1.0f, -1.0f, +1.0f ),
  Vec3( +1.0f, +1.0f, -1.0f ),
  Vec3( -1.0f, +1.0f, -1.0f ),
  Vec3( +1.0f, -1.0f, -1.0f ),
  Vec3( -1.0f, -1.0f, -1.0f )
};

struct Vertex
{
  Point p;
  Vec3  n;
  float u;
  float v;
};

struct AnimVertex
{
  Point p;
  Vec3  n;
};

struct Triangle
{
  float depth;
  int   index;

  bool operator < ( const Triangle& t )
  {
    return depth < t.depth;
  }
};

struct Mesh
{
  int firstIndex;
  int nIndices;

  int firstAnimVertex;
  int nAnimVertices;

  int material;
};

struct Material
{
  String texture;
  float  alpha;
};

struct Anim
{
  enum Behaviour
  {
    DEFAULT,
    NEAREST,
    LINEAR
  };

  struct Key
  {
    float time;
    Vec3  scale;
    Quat  rot;
    Vec3  transl;
  };

  int       firstKey;
  int       nKeys;
  Behaviour behaviour;

  Mat4 interpolate( float time ) const;
};

struct Light
{
  enum Type
  {
    DIRECTIONAL,
    POINT,
    SPOT
  };

  Point p;
  Vec3  dir;
  float coneCoeff[2];
  float attenuation[3];
  Vec3  colour;
  Type  type;
};

struct Node
{
  String      name;
  Mat4        transf;
  DArray<int> meshes;
  DArray<int> children;
};

static List<Vertex>     vertices;
static List<AnimVertex> animVertices;
static List<ushort>     indices;
static List<Triangle>   triangles[8];
static List<Mesh>       meshes;
static List<Material>   materials;
static List<Anim::Key>  animKeys;
static List<Anim>       anims;
static List<Light>      lights;
static List<Node>       nodes;

static int readNode( const aiNode* origNode )
{
  nodes.add();

  int   nodeId = nodes.length() - 1;
  Node& node   = nodes.last();

  node.name = origNode->mName.C_Str();
  node.transf = ~Mat4( origNode->mTransformation[0] );
  node.meshes.resize( int( origNode->mNumMeshes ) );
  node.children.resize( int( origNode->mNumChildren ) );

  Log() << origNode->mName.C_Str();
  Log() << node.transf;
  Log() << node.children.length() << " :: " << node.meshes.length();

  for( int i = 0; i < int( origNode->mNumMeshes ); ++i ) {
    node.meshes[i] = int( origNode->mMeshes[i] );
  }
  for( int i = 0; i < int( origNode->mNumChildren ); ++i ) {
    node.children[i] = readNode( origNode->mChildren[i] );
  }
  return nodeId;
}

static int findNode( const char* name )
{
  for( int i = 0; i < nodes.length(); ++i ) {
    if( nodes[i].name.equals( name ) ) {
      return i;
    }
  }
  return -1;
}

static char             errorBuffer[ERROR_LENGTH] = {};
static Assimp::Importer importer;

const char* ModelBuilder::getError()
{
  return errorBuffer[0] == '\0' ? importer.GetErrorString() : errorBuffer;
}

bool ModelBuilder::isModel( const File& file )
{
  errorBuffer[0] = '\0';

  return importer.IsExtensionSupported( file.extension() );
}

bool ModelBuilder::buildModel( const File& file, OutputStream* os )
{
  errorBuffer[0] = '\0';

  InputStream is = file.inputStream();

  if( !is.isAvailable() ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to read '%s'", file.path().cstr() );
    return false;
  }

  const aiScene* scene = importer.ReadFile( file.realPath(),
                                            aiProcess_JoinIdenticalVertices |
                                            aiProcess_Triangulate |
                                            aiProcess_GenNormals |
                                            aiProcess_ValidateDataStructure |
                                            aiProcess_ImproveCacheLocality |
                                            aiProcess_RemoveRedundantMaterials |
                                            aiProcess_FindInvalidData |
                                            aiProcess_FindInstances |
                                            aiProcess_OptimizeMeshes );
  if( scene == nullptr ) {
    return false;
  }

  if( !scene->HasMeshes() ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Meshes missing" );
    return false;
  }
  if( !scene->HasMaterials() ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Materials missing" );
    return false;
  }

  for( uint i = 0; i < scene->mNumMeshes; ++i ) {
    const aiMesh*     mesh      = scene->mMeshes[i];
    const aiVector3D* positions = mesh->mVertices;
    const aiVector3D* normals   = mesh->mNormals;
    const aiVector3D* texCoords = mesh->mTextureCoords[0];

    int firstVertex = vertices.length();
    int firstIndex  = indices.length();

    for( uint j = 0; j < mesh->mNumVertices; ++j ) {
      vertices.add( {
        Point( positions[j].x, positions[j].y, positions[j].z ),
        Vec3( normals[j].x, normals[j].y, normals[j].z ),
        texCoords == nullptr ? 0.0f : texCoords[j].x,
        texCoords == nullptr ? 0.0f : texCoords[j].y
      } );
    }

    for( uint j = 0; j < mesh->mNumFaces; ++j ) {
      const aiFace& face = mesh->mFaces[j];

      hard_assert( face.mNumIndices == 3 );

      Vec3 a      = vertices[ firstVertex + int( face.mIndices[0] ) ].p - Point::ORIGIN;
      Vec3 b      = vertices[ firstVertex + int( face.mIndices[1] ) ].p - Point::ORIGIN;
      Vec3 c      = vertices[ firstVertex + int( face.mIndices[2] ) ].p - Point::ORIGIN;
      Vec3 centre = ( a + b + c ) / 3.0f;

      for( int k = 0; k < 8; ++k ) {
        triangles[k].add( { centre * DIRS[k], int( j ) } );
      }
    }

    int nIndices = indices.length() - firstIndex;
    int material = int( mesh->mMaterialIndex );

    meshes.add( { firstIndex, nIndices, 0, 0, material } );
  }

  for( uint i = 0; i < scene->mNumMaterials; ++i ) {
    const aiMaterial* material = scene->mMaterials[i];

    aiString path;
    if( material->GetTextureCount( aiTextureType_DIFFUSE ) != 0 ) {
      material->GetTexture( aiTextureType_DIFFUSE, 0, &path );
    }

    float alpha = 1.0f;
    material->Get<float>( AI_MATKEY_OPACITY, alpha );

    materials.add( { String::fileBaseName( path.C_Str() ), alpha } );

    Log() << i << " texure: " << path.C_Str();
  }

  for( uint i = 0; i < scene->mNumAnimations; ++i ) {
    const aiAnimation* anim = scene->mAnimations[i];

    for( uint j = 0; j < anim->mNumChannels; ++j ) {
      const aiNodeAnim* nodeAnim = anim->mChannels[j];

      for( double t = 0.0; t < anim->mDuration; ) {


      }
    }
  }

  for( uint i = 0; i < scene->mNumLights; ++i ) {
    const aiLight* light = scene->mLights[i];

    lights.add( {
      Point( light->mPosition.x, light->mPosition.y, light->mPosition.z ),
      Vec3( light->mDirection.x, light->mDirection.y, light->mDirection.z ),
      { Math::tan( light->mAngleInnerCone / 2.0f ), Math::tan( light->mAngleOuterCone / 2.0f ) },
      { light->mAttenuationConstant, light->mAttenuationLinear, light->mAttenuationQuadratic },
      Vec3( light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b ),
      Light::Type( light->mType - 1 )
    } );
  }

  readNode( scene->mRootNode );

  os->writeInt( vertices.length() );
  os->writeInt( indices.length() );
  os->writeInt( meshes.length() );
  os->writeInt( materials.length() );
  os->writeInt( animKeys.length() );
  os->writeInt( anims.length() );
  os->writeInt( lights.length() );
  os->writeInt( nodes.length() );

  Log() << vertices.length() << " vertices";
  Log() << indices.length() << " indices";
  Log() << meshes.length() << " meshes";
  Log() << materials.length() << " materials";
  Log() << animKeys.length() << " animKeys";
  Log() << anims.length() << " anims";
  Log() << lights.length() << " lights";
  Log() << nodes.length() << " nodes";

  for( int i = 0; i < vertices.length(); ++i ) {
    os->writePoint( vertices[i].p );
    os->writeVec3( vertices[i].n );
    os->writeFloat( vertices[i].u );
    os->writeFloat( vertices[i].v );
  }
  for( int i = 0; i < indices.length(); ++i ) {
    os->writeUShort( indices[i] );
  }

  for( int i = 0; i < meshes.length(); ++i ) {
    os->writeInt( meshes[i].nIndices );
    os->writeInt( meshes[i].firstIndex );
  }

  for( int i = 0; i < materials.length(); ++i ) {
    os->writeString( materials[i].texture );
    os->writeFloat( materials[i].alpha );
  }

  for( int i = 0; i < lights.length(); ++i ) {
    os->writePoint( lights[i].p );
    os->writeVec3( lights[i].dir );
    os->writeFloat( lights[i].coneCoeff[0] ),
    os->writeFloat( lights[i].coneCoeff[1] ),
    os->writeFloat( lights[i].attenuation[0] );
    os->writeFloat( lights[i].attenuation[1] );
    os->writeFloat( lights[i].attenuation[2] );
    os->writeVec3( lights[i].colour );
    os->writeInt( lights[i].type );
  }

  vertices.clear();
  vertices.deallocate();

  indices.clear();
  indices.deallocate();

  for( int i = 0; i < 8; ++i ) {
    triangles[i].clear();
    triangles[i].deallocate();
  }

  meshes.clear();
  meshes.deallocate();

  materials.clear();
  materials.deallocate();

  animKeys.clear();
  animKeys.deallocate();

  anims.clear();
  anims.deallocate();

  lights.clear();
  lights.deallocate();

  nodes.clear();
  nodes.deallocate();

  return true;
}

}

#endif
