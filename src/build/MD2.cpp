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
 * @file build/MD2.cpp
 */

#include "stable.hpp"

#include "build/MD2.hpp"

#include "client/MD2.hpp"
#include "client/Context.hpp"
#include "client/Colours.hpp"

#include "build/Compiler.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace build
{

const Vec3 MD2::NORMALS[] =
{
  Vec3( -0.000000f, -0.525731f,  0.850651f ),
  Vec3( -0.238856f, -0.442863f,  0.864188f ),
  Vec3( -0.000000f, -0.295242f,  0.955423f ),
  Vec3( -0.500000f, -0.309017f,  0.809017f ),
  Vec3( -0.262866f, -0.162460f,  0.951056f ),
  Vec3( -0.000000f,  0.000000f,  1.000000f ),
  Vec3( -0.850651f,  0.000000f,  0.525731f ),
  Vec3( -0.716567f, -0.147621f,  0.681718f ),
  Vec3( -0.716567f,  0.147621f,  0.681718f ),
  Vec3( -0.525731f,  0.000000f,  0.850651f ),
  Vec3( -0.500000f,  0.309017f,  0.809017f ),
  Vec3( -0.000000f,  0.525731f,  0.850651f ),
  Vec3( -0.000000f,  0.295242f,  0.955423f ),
  Vec3( -0.238856f,  0.442863f,  0.864188f ),
  Vec3( -0.262866f,  0.162460f,  0.951056f ),
  Vec3( -0.147621f, -0.681718f,  0.716567f ),
  Vec3( -0.309017f, -0.809017f,  0.500000f ),
  Vec3( -0.425325f, -0.587785f,  0.688191f ),
  Vec3( -0.525731f, -0.850651f,  0.000000f ),
  Vec3( -0.442863f, -0.864188f,  0.238856f ),
  Vec3( -0.681718f, -0.716567f,  0.147621f ),
  Vec3( -0.587785f, -0.688191f,  0.425325f ),
  Vec3( -0.809017f, -0.500000f,  0.309017f ),
  Vec3( -0.864188f, -0.238856f,  0.442863f ),
  Vec3( -0.688191f, -0.425325f,  0.587785f ),
  Vec3( -0.681718f, -0.716567f, -0.147621f ),
  Vec3( -0.809017f, -0.500000f, -0.309017f ),
  Vec3( -0.850651f, -0.525731f,  0.000000f ),
  Vec3( -0.850651f,  0.000000f, -0.525731f ),
  Vec3( -0.864188f, -0.238856f, -0.442863f ),
  Vec3( -0.955423f,  0.000000f, -0.295242f ),
  Vec3( -0.951056f, -0.262866f, -0.162460f ),
  Vec3( -1.000000f,  0.000000f,  0.000000f ),
  Vec3( -0.955423f,  0.000000f,  0.295242f ),
  Vec3( -0.951056f, -0.262866f,  0.162460f ),
  Vec3( -0.864188f,  0.238856f,  0.442863f ),
  Vec3( -0.951056f,  0.262866f,  0.162460f ),
  Vec3( -0.809017f,  0.500000f,  0.309017f ),
  Vec3( -0.864188f,  0.238856f, -0.442863f ),
  Vec3( -0.951056f,  0.262866f, -0.162460f ),
  Vec3( -0.809017f,  0.500000f, -0.309017f ),
  Vec3( -0.525731f,  0.850651f,  0.000000f ),
  Vec3( -0.681718f,  0.716567f,  0.147621f ),
  Vec3( -0.681718f,  0.716567f, -0.147621f ),
  Vec3( -0.850651f,  0.525731f,  0.000000f ),
  Vec3( -0.688191f,  0.425325f,  0.587785f ),
  Vec3( -0.442863f,  0.864188f,  0.238856f ),
  Vec3( -0.587785f,  0.688191f,  0.425325f ),
  Vec3( -0.309017f,  0.809017f,  0.500000f ),
  Vec3( -0.147621f,  0.681718f,  0.716567f ),
  Vec3( -0.425325f,  0.587785f,  0.688191f ),
  Vec3( -0.295242f,  0.955423f,  0.000000f ),
  Vec3( -0.000000f,  1.000000f,  0.000000f ),
  Vec3( -0.162460f,  0.951056f,  0.262866f ),
  Vec3(  0.525731f,  0.850651f,  0.000000f ),
  Vec3(  0.295242f,  0.955423f,  0.000000f ),
  Vec3(  0.442863f,  0.864188f,  0.238856f ),
  Vec3(  0.162460f,  0.951056f,  0.262866f ),
  Vec3(  0.309017f,  0.809017f,  0.500000f ),
  Vec3(  0.147621f,  0.681718f,  0.716567f ),
  Vec3( -0.000000f,  0.850651f,  0.525731f ),
  Vec3( -0.442863f,  0.864188f, -0.238856f ),
  Vec3( -0.309017f,  0.809017f, -0.500000f ),
  Vec3( -0.162460f,  0.951056f, -0.262866f ),
  Vec3( -0.000000f,  0.525731f, -0.850651f ),
  Vec3( -0.147621f,  0.681718f, -0.716567f ),
  Vec3(  0.147621f,  0.681718f, -0.716567f ),
  Vec3( -0.000000f,  0.850651f, -0.525731f ),
  Vec3(  0.309017f,  0.809017f, -0.500000f ),
  Vec3(  0.442863f,  0.864188f, -0.238856f ),
  Vec3(  0.162460f,  0.951056f, -0.262866f ),
  Vec3( -0.716567f,  0.147621f, -0.681718f ),
  Vec3( -0.500000f,  0.309017f, -0.809017f ),
  Vec3( -0.688191f,  0.425325f, -0.587785f ),
  Vec3( -0.238856f,  0.442863f, -0.864188f ),
  Vec3( -0.425325f,  0.587785f, -0.688191f ),
  Vec3( -0.587785f,  0.688191f, -0.425325f ),
  Vec3( -0.716567f, -0.147621f, -0.681718f ),
  Vec3( -0.500000f, -0.309017f, -0.809017f ),
  Vec3( -0.525731f,  0.000000f, -0.850651f ),
  Vec3( -0.000000f, -0.525731f, -0.850651f ),
  Vec3( -0.238856f, -0.442863f, -0.864188f ),
  Vec3( -0.000000f, -0.295242f, -0.955423f ),
  Vec3( -0.262866f, -0.162460f, -0.951056f ),
  Vec3( -0.000000f,  0.000000f, -1.000000f ),
  Vec3( -0.000000f,  0.295242f, -0.955423f ),
  Vec3( -0.262866f,  0.162460f, -0.951056f ),
  Vec3(  0.238856f, -0.442863f, -0.864188f ),
  Vec3(  0.500000f, -0.309017f, -0.809017f ),
  Vec3(  0.262866f, -0.162460f, -0.951056f ),
  Vec3(  0.850651f,  0.000000f, -0.525731f ),
  Vec3(  0.716567f, -0.147621f, -0.681718f ),
  Vec3(  0.716567f,  0.147621f, -0.681718f ),
  Vec3(  0.525731f,  0.000000f, -0.850651f ),
  Vec3(  0.500000f,  0.309017f, -0.809017f ),
  Vec3(  0.238856f,  0.442863f, -0.864188f ),
  Vec3(  0.262866f,  0.162460f, -0.951056f ),
  Vec3(  0.864188f,  0.238856f, -0.442863f ),
  Vec3(  0.809017f,  0.500000f, -0.309017f ),
  Vec3(  0.688191f,  0.425325f, -0.587785f ),
  Vec3(  0.681718f,  0.716567f, -0.147621f ),
  Vec3(  0.587785f,  0.688191f, -0.425325f ),
  Vec3(  0.425325f,  0.587785f, -0.688191f ),
  Vec3(  0.955423f,  0.000000f, -0.295242f ),
  Vec3(  1.000000f,  0.000000f,  0.000000f ),
  Vec3(  0.951056f,  0.262866f, -0.162460f ),
  Vec3(  0.850651f,  0.000000f,  0.525731f ),
  Vec3(  0.955423f,  0.000000f,  0.295242f ),
  Vec3(  0.864188f,  0.238856f,  0.442863f ),
  Vec3(  0.951056f,  0.262866f,  0.162460f ),
  Vec3(  0.809017f,  0.500000f,  0.309017f ),
  Vec3(  0.681718f,  0.716567f,  0.147621f ),
  Vec3(  0.850651f,  0.525731f,  0.000000f ),
  Vec3(  0.864188f, -0.238856f, -0.442863f ),
  Vec3(  0.809017f, -0.500000f, -0.309017f ),
  Vec3(  0.951056f, -0.262866f, -0.162460f ),
  Vec3(  0.525731f, -0.850651f,  0.000000f ),
  Vec3(  0.681718f, -0.716567f, -0.147621f ),
  Vec3(  0.681718f, -0.716567f,  0.147621f ),
  Vec3(  0.850651f, -0.525731f,  0.000000f ),
  Vec3(  0.809017f, -0.500000f,  0.309017f ),
  Vec3(  0.864188f, -0.238856f,  0.442863f ),
  Vec3(  0.951056f, -0.262866f,  0.162460f ),
  Vec3(  0.442863f, -0.864188f,  0.238856f ),
  Vec3(  0.309017f, -0.809017f,  0.500000f ),
  Vec3(  0.587785f, -0.688191f,  0.425325f ),
  Vec3(  0.147621f, -0.681718f,  0.716567f ),
  Vec3(  0.238856f, -0.442863f,  0.864188f ),
  Vec3(  0.425325f, -0.587785f,  0.688191f ),
  Vec3(  0.500000f, -0.309017f,  0.809017f ),
  Vec3(  0.716567f, -0.147621f,  0.681718f ),
  Vec3(  0.688191f, -0.425325f,  0.587785f ),
  Vec3(  0.262866f, -0.162460f,  0.951056f ),
  Vec3(  0.238856f,  0.442863f,  0.864188f ),
  Vec3(  0.262866f,  0.162460f,  0.951056f ),
  Vec3(  0.500000f,  0.309017f,  0.809017f ),
  Vec3(  0.716567f,  0.147621f,  0.681718f ),
  Vec3(  0.525731f,  0.000000f,  0.850651f ),
  Vec3(  0.688191f,  0.425325f,  0.587785f ),
  Vec3(  0.425325f,  0.587785f,  0.688191f ),
  Vec3(  0.587785f,  0.688191f,  0.425325f ),
  Vec3( -0.295242f, -0.955423f,  0.000000f ),
  Vec3( -0.162460f, -0.951056f,  0.262866f ),
  Vec3( -0.000000f, -1.000000f,  0.000000f ),
  Vec3( -0.000000f, -0.850651f,  0.525731f ),
  Vec3(  0.295242f, -0.955423f,  0.000000f ),
  Vec3(  0.162460f, -0.951056f,  0.262866f ),
  Vec3( -0.442863f, -0.864188f, -0.238856f ),
  Vec3( -0.162460f, -0.951056f, -0.262866f ),
  Vec3( -0.309017f, -0.809017f, -0.500000f ),
  Vec3(  0.442863f, -0.864188f, -0.238856f ),
  Vec3(  0.162460f, -0.951056f, -0.262866f ),
  Vec3(  0.309017f, -0.809017f, -0.500000f ),
  Vec3( -0.147621f, -0.681718f, -0.716567f ),
  Vec3(  0.147621f, -0.681718f, -0.716567f ),
  Vec3( -0.000000f, -0.850651f, -0.525731f ),
  Vec3( -0.587785f, -0.688191f, -0.425325f ),
  Vec3( -0.425325f, -0.587785f, -0.688191f ),
  Vec3( -0.688191f, -0.425325f, -0.587785f ),
  Vec3(  0.688191f, -0.425325f, -0.587785f ),
  Vec3(  0.425325f, -0.587785f, -0.688191f ),
  Vec3(  0.587785f, -0.688191f, -0.425325f )
};

void MD2::build( const char* path )
{
  String sPath = path;
  String modelFile = sPath + "/tris.md2";
  String skinFile = sPath + "/skin.png";
  String configFile = sPath + "/config.rc";

  log.println( "Prebuilding MD2 model '%s' {", path );
  log.indent();

  Config config;
  config.load( configFile );

  File file( modelFile );
  if( !file.map() ) {
    throw Exception( "MD2 reading failed" );
  }

  InputStream is = file.inputStream();

  char id[4];
  id[0] = is.readChar();
  id[1] = is.readChar();
  id[2] = is.readChar();
  id[3] = is.readChar();

  int version = is.readInt();

  if( id[0] != 'I' || id[1] != 'D' || id[2] != 'P' || id[3] != '2' || version != 8 ) {
    throw Exception( "Wrong Quake 2 MD2 format" );
  }

  int skinWidth       = is.readInt();
  int skinHeight      = is.readInt();
  int frameSize       = is.readInt();

  // MD2Header::offGLCmds
  is.readInt();

  int nFramePositions = is.readInt();
  int nTexCoords      = is.readInt();
  int nTriangles      = is.readInt();

  // MD2Header::nGLCmds
  is.readInt();

  int nFrames         = is.readInt();

  // MD2Header::offSkins
  is.readInt();

  int offTexCoords    = is.readInt();
  int offTriangles    = is.readInt();
  int offFrames       = is.readInt();

  // MD2Header::offGLCmds
  is.readInt();
  // MD2Header::offEnd
  is.readInt();

  if( nFrames <= 0 || nFramePositions <= 0 ) {
    throw Exception( "MD2 model loading error" );
  }

  if( config.get( "forceStatic", false ) ) {
    nFrames = 1;
  }
  String shaderName   = config.get( "shader", nFrames == 1 ? "mesh" : "md2" );
  float  specular     = config.get( "specular", 0.0f );
  float  scale        = config.get( "scale", 0.04f );

  Vec3   translation  = Vec3( config.get( "translate.x", +0.00f ),
                              config.get( "translate.y", +0.00f ),
                              config.get( "translate.z", -0.04f ) );
  Vec3   jumpTransl   = Vec3( config.get( "jumpTranslate.x", 0.00f ),
                              config.get( "jumpTranslate.y", 0.00f ),
                              config.get( "jumpTranslate.z", 0.00f ) );
  Vec3   weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                              config.get( "weaponTranslate.y", 0.00f ),
                              config.get( "weaponTranslate.z", 0.00f ) );
  Vec3   weaponRot    = Vec3( config.get( "weaponRotate.x", 0.00f ),
                              config.get( "weaponRotate.y", 0.00f ),
                              config.get( "weaponRotate.z", 0.00f ) );

  config.clear( true );

  Mat44 weaponTransf = Mat44::translation( weaponTransl );
  weaponTransf.rotateY( Math::rad( weaponRot.y ) );
  weaponTransf.rotateX( Math::rad( weaponRot.x ) );
  weaponTransf.rotateZ( Math::rad( weaponRot.z ) );

  DArray<TexCoord>    texCoords( nTexCoords );
  DArray<MD2Triangle> triangles( nTriangles );
  DArray<Vec3>        normals( nFrames * nFramePositions );
  DArray<Point3>      vertices( nFrames * nFramePositions );

  is.reset();
  is.forward( offFrames );

  const char* frameData = is.forward( nFrames * frameSize );

  for( int i = 0; i < nFrames; ++i ) {
    const MD2Frame& frame = *reinterpret_cast<const MD2Frame*>( &frameData[i * frameSize] );

    for( int j = 0; j < nFramePositions; ++j ) {
      Vec3&   normal = normals[i * nFramePositions + j];
      Point3& vertex = vertices[i * nFramePositions + j];

      normal   = NORMALS[ frame.verts[j].normal ];

      vertex.x = float( frame.verts[j].p[1] ) * -frame.scale[1] - frame.translate[1];
      vertex.y = float( frame.verts[j].p[0] ) *  frame.scale[0] + frame.translate[0];
      vertex.z = float( frame.verts[j].p[2] ) *  frame.scale[2] + frame.translate[2];

      vertex.x = vertex.x * scale + translation.x;
      vertex.y = vertex.y * scale + translation.y;
      vertex.z = vertex.z * scale + translation.z;

      if( client::MD2::ANIM_LIST[Anim::JUMP].firstFrame <= i &&
          i <= client::MD2::ANIM_LIST[Anim::JUMP].lastFrame ) {
        vertex += jumpTransl;
      }
    }
  }

  is.reset();
  is.forward( offTexCoords );

  for( int i = 0; i < texCoords.length(); ++i ) {
    texCoords[i].u = float( is.readShort() ) / float( skinWidth );
    texCoords[i].v = float( skinHeight - is.readShort() ) / float( skinHeight );
  }

  is.reset();
  is.forward( offTriangles );

  for( int i = 0; i < triangles.length(); ++i ) {
    triangles[i].vertices[0]  = is.readShort();
    triangles[i].vertices[1]  = is.readShort();
    triangles[i].vertices[2]  = is.readShort();
    triangles[i].texCoords[0] = is.readShort();
    triangles[i].texCoords[1] = is.readShort();
    triangles[i].texCoords[2] = is.readShort();
  }

  file.unmap();

  compiler.beginMesh();
  compiler.enable( CAP_UNIQUE );
  compiler.enable( CAP_CW );
  compiler.material( GL_SPECULAR, specular );
  compiler.texture( skinFile );

  compiler.begin( GL_TRIANGLES );

  for( int i = 0; i < nTriangles; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      compiler.texCoord( texCoords[ triangles[i].texCoords[j] ] );

      if( nFrames == 1 ) {
        compiler.normal( normals[ triangles[i].vertices[j] ] );
        compiler.vertex( vertices[ triangles[i].vertices[j] ] );
      }
      else {
        // vertex index in vertex animation buffer
        compiler.animVertex( triangles[i].vertices[j] );
      }
    }
  }

  compiler.end();
  compiler.endMesh();

  triangles.dealloc();
  texCoords.dealloc();

  MeshData mesh;
  compiler.getMeshData( &mesh );

  int nFrameVertices = mesh.vertices.length();

  library.shaderIndex( shaderName );

  if( nFrameVertices > MAX_VERTS ) {
    throw Exception( "MD2 model has too many vertices" );
  }

  BufferStream os;

  os.writeString( shaderName );

  // generate vertex data for animated MD2s
  if( nFrames != 1 ) {
    os.writeInt( nFrames );
    os.writeInt( nFrameVertices );
    os.writeInt( nFramePositions );
    os.writeMat44( weaponTransf );

    // write vertex positions for all frames
    for( int i = 0; i < nFrames; ++i ) {
      for( int j = 0; j < nFramePositions; ++j ) {
        os.writePoint3( vertices[i * nFramePositions + j] );
      }
    }
    // write vertex normals for all frames
    for( int i = 0; i < nFrames; ++i ) {
      for( int j = 0; j < nFramePositions; ++j ) {
        os.writeVec3( normals[i * nFramePositions + j] );
      }
    }

    // if we have an animated model, we use vertex position to save texture coordinate for vertex
    // texture to fetch the positions in both frames and interpolate them in vertex shader
    foreach( vertex, mesh.vertices.iter() ) {
      vertex->pos[0] = ( vertex->pos[0] + 0.5f ) / float( nFramePositions );
      vertex->pos[1] = 0.0f;
      vertex->pos[2] = 0.0f;
    }
  }

  normals.dealloc();
  vertices.dealloc();

  mesh.write( &os );

  if( nFrames != 1 ) {
    log.print( "Writing to '%s' ...", ( sPath + ".ozcMD2" ).cstr() );
    File( sPath + ".ozcMD2" ).write( &os );
    log.printEnd( " OK" );
  }
  else {
    log.print( "Writing to '%s' ...", ( sPath + ".ozcSMM" ).cstr() );
    File( sPath + ".ozcSMM" ).write( &os );
    log.printEnd( " OK" );
  }

  log.unindent();
  log.println( "}" );
}

}
}
