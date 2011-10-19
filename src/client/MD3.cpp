/*
 *  MD3.cpp
 *
 *  http://www.wikipedia.org/MD3_(file_format)
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/MD3.hpp"

#include "client/Context.hpp"
#include "client/Compiler.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

#ifndef OZ_TOOLS

  MD3::MD3( int id_ ) : id( id_ ), isLoaded( false )
  {}

  MD3::~MD3()
  {
    OZ_GL_CHECK_ERROR();
  }

  void MD3::load()
  {}

  void MD3::drawFrame( int frame ) const
  {
  }

#else

  String MD3::sPath;
  Config MD3::config;

  bool   MD3::forceStatic;
  float  MD3::scale;
  Mat44  MD3::meshTransf;
  Vec3   MD3::jumpTransl;
  Mat44  MD3::weaponTransf;

  void MD3::buildMesh( const char* name, int frame, DArray<MD3Tag>* tags )
  {
    log.print( "Mesh '%s' ...", name );

    String modelFile = sPath + "/" + String( name ) + ".md3";

    FILE* file = fopen( modelFile, "rb" );
    if( file == null ) {
      throw Exception( "MD3 model part file '" + modelFile + "' not found" );
    }

    MD3Header header;
    fread( &header, sizeof( MD3Header ), 1, file );

    if( header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '3' ) {
      fclose( file );
      throw Exception( "MD3 model part file invalid format" );
    }

    if( header.nFrames == 0 || header.nSurfaces == 0 ) {
      throw Exception( "Invalid MD3 header counts" );
    }

    if( forceStatic ) {
      header.nFrames = 1;
    }

    if( header.nTags != 0 ) {
      tags->alloc( header.nTags );

      fseek( file, header.offTags, SEEK_SET );
      fread( *tags, sizeof( MD3Tag ), size_t( header.nTags ), file );
    }

    fseek( file, header.offSurfaces, SEEK_SET );

    int indexBase = 0;

    for( int i = 0; i < header.nSurfaces; ++i ) {
      long surfaceStart = ftell( file );

      MD3Surface surface;
      fread( &surface, sizeof( MD3Surface ), 1, file );

      if( surface.nFrames == 0 || surface.nTriangles == 0 || surface.nShaders == 0 ||
          surface.nVertices == 0 )
      {
        throw Exception( "Invalid MD3 surface counts" );
      }

      DArray<MD3Triangle> surfaceTriangles( surface.nTriangles );
      DArray<MD3Shader>   surfaceShaders( surface.nShaders );
      DArray<MD3TexCoord> surfaceTexCoords( surface.nVertices );
      DArray<MD3Vertex>   surfaceVertices( surface.nFrames * surface.nVertices );

      fseek( file, surfaceStart + surface.offTriangles, SEEK_SET );
      fread( surfaceTriangles, sizeof( MD3Triangle ), size_t( surfaceTriangles.length() ), file );

      fseek( file, surfaceStart + surface.offShaders, SEEK_SET );
      fread( surfaceShaders, sizeof( MD3Shader ), size_t( surfaceShaders.length() ), file );

      fseek( file, surfaceStart + surface.offTexCoords, SEEK_SET );
      fread( surfaceTexCoords, sizeof( TexCoord ), size_t( surfaceTexCoords.length() ), file );

      fseek( file, surfaceStart + surface.offVertices, SEEK_SET );
      fread( surfaceVertices, sizeof( MD3Vertex ), size_t( surfaceVertices.length() ), file );

      fseek( file, surfaceStart + surface.offEnd, SEEK_SET );

      if( !forceStatic && surface.nFrames != header.nFrames ) {
        throw Exception( "Invalid MD3 surface # of frames" );
      }

      const char* skinFile = max( String::findLast( surfaceShaders[0].name, '/' ),
                                  String::findLast( surfaceShaders[0].name, '\\' ) );

      if( skinFile == null ) {
        skinFile = surfaceShaders[0].name;
      }
      else {
        ++skinFile;
      }

      compiler.texture( sPath + "/" + skinFile );

      compiler.begin( GL_TRIANGLES );

      for( int i = 0; i < surfaceTriangles.length(); ++i ) {
        for( int j = 0; j < 3; ++j ) {
          int k = surfaceTriangles[i].vertices[j];

          if( frame != -1 ) {
            int l = frame * surface.nVertices + k;

            float h  = float( surfaceVertices[l].normal[0] ) * Math::TAU / 255.0f;
            float v  = float( surfaceVertices[l].normal[1] ) * Math::TAU / 255.0f;
            float xy = Math::sin( v );

            Vec3 normal = Vec3( xy * Math::sin( -h ), xy * Math::cos( +h ), Math::cos( v ) );

            float x = float( -surfaceVertices[l].pos[1] ) / 64.0f;
            float y = float( +surfaceVertices[l].pos[0] ) / 64.0f;
            float z = float( +surfaceVertices[l].pos[2] ) / 64.0f;

            Point3 position = Point3( x * scale, y * scale, z * scale );

            compiler.texCoord( surfaceTexCoords[k].s, 1.0f - surfaceTexCoords[k].t );
            compiler.normal( meshTransf * normal );
            compiler.vertex( meshTransf * position );
          }
        }
      }

      compiler.end();

      indexBase += surface.nVertices;
    }

    log.printEnd( " OK" );
  }

  void MD3::prebuild( const char* path )
  {
    sPath = path;

    String configFile = sPath + "/config.rc";

    log.println( "Prebuilding MD3 model '%s' {", path );
    log.indent();

    Config config;
    config.load( configFile );

    forceStatic  = config.get( "forceStatic", false );
    // FIXME
    forceStatic  = true;
    String shaderName   = config.get( "shader", forceStatic ? "mesh" : "md3" );
    float  specular     = config.get( "specular", 0.0f );

    scale        = config.get( "scale", 0.04f );

    Vec3 translation  = Vec3( config.get( "translate.x", +0.00f ),
                              config.get( "translate.y", +0.00f ),
                              config.get( "translate.z", -0.04f ) );

    Vec3 weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                              config.get( "weaponTranslate.y", 0.00f ),
                              config.get( "weaponTranslate.z", 0.00f ) );

    Vec3 weaponRot    = Vec3( config.get( "weaponRotate.x", 0.00f ),
                              config.get( "weaponRotate.y", 0.00f ),
                              config.get( "weaponRotate.z", 0.00f ) );

    meshTransf = Mat44::translation( translation );

    weaponTransf = Mat44::translation( weaponTransl );
    weaponTransf.rotateX( Math::rad( weaponRot.x ) );
    weaponTransf.rotateZ( Math::rad( weaponRot.z ) );
    weaponTransf.rotateY( Math::rad( weaponRot.y ) );

    const char* model = config.get( "model", "" );

    Buffer buffer( 16 * 1024 * 1024 );
    OutputStream ostream = buffer.outputStream();

    ostream.writeString( shaderName );

    if( !String::isEmpty( model ) ) {
      compiler.beginMesh();

      compiler.enable( CAP_UNIQUE );
      compiler.enable( CAP_CW );
      compiler.material( GL_SPECULAR, specular );

      DArray<MD3Tag> tags;
      buildMesh( model, 0, &tags );

      compiler.endMesh();

      MeshData mesh;
      compiler.getMeshData( &mesh );
      mesh.write( &ostream );
    }
    else if( forceStatic ) {
      compiler.beginMesh();

      compiler.enable( CAP_UNIQUE );
      compiler.enable( CAP_CW );
      compiler.material( GL_SPECULAR, specular );

      DArray<MD3Tag> tags;

      buildMesh( "lower", 0, &tags );

      foreach( tag, tags.citer() ) {
        if( String::equals( tag->name, "tag_torso" ) ) {
          Mat44 transf = Mat44( tag->rot[0], tag->rot[1], tag->rot[2], 0.0f,
                                tag->rot[3], tag->rot[4], tag->rot[5], 0.0f,
                                tag->rot[6], tag->rot[7], tag->rot[8], 0.0f,
                                -tag->pos[1] * scale, +tag->pos[0] * scale, +tag->pos[2] * scale, 1.0f );

          meshTransf = meshTransf * transf;

          tags.dealloc();
          break;
        }
      }

      buildMesh( "upper", 0, &tags );

      foreach( tag, tags.citer() ) {
        if( String::equals( tag->name, "tag_head" ) ) {
          Mat44 transf = Mat44( tag->rot[0], tag->rot[1], tag->rot[2], 0.0f,
                                tag->rot[3], tag->rot[4], tag->rot[5], 0.0f,
                                tag->rot[6], tag->rot[7], tag->rot[8], 0.0f,
                                -tag->pos[1] * scale, +tag->pos[0] * scale, +tag->pos[2] * scale, 1.0f );

          meshTransf = meshTransf * transf;

          tags.dealloc();
          break;
        }
      }

      buildMesh( "head", 0, &tags );

      compiler.endMesh();

      MeshData mesh;
      compiler.getMeshData( &mesh );
      mesh.write( &ostream );
    }

    if( forceStatic ) {
      log.print( "Writing to '%s%s' ...", sPath.cstr(), ".ozcSMM" );
      buffer.write( sPath + ".ozcSMM", ostream.length() );
      log.printEnd( " OK" );
    }
    else {
      log.print( "Writing to '%s%s' ...", sPath.cstr(), ".ozcMD3" );
      buffer.write( sPath + ".ozcMD3", ostream.length() );
      log.printEnd( " OK" );
    }

    shaderName.clear();
    config.clear();
    sPath.clear();

    log.unindent();
    log.println( "}" );
  }

#endif

}
}
