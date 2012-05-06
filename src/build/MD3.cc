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
 * @file build/MD3.cc
 */

#include "stable.hh"

#include "build/MD3.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

#include "build/Compiler.hh"

namespace oz
{
namespace build
{

void MD3::readAnimData()
{
  PhysFile animFile( sPath + "/animation.cfg" );

  String realPath = animFile.realDir() + "/" + animFile.path();

  FILE* fs = fopen( realPath, "r" );
  if( fs == null ) {
    throw Exception( "Reading animation data failed" );
  }

  char line[1024];

  while( fgets( line, 1024, fs ) != null ) {

  }

  fclose( fs );
}

void MD3::buildMesh( const char* name, int frame )
{
  Log::print( "Mesh '%s' ...", name );

  PhysFile file( String::str( "%s/%s.md3", sPath.cstr(), name ) );
  if( !file.map() ) {
    throw Exception( "Cannot mmap MD3 model part file '%s'", file.path().cstr() );
  }

  InputStream is = file.inputStream( Endian::LITTLE );

  MD3Header header;

  header.id[0]   = is.readChar();
  header.id[1]   = is.readChar();
  header.id[2]   = is.readChar();
  header.id[3]   = is.readChar();
  header.version = is.readInt();

  if( header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'P' || header.id[3] != '3' ) {
    throw Exception( "MD3 model part file has an invalid format" );
  }

  // header.fileName
  is.forward( 64 );

  header.flags       = is.readInt();
  header.nFrames     = is.readInt();
  header.nTags       = is.readInt();
  header.nSurfaces   = is.readInt();
  header.nSkins      = is.readInt();
  header.offFrames   = is.readInt();
  header.offTags     = is.readInt();
  header.offSurfaces = is.readInt();
  header.offEnd      = is.readInt();

  if( header.nFrames == 0 || header.nSurfaces == 0 ) {
    throw Exception( "Invalid MD3 header counts" );
  }

  if( String::equals( name, "lower" ) ) {
    nLowerFrames = header.nFrames;
  }
  else if( String::equals( name, "upper" ) ) {
    nUpperFrames = header.nFrames;
  }

  if( header.nTags != 0 ) {
    is.reset();
    is.forward( header.offTags );

    for( int i = 0; i < header.nTags; ++i ) {
      const char* tag = is.forward( 64 );

      float tx  = is.readFloat();
      float ty  = is.readFloat();
      float tz  = is.readFloat();

      float m00 = is.readFloat();
      float m01 = is.readFloat();
      float m02 = is.readFloat();
      float m10 = is.readFloat();
      float m11 = is.readFloat();
      float m12 = is.readFloat();
      float m20 = is.readFloat();
      float m21 = is.readFloat();
      float m22 = is.readFloat();

      Vec3  transl = Vec3( scale * -ty, scale * tx, scale * tz );
      Mat44 rotMat = Mat44( +m11, -m10, -m12, 0.0f,
                            -m01, +m00, +m02, 0.0f,
                            -m21, +m20, +m22, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f );

      if( String::equals( name, "lower" ) ) {
        if( String::equals( name, "tag_torso" ) ) {
          joints[i][client::MD3::JOINT_HIP].transl = transl;
          joints[i][client::MD3::JOINT_HIP].rot    = rotMat.toQuat();
        }
      }
      else if( String::equals( name, "upper" ) ) {
        if( String::equals( tag, "tag_head" ) ) {
          joints[i][client::MD3::JOINT_NECK].transl = transl;
          joints[i][client::MD3::JOINT_NECK].rot    = rotMat.toQuat();
        }
        else if( String::equals( tag, "tag_weapon" ) ) {
          joints[i][client::MD3::JOINT_WEAPON].transl = transl;
          joints[i][client::MD3::JOINT_WEAPON].rot    = rotMat.toQuat();
        }
      }
    }
  }

  // FIXME indexBase unused
  int indexBase = 0;

  is.reset();
  is.forward( header.offSurfaces );

  for( int i = 0; i < header.nSurfaces; ++i ) {
    int surfaceStart = is.length();

    MD3Surface surface;

    surface.id           = is.readInt();
    aCopy( surface.name, is.forward( 64 ), 64 );
    surface.flags        = is.readInt();

    surface.nFrames      = is.readInt();
    surface.nShaders     = is.readInt();
    surface.nVertices    = is.readInt();
    surface.nTriangles   = is.readInt();

    surface.offTriangles = is.readInt();
    surface.offShaders   = is.readInt();
    surface.offTexCoords = is.readInt();
    surface.offVertices  = is.readInt();
    surface.offEnd       = is.readInt();

    if( surface.nFrames == 0 || surface.nTriangles == 0 || surface.nShaders == 0 ||
        surface.nVertices == 0 )
    {
      throw Exception( "Invalid MD3 surface counts" );
    }

    if( surface.nFrames != header.nFrames ) {
      throw Exception( "Invalid MD3 surface # of frames" );
    }

    String texture;

    DArray<MD3Triangle> surfaceTriangles( surface.nTriangles );
    DArray<MD3Shader>   surfaceShaders( surface.nShaders );
    DArray<TexCoord>    surfaceTexCoords( surface.nVertices );
    DArray<MD3Vertex>   surfaceVertices( surface.nFrames * surface.nVertices );
    DArray<Vec3>        normals( surfaceVertices.length() );
    DArray<Point>       vertices( surfaceVertices.length() );

    is.reset();
    is.forward( surfaceStart + surface.offTriangles );

    for( int i = 0; i < surfaceTriangles.length(); ++i ) {
      surfaceTriangles[i].vertices[0] = is.readInt();
      surfaceTriangles[i].vertices[1] = is.readInt();
      surfaceTriangles[i].vertices[2] = is.readInt();
    }

    is.reset();
    is.forward( surfaceStart + surface.offShaders );

    for( int i = 0; i < surfaceShaders.length(); ++i ) {
      aCopy( surfaceShaders[i].name, is.forward( 64 ), 64 );
      surfaceShaders[i].index = is.readInt();
    }

    if( skin.isEmpty() ) {
      PhysFile skinFile( String::replace( surfaceShaders[0].name, '\\', '/' ) );
      texture = skinFile.baseName();
    }
    else {
      PhysFile skinFile( skin );
      texture = skinFile.baseName();
    }

    is.reset();
    is.forward( surfaceStart + surface.offTexCoords );

    for( int i = 0; i < surfaceTexCoords.length(); ++i ) {
      surfaceTexCoords[i].u = is.readFloat();
      surfaceTexCoords[i].v = 1.0f - is.readFloat();
    }

    is.reset();
    is.forward( surfaceStart + surface.offVertices );

    for( int i = 0; i < surfaceVertices.length(); ++i ) {
      vertices[i].y = float( +is.readShort() ) / 64.0f * scale;
      vertices[i].x = float( -is.readShort() ) / 64.0f * scale;
      vertices[i].z = float( +is.readShort() ) / 64.0f * scale;

      float h  = float( is.readChar() ) / 255.0f * Math::TAU;
      float v  = float( is.readChar() ) / 255.0f * Math::TAU;
      float xy = Math::sin( v );

      normals[i].y = +Math::cos( h ) * xy;
      normals[i].x = -Math::sin( h ) * xy;
      normals[i].z = +Math::cos( v );
    }

    is.reset();
    is.forward( surfaceStart + surface.offEnd );

    compiler.texture( sPath + "/" + texture );

    compiler.begin( GL_TRIANGLES );

    for( int i = 0; i < surfaceTriangles.length(); ++i ) {
      for( int j = 0; j < 3; ++j ) {
        int k = surfaceTriangles[i].vertices[j];
        int l = frame == -1 ? k : frame * surface.nVertices + k;

        compiler.texCoord( surfaceTexCoords[k] );
        compiler.normal( meshTransf * normals[l] );
        compiler.vertex( meshTransf * vertices[l] );
      }
    }

    compiler.end();

    indexBase += surface.nVertices;
  }

  file.unmap();

  Log::printEnd( " OK" );
}

void MD3::load()
{
  PhysFile configFile( sPath + "/config.rc" );

  Config config;
  config.load( configFile );

  scale      = config.get( "scale", 0.04f );
  skin       = config.get( "skin", "" );

  model      = config.get( "model", "" );
  frame      = config.get( "frame", -1 );
  lowerFrame = config.get( "lowerFrame", -1 );
  upperFrame = config.get( "upperFrame", -1 );
  shaderName = config.get( "shader", frame == -1 ? "md3" : "mesh" );

  Vec3 weaponTransl = Vec3( config.get( "weaponTranslate.x", 0.00f ),
                            config.get( "weaponTranslate.y", 0.00f ),
                            config.get( "weaponTranslate.z", 0.00f ) );
  Vec3 weaponRot    = Vec3( config.get( "weaponRotate.x", 0.00f ),
                            config.get( "weaponRotate.y", 0.00f ),
                            config.get( "weaponRotate.z", 0.00f ) );

  Mat44 weaponTransf = Mat44::translation( weaponTransl );
  weaponTransf.rotateX( Math::rad( weaponRot.x ) );
  weaponTransf.rotateZ( Math::rad( weaponRot.z ) );
  weaponTransf.rotateY( Math::rad( weaponRot.y ) );

  config.clear( true );
}

void MD3::save()
{
  BufferStream os;

  os.writeString( shaderName );

  if( !String::isEmpty( model ) ) {
    if( frame == -1 ) {
      throw Exception( "Custom models can only be static. Must specify frame" );
    }

    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );

    meshTransf = Mat44::ID;

    buildMesh( model, frame );

    compiler.endMesh();

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }
  else if( frame != -1 ) {
    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );

