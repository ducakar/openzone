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
#include "Context.h"

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

  MD3::Part::Part( MD3 *parent, const String &dir, const char *fileName, MD3Tag **tags )
  {
    const String &path = dir + fileName;
    FILE *file;

    file = fopen( path, "rb" );
    if( file == null ) {
      log.println( "MD3 model part file '%s' not found", path.cstr() );
      throw Exception( "MD3 model part file not found" );
    }

    MD3Header header;
    fread( &header, sizeof( MD3Header ), 1, file );
    if( header.id != FOURCC( 'I', 'D', 'P', '3' ) || header.version != 15 ) {
      fclose( file );
      log.println( "MD3 model part file '%s' invalid format", path.cstr() );
      throw Exception( "MD3 model part file invalid format" );
    }

    nFrames = header.nFrames;

    MD3Frame *frames = new MD3Frame[header.nFrames];
    fread( frames, sizeof( MD3Frame ), header.nFrames, file );
    delete[] frames;

    *tags = new MD3Tag[header.nFrames * header.nTags];
    fread( *tags, sizeof( MD3Tag ), header.nFrames * header.nTags, file );

    meshes( header.nSurfaces );
    for( int i = 0; i < header.nSurfaces; i++ ) {
      Mesh *mesh = &meshes[i];

      MD3Surface surface;
      fread( &surface, sizeof( MD3Surface ), 1, file );

      mesh->nVertices = surface.nVertices;

      mesh->triangles( surface.nTriangles );
      fread( mesh->triangles, sizeof( Triangle ), surface.nTriangles, file );

      MD3Shader *shaders = new MD3Shader[surface.nShaders];
      fread( shaders, sizeof( MD3Shader ), surface.nShaders, file );

      const char *shaderBaseName;

      shaderBaseName = String::lastIndex( shaders[0].name, '/' );
      if( shaderBaseName == null ) {
        shaderBaseName = String::lastIndex( shaders[0].name, '\\' );
      }
      if( shaderBaseName == null ) {
        log.println( "MD3 model file '%s' invalid format", path.cstr() );
        throw Exception( "MD3 model part file invalid format" );
      }
      shaderBaseName++;
      mesh->texId = context.loadTexture( dir + shaderBaseName );
      parent->textures.include( mesh->texId );
      delete[] shaders;

      mesh->texCoords( surface.nVertices );
      fread( mesh->texCoords, sizeof( TexCoord ), surface.nVertices, file );

      // convert (S,T) -> (U,V) i.e. top-left origin coords to lower-left origin coords
      for( int j = 0; j < mesh->texCoords.length(); j++ ) {
//         mesh->texCoords[j].v = 1.0f - mesh->texCoords[j].v;
      }

      mesh->vertices( surface.nFrames * surface.nVertices );
      MD3Vertex *vertices = new MD3Vertex[mesh->vertices.length()];
      fread( vertices, sizeof( MD3Vertex ), mesh->vertices.length(), file );

      for( int j = 0; j < mesh->vertices.length(); j++ ) {
        mesh->vertices[j].p.x = -vertices[j].p[1] / 64.0f;
        mesh->vertices[j].p.y =  vertices[j].p[0] / 64.0f;
        mesh->vertices[j].p.z =  vertices[j].p[2] / 64.0f;

        // convert from zenith/azimuth coords
        float azimuth = vertices[j].normal[0] * Math::_2_PI / 255.0f;
        float zenith  = vertices[j].normal[1] * Math::_2_PI / 255.0f;
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
    if( !meshes.isEmpty() ) {
      for( int i = 0; i < meshes.length(); i++ ) {
        meshes[i].triangles.clear();
        meshes[i].texCoords.clear();
        meshes[i].vertices.clear();
      }
      meshes.clear();
    }
  }

  void MD3::Part::scale( float scale )
  {
    foreach( mesh, meshes.iterator() ) {
      foreach( v, mesh->vertices.iterator() ) {
        v->p *= scale;
      }
    }
  }

  void MD3::Part::translate( const Vec3 &t )
  {
    foreach( mesh, meshes.iterator() ) {
      foreach( v, mesh->vertices.iterator() ) {
        v->p += t;
      }
    }
  }

  void MD3::Part::drawFrame( int frame ) const
  {
    assert( 0 <= frame && frame < nFrames );

    for( int i = 0; i < meshes.length(); i++ ) {
      const Mesh &mesh = meshes[i];

      glBindTexture( GL_TEXTURE_2D, mesh.texId );

      glBegin( GL_TRIANGLES );
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

        glTexCoord2fv( &texCoord2.u );
        glNormal3fv( &vertex2.normal.x );
        glVertex3fv( &vertex2.p.x );

        glTexCoord2fv( &texCoord1.u );
        glNormal3fv( &vertex1.normal.x );
        glVertex3fv( &vertex1.p.x );
      }
      glEnd();
    }
  }

  void MD3::scale( float scale )
  {
    head->scale( scale );
    upper->scale( scale );
    lower->scale( scale );

    foreach( offset, headOffsets.iterator() ) {
      offset->p *= scale;
    }
    foreach( offset, lowerOffsets.iterator() ) {
      offset->p *= scale;
    }
    foreach( offset, weaponOffsets.iterator() ) {
      offset->p *= scale;
    }
  }

  void MD3::translate( const Vec3 &t )
  {
    head->translate( t );
    upper->translate( t );
    lower->translate( t );
  }

  MD3::MD3( const char *name_ )
  {
    String name       = name_;
    String dir        = "mdl/" + name + "/";
    String configFile = dir + "config.xml";

    MD3Tag *headTags;
    MD3Tag *upperTags;
    MD3Tag *lowerTags;

    head  = new Part( this, dir, name + "_head.md3", &headTags );
    upper = new Part( this, dir, name + "_upper.md3", &upperTags );
    lower = new Part( this, dir, name + "_lower.md3", &lowerTags );

    headOffsets( upper->nFrames );
    lowerOffsets( upper->nFrames );
    weaponOffsets( upper->nFrames );

    // assemble the model
    for( int i = 0; i < upper->nFrames; i++ ) {
      headOffsets[i].p.x   = -upperTags[3 * i + 1].p.y;
      headOffsets[i].p.y   =  upperTags[3 * i + 1].p.x;
      headOffsets[i].p.z   = -upperTags[3 * i + 1].p.z;

      lowerOffsets[i].p.x  = -lowerTags[i].p.y;
      lowerOffsets[i].p.y  =  lowerTags[i].p.x;
      lowerOffsets[i].p.z  = -lowerTags[i].p.z;

      weaponOffsets[i].p.x = -upperTags[3 * i].p.y;
      weaponOffsets[i].p.y =  upperTags[3 * i].p.x;
      weaponOffsets[i].p.z = -upperTags[3 * i].p.z;
    }

    delete[] headTags;
    delete[] upperTags;
    delete[] lowerTags;

    Config config;
    config.load( configFile );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation( config.get( "translate.x", 0.0f ),
                      config.get( "translate.y", 0.0f ),
                      config.get( "translate.z", 0.0f ) );
    Vec3 crouchTranslation( config.get( "crouchTranslate.x", 0.0f ),
                            config.get( "crouchTranslate.y", 0.0f ),
                            config.get( "crouchTranslate.z", 0.0f ) );
    config.clear();

    if( scaling != 1.0f ) {
      scale( scaling );
    }
    translate( translation );

    if( !crouchTranslation.isZero() ) {
//       translate( ANIM_CROUCH_STAND,  crouchTranslation );
//       translate( ANIM_CROUCH_WALK,   crouchTranslation );
//       translate( ANIM_CROUCH_ATTACK, crouchTranslation );
//       translate( ANIM_CROUCH_PAIN,   crouchTranslation );
//       translate( ANIM_CROUCH_DEATH,  crouchTranslation );
    }
  }

  MD3::~MD3()
  {
    assert( glGetError() == GL_NO_ERROR );
  }

  void MD3::drawFrame( int frame ) const
  {
    glPushMatrix();

    upper->drawFrame( frame );

//     glTranslatef( headOffsets[frame].p.x, headOffsets[frame].p.y, headOffsets[frame].p.z );
//     head->drawFrame( 0 );
//
//     glPopMatrix();
//     glPushMatrix();

    glTranslatef( lowerOffsets[frame].p.x, lowerOffsets[frame].p.y, lowerOffsets[frame].p.z );
    lower->drawFrame( frame );

    glPopMatrix();
  }

  void MD3::genList()
  {
    list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
      drawFrame( 0 );
    glEndList();
  }

  void MD3::trim()
  {}

}
}
