/*
 *  MD3.cpp
 *
 *  http://www.wikipedia.org/MD3_(file_format)
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "MD3.h"

#include "matrix/bv.h"

#define FOURCC( a, b, c, d ) \
  ( ( a ) | ( ( b ) << 8 ) | ( ( c ) << 16 ) | ( ( d ) << 24 ) )

namespace oz
{
namespace client
{

  struct MD3Header
  {
    int  id;
    int  version;
    char fileName[64];
    int  flags;

    int  nFrames;
    int  nTags;
    int  nSurfaces;
    int  nSkins;

    int  offFrames;
    int  offTags;
    int  offSurfaces;
    int  offEOF;
  };

  struct MD3Tag
  {
    char  name[64];
    Vec3  p;
    Mat33 rot;
  };

  struct MD3Surface
  {
    int  id;
    char name[64];
    int  flags;

    int  nFrames;
    int  nShaders;
    int  nVertices;
    int  nTriangles;

    int  offTriangles;
    int  offShaders;
    int  offTexCoords;
    int  offVertices;
    int  offEnd;
  };

  struct MD3Frame : Bounds
  {
    Vec3  p;
    float radius;
    char  name[16];
  };

  struct MD3Shader
  {
    char name[64];
    int  index;
  };

  struct MD3Vertex
  {
    short p[3];
    ubyte normal[2];
  };

  MD3::Part::Part( const char *path )
  {
    FILE *file;

    file = fopen( path, "rb" );
    if( file == null ) {
      throw Exception( 0, "MD3 file not found" );
    }

    MD3Header header;
    fread( &header, sizeof( MD3Header ), 1, file );
    if( header.id != FOURCC( 'I', 'D', 'P', '3' ) || header.version != 15 ) {
      fclose( file );
      throw Exception( 0, "MD3 invalid format" );
    }

    MD3Frame *frames = new MD3Frame[header.nFrames];
    fread( frames, sizeof( MD3Frame ), header.nFrames, file );
    delete[] frames;

    MD3Tag *tags = new MD3Tag[header.nFrames * header.nTags];
    fread( tags, sizeof( MD3Tag ), header.nFrames * header.nTags, file );
    delete[] tags;

    links( header.nTags );
    aSet<Part*>( links, null, header.nTags );

    fseek( file, header.offSurfaces, SEEK_SET );

    meshes( header.nSurfaces );

    for( int i = 0; i < header.nSurfaces; i++ ) {
      Mesh *mesh = &meshes[i];

      MD3Surface surface;
      fread( &surface, sizeof( MD3Surface ), 1, file );

      mesh->nVertices = surface.nVertices;

      MD3Shader *shaders = new MD3Shader[surface.nShaders];
      fread( shaders, sizeof( MD3Shader ), surface.nShaders, file );
      delete[] shaders;

      mesh->triangles( surface.nTriangles );
      fread( mesh->triangles, sizeof( Triangle ), surface.nTriangles, file );

      mesh->texCoords( surface.nVertices );
      fread( mesh->texCoords, sizeof( TexCoord ), surface.nVertices, file );

      // convert (S,T) -> (U,V) i.e. top-left origin coords to lower-left origin coords
      for( int j = 0; j < mesh->texCoords.length(); j++ ) {
        mesh->texCoords[j].v = 1.0f - mesh->texCoords[j].v;
      }

      mesh->vertices( surface.nFrames * surface.nVertices );
      MD3Vertex *vertices = new MD3Vertex[mesh->vertices.length()];
      fread( vertices, sizeof( MD3Vertex ), mesh->vertices.length(), file );

      for( int j = 0; j < mesh->vertices.length(); j++ ) {
        mesh->vertices[j].p.x = vertices[i].p[0] / 64.0f;
        mesh->vertices[j].p.y = vertices[i].p[1] / 64.0f;
        mesh->vertices[j].p.z = vertices[i].p[2] / 64.0f;

        // convert from zenith/azimuth coords
        float azimuth = vertices[i].normal[0] * Math::_2_PI / 255.0f;
        float zenith  = vertices[i].normal[1] * Math::_2_PI / 255.0f;
        float xy      = Math::sin( zenith );

        mesh->vertices[j].normal.x = xy * Math::cos( azimuth );
        mesh->vertices[j].normal.y = xy * Math::sin( azimuth );
        mesh->vertices[j].normal.z = Math::cos( zenith );
      }
      delete[] vertices;
    }
  }

  MD3::Part::~Part()
  {
    if( !links.isEmpty() ) {
      links.clear();
    }
    if( !meshes.isEmpty() ) {
      for( int i = 0; i < meshes.length(); i++ ) {
        meshes[i].triangles.clear();
        meshes[i].texCoords.clear();
        meshes[i].vertices.clear();
      }
      meshes.clear();
    }
  }

  void MD3::Part::drawFrame( int frame ) const
  {
    foreach( i, meshes.iterator() ) {
    }

    for( int i = 0; i < meshes.length(); i++ ) {
      const Mesh &mesh = meshes[i];

      glBindTexture( GL_TEXTURE_2D, mesh.texId );

      glBegin( GL_QUADS );
      for( int j = 0; j < mesh.triangles.length(); j++ ) {
        const Triangle &triangle  = mesh.triangles[j];
        const TexCoord &texCoord0 = mesh.texCoords[triangle.indices[0]];
        const TexCoord &texCoord1 = mesh.texCoords[triangle.indices[1]];
        const TexCoord &texCoord2 = mesh.texCoords[triangle.indices[2]];
        const Vertex   &vertex0   = mesh.vertices[frame * mesh.nVertices + triangle.indices[0]];
        const Vertex   &vertex1   = mesh.vertices[frame * mesh.nVertices + triangle.indices[1]];
        const Vertex   &vertex2   = mesh.vertices[frame * mesh.nVertices + triangle.indices[2]];

        glTexCoord2fv( &texCoord0.u );
        glNormal3fv( &vertex0.normal.x );
        glVertex3fv( &vertex0.p.x );

        glTexCoord2fv( &texCoord1.u );
        glNormal3fv( &vertex1.normal.x );
        glVertex3fv( &vertex1.p.x );

        glTexCoord2fv( &texCoord2.u );
        glNormal3fv( &vertex2.normal.x );
        glVertex3fv( &vertex2.p.x );
      }
      glEnd();
    }
  }

  MD3::MD3( const char *name_ )
  {
    String name = name_;

    head  = new Part( "mdl/" + name + "/" + name + "_head.md3" );
    torso = new Part( "mdl/" + name + "/" + name + "_upper.md3" );
    legs  = new Part( "mdl/" + name + "/" + name + "_lower.md3" );
  }

  MD3::~MD3()
  {}

  uint MD3::genList( const char *name )
  {
    MD3 md3( name );

    uint list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
      md3.drawFrame( 0 );
    glEndList();

    return list;
  }

  void MD3::drawFrame( int frame ) const
  {
    head->drawFrame( frame );
    torso->drawFrame( frame );
    legs->drawFrame( frame );
  }

}
}
