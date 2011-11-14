/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/MD3.cpp
 */

#include "stable.hpp"

#include "build/MD3.hpp"

#include "client/Context.hpp"
#include "client/OpenGL.hpp"

#include "build/Compiler.hpp"

namespace oz
{
namespace build
{

MD3::AnimInfo MD3::legsAnimList[LEGS_ANIM_MAX];
MD3::AnimInfo MD3::torsoAnimList[TORSO_ANIM_MAX];
MD3::Joint    MD3::joints[MAX_FRAMES][JOINTS_MAX];

String MD3::sPath;
Config MD3::config;

String MD3::skin;
float  MD3::scale;
Mat44  MD3::meshTransf;
int    MD3::nTags;

DArray<MD3::MD3Tag> MD3::tags;

void MD3::readAnimData()
{
  String animFile = sPath + "/animation.cfg";

  FILE* file = fopen( animFile, "r" );
  if( file == null ) {
    throw Exception( "Reading animation data failed" );
  }

  char line[1024];

  while( fgets( line, 1024, file ) != null ) {

  }
}

MD3::Joint MD3::toJoint( const MD3Tag* tag )
{
  Joint joint;

  auto& rot = tag->rot;
  auto& pos = tag->pos;

  float w2 = Math::sqrt( 1.0f + rot[0]*rot[0] + rot[4]*rot[4] + rot[8]*rot[8] );
  float x  = ( rot[5] - rot[7] ) / ( 2.0f * w2 );
  float y  = ( rot[6] - rot[2] ) / ( 2.0f * w2 );
  float z  = ( rot[1] - rot[3] ) / ( 2.0f * w2 );

  joint.rot    = Quat( x, y, z, w2 / 2.0f );
  joint.transl = Vec3( pos );

  return joint;
}

void MD3::buildMesh( const char* name, int frame )
{
  log.print( "Mesh '%s' ...", name );

  String modelFile = sPath + "/" + String( name ) + ".md3";

  FILE* file = fopen( modelFile, "rb" );
  if( file == null ) {
    throw Exception( "MD3 model part file '%s' not found", modelFile.cstr() );
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

  nTags = header.nTags;

  if( header.nTags != 0 ) {
    tags.alloc( header.nFrames * header.nTags );

    fseek( file, header.offTags, SEEK_SET );
    fread( tags, sizeof( MD3Tag ), size_t( tags.length() ), file );

    if( String::equals( name, "lower" ) ) {
      if( header.nTags != 1 ) {
        throw Exception( "lower.md3 should only have one tag defined (tag_torso)" );
      }

      for( int i = 0; i < header.nFrames; ++i ) {
        joints[i][JOINT_HIP] = toJoint( &tags[i] );
      }
    }
    else if( String::equals( name, "upper" ) ) {
      for( int i = 0; i < header.nFrames; ++i ) {
        for( int j = 0; j < header.nTags; ++j ) {
          if( String::equals( tags[i * header.nTags + j].name, "tag_torso" ) ) {

          }
        }
      }
    }
  }

  // FIXME indexBase unused
  int indexBase = 0;

  fseek( file, header.offSurfaces, SEEK_SET );

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
    DArray<Vec3>        normals( surfaceVertices.length() );
    DArray<Point3>      vertices( surfaceVertices.length() );

    fseek( file, surfaceStart + surface.offTriangles, SEEK_SET );
    fread( surfaceTriangles, sizeof( MD3Triangle ), size_t( surfaceTriangles.length() ), file );

    fseek( file, surfaceStart + surface.offShaders, SEEK_SET );
    fread( surfaceShaders, sizeof( MD3Shader ), size_t( surfaceShaders.length() ), file );

    fseek( file, surfaceStart + surface.offTexCoords, SEEK_SET );
    fread( surfaceTexCoords, sizeof( TexCoord ), size_t( surfaceTexCoords.length() ), file );

    fseek( file, surfaceStart + surface.offVertices, SEEK_SET );
    fread( surfaceVertices, sizeof( MD3Vertex ), size_t( surfaceVertices.length() ), file );

    fseek( file, surfaceStart + surface.offEnd, SEEK_SET );

    if( surface.nFrames != header.nFrames ) {
      throw Exception( "Invalid MD3 surface # of frames" );
    }

    for( int i = 0; i < surfaceVertices.length(); ++i ) {
      float h  = float( surfaceVertices[i].normal[0] ) / 255.0f * Math::TAU;
      float v  = float( surfaceVertices[i].normal[1] ) / 255.0f * Math::TAU;
      float xy = Math::sin( v );

      normals[i].x = xy * Math::sin( -h );
      normals[i].y = xy * Math::cos( +h );
      normals[i].z = -Math::cos( v );

      vertices[i].x = float( -surfaceVertices[i].pos[1] ) / 64.0f * scale;
      vertices[i].y = float( +surfaceVertices[i].pos[0] ) / 64.0f * scale;
      vertices[i].z = float( +surfaceVertices[i].pos[2] ) / 64.0f * scale;
    }

    if( skin.isEmpty() ) {
      File skinFile( String::replace( surfaceShaders[0].name, '\\', '/' ).cstr() );
      skin = skinFile.name();
    }

    compiler.texture( sPath + "/" + skin );

    compiler.begin( GL_TRIANGLES );

    for( int i = 0; i < surfaceTriangles.length(); ++i ) {
      for( int j = 0; j < 3; ++j ) {
        int k = surfaceTriangles[i].vertices[j];

        if( frame != -1 ) {
          int l = frame * surface.nVertices + k;

          compiler.texCoord( surfaceTexCoords[k].s, 1.0f - surfaceTexCoords[k].t );
          compiler.normal( meshTransf * normals[l] );
          compiler.vertex( meshTransf * vertices[l] );
        }
      }
    }

    compiler.end();

    indexBase += surface.nVertices;
  }

  log.printEnd( " OK" );
}

void MD3::build( const char* path )
{
  sPath = path;

  String configFile = sPath + "/config.rc";

  log.println( "Prebuilding MD3 model '%s' {", path );
  log.indent();

  Config config;
  config.load( configFile );

  scale               = config.get( "scale", 0.04f );
  skin                = config.get( "skin", "" );

  String model        = config.get( "model", "" );
  int    frame        = config.get( "frame", 0 );
  String shaderName   = config.get( "shader", frame == -1 ? "md3" : "mesh" );
  float  specular     = config.get( "specular", 0.0f );

  Vec3   weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                              config.get( "weaponTranslate.y", 0.00f ),
                              config.get( "weaponTranslate.z", 0.00f ) );
  Vec3   weaponRot    = Vec3( config.get( "weaponRotate.x", 0.00f ),
                              config.get( "weaponRotate.y", 0.00f ),
                              config.get( "weaponRotate.z", 0.00f ) );

