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
 * @file build/OBJ.cc
 */

#include "stable.hh"

#include "build/OBJ.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

#include "build/Compiler.hh"

namespace oz
{
namespace build
{

char* OBJ::skipSpaces( char* pos )
{
  while( *pos == ' ' || *pos == '\t' ) {
    ++pos;
  }
  return pos;
}

char* OBJ::readWord( char* pos )
{
  while( *pos != ' ' && *pos != '\t' && *pos != '\n' && *pos != EOF ) {
    ++pos;
  }
  return pos;
}

void OBJ::readVertexData( char* pos )
{
  // currently pos points to the char just after 'v'

  // vertex coords
  if( *pos == ' ' ) {
    ++pos;

    float x, y, z;
    if( sscanf( pos, "%f %f %f", &x, &y, &z ) != 3 ) {
      throw Exception( "Invalid OBJ vertex position specification" );
    }

    positions.add( Point3( x, y, z ) );
  }
  // vertex normal coords
  else if( *pos == 'n' ) {
    pos += 2;

    float x, y, z;
    if( sscanf( pos, "%f %f %f", &x, &y, &z ) != 3 ) {
      throw Exception( "Invalid OBJ vertex normal specification" );
    }

    normals.add( Vec3( x, y, z ) );
  }
  // vertex texture coords
  else if( *pos == 't' ) {
    pos += 2;

    float u, v;
    if( sscanf( pos, "%f %f", &u, &v ) != 2 ) {
      throw Exception( "Invalid OBJ vertex texture coordinate specification" );
    }

    texCoords.add( TexCoord( u, v ) );
  }
  else {
    throw Exception( "Invalid OBJ vertex specification" );
  }
}

void OBJ::readFace( char* pos, int part )
{
  char* end;

  int vertIndex, normalIndex, texCoordIndex;

  pos = skipSpaces( pos );
  end = readWord( pos );

  int wordLength = int( end - pos );
  if( wordLength <= 0 ) {
    throw Exception( "Invalid OBJ face specification" );
  }

  int firstSlash = -1;
  int lastSlash = -1;

  parts[part].faces.add();
  Face& face = parts[part].faces.last();

  // find slashes and determine whether we have vert, vert/tex, vert//norm or vert/tex/norm
  for( int i = 0; i < wordLength; ++i ) {
    if( pos[i] == '/' ) {
      if( firstSlash == -1 ) {
        firstSlash = i;
      }
      else {
        lastSlash = i;
      }
    }
  }

  // vert
  if( firstSlash == -1 ) {
    do {
      if( sscanf( pos, "%d", &vertIndex ) != 1 ) {
        throw Exception( "Invalid OBJ face specification ('v')" );
      }
      face.vertices.add( FaceVertex( vertIndex - 1, -1, -1 ) );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert/tex
  else if( lastSlash == -1 ) {
    do {
      if( sscanf( pos, "%d/%d", &vertIndex, &texCoordIndex ) != 2 ) {
        throw Exception( "Invalid OBJ face specification ('v/t')" );
      }
      face.vertices.add( FaceVertex( vertIndex - 1, -1, texCoordIndex - 1 ) );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert//norm
  else if( firstSlash + 1 == lastSlash ) {
    do {
      if( sscanf( pos, "%d//%d", &vertIndex, &normalIndex ) != 2 ) {
        throw Exception( "Invalid OBJ face specification ('v//n')" );
      }
      face.vertices.add( FaceVertex( vertIndex - 1, normalIndex - 1, -1 ) );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert/tex/norm
  else {
    do {
      if( sscanf( pos, "%d/%d/%d", &vertIndex, &texCoordIndex, &normalIndex ) != 3 ) {
        throw Exception( "Invalid OBJ face specification ('v/t/n')" );
      }
      face.vertices.add( FaceVertex( vertIndex - 1, normalIndex - 1, texCoordIndex - 1 ) );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }

  if( face.vertices.length() < 3 ) {
    throw Exception( "Invalid OBJ face, must have at least 3 vertices" );
  }
}

void OBJ::loadMaterials( const String& path )
{
  FILE* file;
  char buffer[LINE_BUFFER_SIZE];

  file = fopen( path + "/data.mtl", "r" );
  if( file == null ) {
    throw Exception( "OBJ model must have a corresponding 'data.mtl' file." );
  }

  String mtlName;
  Part   part;

  part.texture  = "";
  part.masks    = "";
  part.alpha    = 1.0f;
  part.specular = 0.0f;

  char* pos = fgets( buffer, LINE_BUFFER_SIZE, file );
  char* end;

  // until EOF reached
  while( pos != null ) {
    pos = skipSpaces( pos );

    switch( pos[0] ) {
      case 'n': {
        if( aEquals( pos, "newmtl", 6 ) ) {
          end = readWord( pos );
          pos = skipSpaces( end );
          *end = '\0';

          if( !mtlName.isEmpty() ) {
            materialIndices.add( mtlName, parts.length() );
            parts.add( part );
          }

          end = readWord( pos );
          *end = '\0';

          mtlName = pos;
          part.texture  = "";
          part.alpha    = 1.0f;
          part.specular = 0.0f;
        }
        break;
      }
      case 'K': {
        if( pos[1] == 's' ) {
          Vec4 colour;
          sscanf( pos + 2, "%f %f %f", &colour.x, &colour.y, &colour.z );
          part.specular = ( colour.x + colour.y + colour.z ) / 3.0f;
        }
        break;
      }
      case 'd': {
        sscanf( pos + 1, "%f", &part.alpha );
        break;
      }
      case 'm': {
        if( aEquals( pos, "map_Kd", 6 ) ) {
          end = readWord( pos );
          pos = skipSpaces( end );
          end = readWord( pos );
          *end = '\0';

          File texFile( path + "/" + pos );
          File masksFile( path + "/" + texFile.baseName() + "_masks." + texFile.extension() );

          if( texFile.getType() == File::MISSING ) {
            throw Exception( "OBJ texture '%s' missing", texFile.path().cstr() );
          }

          part.texture = texFile.path();

          if( masksFile.getType() != File::MISSING ) {
            part.masks = masksFile.path();
          }
        }
        break;
      }
      default: {
        break;
      }
    }
    // next line
    pos = fgets( buffer, LINE_BUFFER_SIZE, file );
  }

  if( !mtlName.isEmpty() ) {
    materialIndices.add( mtlName, parts.length() );
    parts.add( part );
  }

  fclose( file );
}

void OBJ::load()
{
  int currentMaterial = 0;

  File modelFile( path + "/data.obj" );
  File configFile( path + "/config.rc" );

  log.print( "Loading OBJ model '%s' ...", modelFile.path().cstr() );

  Config config;
  config.load( configFile );

  float  scaling     = config.get( "scale", 1.0f );
  Vec3   translation = Vec3( config.get( "translate.x", 0.0f ),
                             config.get( "translate.y", 0.0f ),
                             config.get( "translate.z", 0.0f ) );

  shader = config.get( "shader", "mesh" );

  config.clear( true );

  loadMaterials( path );

  FILE* file = fopen( modelFile.path().cstr(), "r" );
  if( file == null ) {
    throw Exception( "Cannot open OBJ data.obj file" );
  }

  DArray<char> buffer( LINE_BUFFER_SIZE );

  char* pos = fgets( buffer, LINE_BUFFER_SIZE, file );
  char* end;

  // until EOF reached
  while( pos != null ) {
    pos = skipSpaces( pos );

    switch( *pos ) {
      case 'v': {
        readVertexData( pos + 1 );
        break;
      }
      // face
      case 'f': {
        readFace( pos + 1, currentMaterial );
        break;
      }
      // usemtl
      case 'u': {
        if( aEquals( pos, "usemtl", 6 ) ) {
          pos += 6;
          pos = skipSpaces( pos );
          end = readWord( pos );
          *end = '\0';

          const int* value = materialIndices.find( pos );
          if( value != null ) {
            currentMaterial = *value;
          }
          else {
            fclose( file );
            throw Exception( "Invalid OBJ material requested: %s", &buffer[0] );
          }
        }
        break;
      }
      default: {
        break;
      }
    }
    // next line
    pos = fgets( buffer, LINE_BUFFER_SIZE, file );
  }
  fclose( file );

  if( positions.isEmpty() ) {
    throw Exception( "No vertices" );
  }
  for( int i = 0; i < positions.length(); ++i ) {
    positions[i] = Point3::ORIGIN + translation + scaling * ( positions[i] - Point3::ORIGIN );
  }

  log.printEnd( " OK" );
}

void OBJ::save()
{
  String destPath = path + ".ozcSMM";

  compiler.beginMesh();
  compiler.enable( CAP_UNIQUE );

  for( int i = 0; i < parts.length(); ++i ) {
    compiler.texture( parts[i].texture );
    compiler.masks( parts[i].masks );

    compiler.material( GL_DIFFUSE,  parts[i].alpha  );
    compiler.material( GL_SPECULAR, parts[i].specular );

    for( int j = 0; j < parts[i].faces.length(); ++j ) {
      const Face& face = parts[i].faces[j];

      compiler.begin( GL_POLYGON );

      for( int k = 0; k < face.vertices.length(); ++k ) {
        const FaceVertex& vertex = face.vertices[k];

        if( vertex.texCoord != -1 ) {
          compiler.texCoord( texCoords[vertex.texCoord].u, texCoords[vertex.texCoord].v );
        }
        compiler.normal( normals[vertex.normal] );
        compiler.vertex( positions[vertex.position] );
      }

      compiler.end();
    }
  }

  compiler.endMesh();

  MeshData mesh;
  compiler.getMeshData( &mesh );

  BufferStream os;

  os.writeString( shader );
  mesh.write( &os );

  log.print( "Writing to '%s' ...", destPath.cstr() );
  File( destPath ).write( &os );
  log.printEnd( " OK" );
}

OBJ::OBJ( const char* path_ ) : path( path_ )
{}

void OBJ::build( const char* path )
{
  log.println( "Prebuilding OBJ model '%s' {", path );
  log.indent();

  OBJ* obj = new OBJ( path );
  obj->load();
  obj->save();
  delete obj;

  log.unindent();
  log.println( "}" );
}

}
}
