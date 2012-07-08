/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file build/MD2.cc
 */

#include "stable.hh"

#include "build/MD2.hh"

#include "client/MD2.hh"
#include "client/Context.hh"

#include "build/Compiler.hh"

#include "client/OpenGL.hh"

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

  PFile modelFile( sPath + "/tris.md2" );
  PFile configFile( sPath + "/config.json" );
  String skinPath = sPath + "/skin";

  Log::println( "Prebuilding MD2 model '%s' {", path );
  Log::indent();

  JSON config;
  config.load( configFile );

  if( !modelFile.map() ) {
    throw Exception( "MD2 reading failed" );
  }

  InputStream is = modelFile.inputStream( Endian::LITTLE );

  MD2Header header;

  header.id[0]   = is.readChar();
  header.id[1]   = is.readChar();
  header.id[2]   = is.readChar();
  header.id[3]   = is.readChar();
  header.version = is.readInt();

  if( header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '2' ||
      header.version != 8 )
  {
    throw Exception( "Wrong Quake 2 MD2 format" );
  }

  header.skinWidth  = is.readInt();
  header.skinHeight = is.readInt();
  header.frameSize  = is.readInt();

  // MD2Header::nSkins
  is.readInt();

  header.nFramePositions = is.readInt();
  header.nTexCoords      = is.readInt();
  header.nTriangles      = is.readInt();

  // MD2Header::nGLCmds
  is.readInt();

  header.nFrames = is.readInt();

  // MD2Header::offSkins
  is.readInt();

  header.offTexCoords = is.readInt();
  header.offTriangles = is.readInt();
  header.offFrames    = is.readInt();

  // MD2Header::offGLCmds
  is.readInt();
  // MD2Header::offEnd
  is.readInt();

  if( header.nFrames <= 0 || header.nFramePositions <= 0 ) {
    throw Exception( "MD2 model loading error" );
  }

  if( config["forceStatic"].get( false ) ) {
    header.nFrames = 1;
  }
  String shaderName   = config["shader"].get( header.nFrames == 1 ? "mesh" : "dmesh" );
  float  scale        = config["scale"].get( 0.04f );

  Vec3   translation  = Vec3( config["translate.x"].get( +0.00f ),
                              config["translate.y"].get( +0.00f ),
                              config["translate.z"].get( -0.04f ) );
  Vec3   jumpTransl   = Vec3( config["jumpTranslate.x"].get( 0.00f ),
                              config["jumpTranslate.y"].get( 0.00f ),
                              config["jumpTranslate.z"].get( 0.00f ) );
  Vec3   weaponTransl = Vec3( config["weaponTranslate.x"].get( 0.00f ),
                              config["weaponTranslate.y"].get( 0.00f ),
                              config["weaponTranslate.z"].get( 0.00f ) );
  Vec3   weaponRot    = Vec3( config["weaponRotate.x"].get( 0.00f ),
                              config["weaponRotate.y"].get( 0.00f ),
                              config["weaponRotate.z"].get( 0.00f ) );

  config.clear( true );

  Mat44 weaponTransf = Mat44::translation( weaponTransl );
  weaponTransf.rotateY( Math::rad( weaponRot.y ) );
  weaponTransf.rotateX( Math::rad( weaponRot.x ) );
  weaponTransf.rotateZ( Math::rad( weaponRot.z ) );

  DArray<TexCoord>    texCoords( header.nTexCoords );
  DArray<MD2Triangle> triangles( header.nTriangles );
  DArray<Vec3>        normals( header.nFrames * header.nFramePositions );
  DArray<Point>       positions( header.nFrames * header.nFramePositions );

  is.reset();
  is.forward( header.offFrames );

  const char* frameData = is.forward( header.nFrames * header.frameSize );

  for( int i = 0; i < header.nFrames; ++i ) {
    const MD2Frame& frame = *reinterpret_cast<const MD2Frame*>( &frameData[i * header.frameSize] );

    for( int j = 0; j < header.nFramePositions; ++j ) {
      Vec3&  normal = normals[i * header.nFramePositions + j];
      Point& position = positions[i * header.nFramePositions + j];

      normal = NORMALS[ frame.verts[j].normal ];

      position.x = float( frame.verts[j].p[1] ) * -frame.scale[1] - frame.translate[1];
      position.y = float( frame.verts[j].p[0] ) *  frame.scale[0] + frame.translate[0];
      position.z = float( frame.verts[j].p[2] ) *  frame.scale[2] + frame.translate[2];

      position.x = position.x * scale + translation.x;
      position.y = position.y * scale + translation.y;
      position.z = position.z * scale + translation.z;

      if( client::MD2::ANIM_LIST[client::MD2::ANIM_JUMP].firstFrame <= i &&
          i <= client::MD2::ANIM_LIST[client::MD2::ANIM_JUMP].lastFrame )
      {
        position += jumpTransl;
      }
    }
  }

  is.reset();
  is.forward( header.offTexCoords );

  for( int i = 0; i < texCoords.length(); ++i ) {
    texCoords[i].u = float( is.readShort() ) / float( header.skinWidth );
    texCoords[i].v = float( header.skinHeight - is.readShort() ) / float( header.skinHeight );
  }

  is.reset();
  is.forward( header.offTriangles );

  for( int i = 0; i < triangles.length(); ++i ) {
    triangles[i].vertices[0]  = is.readShort();
    triangles[i].vertices[1]  = is.readShort();
    triangles[i].vertices[2]  = is.readShort();
    triangles[i].texCoords[0] = is.readShort();
    triangles[i].texCoords[1] = is.readShort();
    triangles[i].texCoords[2] = is.readShort();
  }

  modelFile.unmap();

  compiler.beginMesh();
  compiler.enable( Compiler::UNIQUE );
  compiler.enable( Compiler::CLOCKWISE );

  if( header.nFrames != 1 ) {
    compiler.anim( header.nFrames, header.nFramePositions );
    compiler.animPositions( positions[0] );
    compiler.animNormals( normals[0] );
  }

  compiler.shader( shaderName );
  compiler.texture( skinPath );

  compiler.begin( Compiler::TRIANGLES );

  for( int i = 0; i < header.nTriangles; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      compiler.texCoord( texCoords[ triangles[i].texCoords[j] ] );

      if( header.nFrames == 1 ) {
        compiler.normal( normals[ triangles[i].vertices[j] ] );
        compiler.vertex( positions[ triangles[i].vertices[j] ] );
      }
      else {
        // vertex index in vertex animation buffer
        compiler.animVertex( triangles[i].vertices[j] );
      }
    }
  }

  compiler.end();
  compiler.endMesh();

  texCoords.dealloc();
  triangles.dealloc();
  normals.dealloc();
  positions.dealloc();

  BufferStream os;

  // generate vertex data for animated MD2s
  if( header.nFrames != 1 ) {
    os.writeMat44( weaponTransf );
  }

  compiler.writeMesh( &os );

  File::mkdir( sPath );

  if( header.nFrames != 1 ) {
    File destFile( sPath + "/data.ozcMD2" );

    Log::print( "Writing to '%s' ...", destFile.path().cstr() );

    if( !destFile.write( os.begin(), os.length() ) ) {
      throw Exception( "Failed to write '%s'", destFile.path().cstr() );
    }

    Log::printEnd( " OK" );
  }
  else {
    File destFile( sPath + "/data.ozcSMM" );

    Log::print( "Writing to '%s' ...", destFile.path().cstr() );

    if( !destFile.write( os.begin(), os.length() ) ) {
      throw Exception( "Failed to write '%s'", destFile.path().cstr() );
    }

    Log::printEnd( " OK" );
  }

  Log::unindent();
  Log::println( "}" );
}

}
}
