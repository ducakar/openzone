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
 * @file builder/AssImp.cc
 */

#include <builder/AssImp.hh>

#include <client/Context.hh>
#include <builder/Compiler.hh>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace oz
{
namespace builder
{

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

  Mat44 interpolate( float time ) const;
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
  Mat44       transf;
  DArray<int> meshes;
  DArray<int> children;
};

static List<Material>   materials;
static List<Anim::Key>  animKeys;
static List<Anim>       anims;
static List<Light>      lights;
static List<Node>       nodes;
static Assimp::Importer importer;

static int readNode( const aiNode* origNode )
{
  nodes.add();

  int   nodeId = nodes.length() - 1;
  Node& node   = nodes.last();

  node.name = origNode->mName.C_Str();
  node.transf = ~Mat44( origNode->mTransformation[0] );
  node.meshes.resize( int( origNode->mNumMeshes ) );
  node.children.resize( int( origNode->mNumChildren ) );

  Log() << origNode->mName.C_Str();
  Log() << node.transf;
  Log() << node.children.length() << " :: " << node.meshes.length();

  for( int i = 0; i < int( origNode->mNumMeshes ); ++i ) {
    node.meshes[i] = int( origNode->mMeshes[i] );
  }
  for( int i = 0; i < int( origNode->mNumChildren ); ++i ) {
    int childId = readNode( origNode->mChildren[i] );

    // Nodes vector may reallocate storage, re-reference node.
    Node& node = nodes[nodeId];
    node.children[i] = childId;
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

void AssImp::build( const char* path )
{
  Log::println( "Prebuilding Collada model '%s' {", path );
  Log::indent();

  File   modelFile = String( path, "/data.dae" );
  File   outFile   = String( &path[1], "/data.ozcSMM" );
  String basePath  = String( path, "/" );

  if( modelFile.type() == File::MISSING ) {
    modelFile = String( path, "/data.obj" );
  }

  InputStream istream = modelFile.inputStream();

  if( !istream.isAvailable() ) {
    OZ_ERROR( "Failed to read '%s' (.dae and .obj extensions probed)", path );
  }

  const aiScene* scene = importer.ReadFile( modelFile.realPath(),
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
    OZ_ERROR( "Error loading '%s': Failed to load scene", modelFile.path().cstr() );
  }

  if( !scene->HasMeshes() ) {
    OZ_ERROR( "Error loading '%s': Meshes missing", modelFile.path().cstr() );
  }
  if( !scene->HasMaterials() ) {
    OZ_ERROR( "Error loading '%s': Materials missing", modelFile.path().cstr() );
  }

  compiler.beginModel();

  for( uint i = 0; i < scene->mNumMeshes; ++i ) {
    const aiMesh*     mesh      = scene->mMeshes[i];
    const aiMaterial* material  = scene->mMaterials[mesh->mMaterialIndex];
    const aiVector3D* positions = mesh->mVertices;
    const aiVector3D* normals   = mesh->mNormals;
    const aiVector3D* texCoords = mesh->mTextureCoords[0];

    aiString texture;
    if( material->GetTextureCount( aiTextureType_DIFFUSE ) != 0 ) {
      material->GetTexture( aiTextureType_DIFFUSE, 0, &texture );
    }

    float alpha = 1.0f;
    material->Get<float>( AI_MATKEY_OPACITY, alpha );

    compiler.texture( basePath + String::fileBaseName( texture.C_Str() ) );
    compiler.blend( alpha != 1.0f );

    compiler.begin( Compiler::TRIANGLES );

    for( uint j = 0; j < mesh->mNumFaces; ++j ) {
      const aiFace& face = mesh->mFaces[j];

      hard_assert( face.mNumIndices == 3 );

      for( uint k = 0; k < 3; ++k ) {
        uint index = face.mIndices[k];

        if( texCoords != nullptr ) {
          compiler.texCoord( texCoords[index].x, 1.0f - texCoords[index].y );
        }
        compiler.normal( normals[index].x, normals[index].y, normals[index].z );
        compiler.vertex( positions[index].x, positions[index].y, positions[index].z );
      }
    }

    compiler.end();
  }

  compiler.endModel();

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

  OutputStream os( 0, Endian::LITTLE );

  compiler.writeModel( &os );
  compiler.buildModelTextures( outFile.directory() );

  Log::print( "Writing to '%s' ...", outFile.path().cstr() );

  if( !outFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write %s", outFile.path().cstr() );
  }

  Log::printEnd( " OK" );

  Log::unindent();
  Log::println( "}" );
}

AssImp assImp;

}
}