    meshTransf = Mat44::ID;

    buildMesh( "lower", frame );

    meshTransf = meshTransf * Mat44::translation( joints[frame][client::MD3::JOINT_HIP].transl );
    meshTransf = meshTransf * Mat44::rotation( joints[frame][client::MD3::JOINT_HIP].rot );

    buildMesh( "upper", frame );

    meshTransf = meshTransf * Mat44::translation( joints[frame][client::MD3::JOINT_NECK].transl );
    meshTransf = meshTransf * Mat44::rotation( joints[frame][client::MD3::JOINT_NECK].rot );

    buildMesh( "head", 0 );

    compiler.endMesh();

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }
  else {
    compiler.beginMesh();

    compiler.enable( CAP_UNIQUE );
    compiler.enable( CAP_CW );

    compiler.component( 0 );
    buildMesh( "lower", frame );

    compiler.component( 1 );
    buildMesh( "upper", frame );

    compiler.component( 2 );
    buildMesh( "head", 0 );

    compiler.endMesh();

    os.writeInt( nLowerFrames );
    os.writeInt( nUpperFrames );

    for( int i = 0; i < nLowerFrames; ++i ) {
      os.writeVec3( joints[i][client::MD3::JOINT_HIP].transl );
      os.writeQuat( joints[i][client::MD3::JOINT_HIP].rot );
    }
    for( int i = 0; i < nUpperFrames; ++i ) {
      os.writeVec3( joints[i][client::MD3::JOINT_NECK].transl );
      os.writeQuat( joints[i][client::MD3::JOINT_NECK].rot );
    }
    for( int i = 0; i < nUpperFrames; ++i ) {
      os.writeVec3( joints[i][client::MD3::JOINT_WEAPON].transl );
      os.writeQuat( joints[i][client::MD3::JOINT_WEAPON].rot );
    }

    MeshData mesh;
    compiler.getMeshData( &mesh );
    mesh.write( &os );
  }

  File::mkdir( sPath );

  if( frame != 1 ) {
    File destFile( sPath + "/data.ozcMD3" );

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
}

MD3::MD3( const char* path ) :
  sPath( path )
{}

void MD3::build( const char* path )
{
  Log::println( "Prebuilding MD3 model '%s' {", path );
  Log::indent();

  MD3* md3 = new MD3( path );
  md3->load();
  md3->save();

  Log::unindent();
  Log::println( "}" );
}

}
}