  meshTransf = Mat44::ID;

  Mat44 weaponTransf = Mat44::translation( weaponTransl );
  weaponTransf.rotateX( Math::rad( weaponRot.x ) );
  weaponTransf.rotateZ( Math::rad( weaponRot.z ) );
  weaponTransf.rotateY( Math::rad( weaponRot.y ) );

  BufferStream os;

  os.writeString( shaderName );

  if( !String::isEmpty( model ) ) {
    if( frame == -1 ) {
      throw Exception( "Custom models can only be static. Must specify frame" );
    }

    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );
    compiler.material( GL_SPECULAR, specular );

    buildMesh( model, frame );

    compiler.endMesh();

    tags.dealloc();

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }
  else if( frame != -1 ) {
    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );
    compiler.material( GL_SPECULAR, specular );

    buildMesh( "lower", frame );

    for( int i = frame * nTags; i < ( frame + 1 ) * nTags; ++i ) {
      MD3Tag& tag = tags[i];

      if( String::equals( tag.name, "tag_torso" ) ) {
        Mat44 transf = Mat44( +tag.rot[4], -tag.rot[3], -tag.rot[5], 0.0f,
                              -tag.rot[1], +tag.rot[0], +tag.rot[2], 0.0f,
                              -tag.rot[7], +tag.rot[6], +tag.rot[8], 0.0f,
                              -tag.pos[1] * scale, +tag.pos[0] * scale, +tag.pos[2] * scale, 1.0f );

        meshTransf = meshTransf * transf;
        break;
      }
    }
    tags.dealloc();

    buildMesh( "upper", frame );

    for( int i = frame * nTags; i < ( frame + 1 ) * nTags; ++i ) {
      MD3Tag& tag = tags[i];

      if( String::equals( tag.name, "tag_head" ) ) {
        Mat44 transf = Mat44( +tag.rot[4], -tag.rot[3], -tag.rot[5], 0.0f,
                              -tag.rot[1], +tag.rot[0], +tag.rot[2], 0.0f,
                              -tag.rot[7], +tag.rot[6], +tag.rot[8], 0.0f,
                              -tag.pos[1] * scale, +tag.pos[0] * scale, +tag.pos[2] * scale, 1.0f );

        meshTransf = meshTransf * transf;
        break;
      }
    }
    tags.dealloc();

    buildMesh( "head", 0 );

    compiler.endMesh();

    tags.dealloc();

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }
  else {
    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );
    compiler.material( GL_SPECULAR, specular );

    compiler.component( 0 );
    buildMesh( "lower", frame );

    compiler.component( 1 );
    buildMesh( "upper", frame );

    compiler.component( 2 );
    buildMesh( "head", 0 );
    tags.dealloc();

    compiler.endMesh();

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }

  if( frame != -1 ) {
    log.print( "Writing to '%s%s' ...", sPath.cstr(), ".ozcSMM" );
    File( sPath + ".ozcSMM" ).write( &os );
    log.printEnd( " OK" );
  }
  else {
    log.print( "Writing to '%s%s' ...", sPath.cstr(), ".ozcMD3" );
    File( sPath + ".ozcMD3" ).write( &os );
    log.printEnd( " OK" );
  }

  config.clear( true );

  shaderName.dealloc();
  sPath.dealloc();
  skin.dealloc();

  log.unindent();
  log.println( "}" );
}

}
}
