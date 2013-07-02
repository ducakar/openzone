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

#include "ModelBuilder.hh"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace oz
{

struct Vertex
{
  Point p;
  Vec3  n;
  float s;
  float t;

  OZ_HIDDEN
  bool operator == ( const Vertex& v ) const
  {
    return p == v.p && n == v.n && s == v.s && t == v.t;
  }

  OZ_HIDDEN
  bool operator < ( const Vertex& v ) const
  {
    if( p.x < v.p.x ) { return true; } if( p.x > v.p.x ) { return false; }
    if( p.y < v.p.y ) { return true; } if( p.y > v.p.y ) { return false; }
    if( p.z < v.p.z ) { return true; } if( p.z > v.p.z ) { return false; }

    if( n.x < v.n.x ) { return true; } if( n.x > v.n.x ) { return false; }
    if( n.y < v.n.y ) { return true; } if( n.y > v.n.y ) { return false; }
    if( n.z < v.n.z ) { return true; } if( n.z > v.n.z ) { return false; }

    if( s < v.s ) { return true; } if( s > v.s ) { return false; }
    if( t < v.t ) { return true; } if( t > v.t ) { return false; }

    return false;
  }

  OZ_HIDDEN
  void write( OutputStream* ostream ) const
  {
    ostream->writePoint( p );
    ostream->writeVec3( n );
    ostream->writeFloat( s );
    ostream->writeFloat( t );
  }
};

struct Material
{
  String texture;
  float  alpha;
};

struct Mesh
{
  int firstIndex;
  int nIndices;
  int material;
};

static const int ERROR_LENGTH = 1024;

static char errorBuffer[ERROR_LENGTH] = {};

static Assimp::Importer importer;

bool ModelBuilder::isModel( const File& file )
{
  return importer.IsExtensionSupported( file.extension() );
}

const char* ModelBuilder::getError()
{
  return errorBuffer[0] == '\0' ? importer.GetErrorString() : errorBuffer;
}

bool ModelBuilder::buildModel( const File& file, OutputStream* ostream )
{
  errorBuffer[0] = '\0';

  Buffer      buffer;
  InputStream istream;

  if( file.isMapped() ) {
    istream = file.inputStream();
  }
  else {
    buffer  = file.read();
    istream = buffer.inputStream();
  }

  if( !istream.isAvailable() ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to read '%s'", file.path().cstr() );
    return false;
  }

  const aiScene* scene = importer.ReadFileFromMemory( istream.begin(), size_t( istream.capacity() ),
                                                      aiProcess_JoinIdenticalVertices |
                                                      aiProcess_Triangulate |
                                                      aiProcess_GenNormals |
                                                      aiProcess_ImproveCacheLocality |
                                                      aiProcess_SortByPType );
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

  Set<Vertex>    vertices;
  List<ushort>   indices;
  List<Material> materials;
  List<Mesh>     meshes;

  for( uint i = 0; i < scene->mNumMeshes; ++i ) {
    const aiMesh* mesh = scene->mMeshes[i];

    for( uint j = 0; j < mesh->mNumVertices; ++j ) {
      const aiVector3D* positions = mesh->mVertices;
      const aiVector3D* normals   = mesh->mNormals;
      const aiVector3D* texCoords = mesh->mTextureCoords[0];

      vertices.add( {
        Point( positions[j].x, positions[j].y, positions[j].z ),
        Vec3( normals[j].x, normals[j].y, normals[j].z ),
        texCoords == nullptr ? 0.0f : texCoords[j].x,
        texCoords == nullptr ? 0.0f : texCoords[j].y
      } );
    }

    int firstIndex = indices.length();

    for( uint j = 0; j < mesh->mNumFaces; ++j ) {
      const aiFace& face = mesh->mFaces[j];

      hard_assert( face.mNumIndices == 3 );

      indices.add( ushort( face.mIndices[0] ) );
      indices.add( ushort( face.mIndices[1] ) );
      indices.add( ushort( face.mIndices[2] ) );
    }

    int nIndices = indices.length() - firstIndex;
    int material = int( mesh->mMaterialIndex );

    meshes.add( { firstIndex, nIndices, material } );
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

    Log() << i << ": " << String::fileBaseName( path.C_Str() ) << "\n";
  }

  ostream->writeInt( vertices.length() );
  ostream->writeInt( indices.length() );
  ostream->writeInt( materials.length() );
  ostream->writeInt( meshes.length() );

  for( int i = 0; i < vertices.length(); ++i ) {
    vertices[i].write( ostream );
  }
  for( int i = 0; i < indices.length(); ++i ) {
    ostream->writeUShort( indices[i] );
  }
  for( int i = 0; i < materials.length(); ++i ) {
    ostream->writeString( materials[i].texture );
    ostream->writeFloat( materials[i].alpha );
  }
  for( int i = 0; i < meshes.length(); ++i ) {
    ostream->writeInt( meshes[i].firstIndex );
    ostream->writeInt( meshes[i].nIndices );
  }

  Log() << "Meshes: " << meshes.length() << "\n";
  Log() << "Vertices: " << vertices.length() << "\n";
  Log() << "Indices: " << indices.length() << "\n";

  return true;
}

}
