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
 * @file build/OBJ.cpp
 */

#include "stable.hpp"

#include "build/OBJ.hpp"

#include "client/Context.hpp"
#include "client/OpenGL.hpp"

#include "build/Compiler.hpp"

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

bool OBJ::readVertexData( char* pos )
{
  // pos should point to char just after 'v'

  // vertex coords
  if( *pos == ' ' ) {
    ++pos;

    float x, y, z;
    int nMatches = sscanf( pos, "%f %f %f", &x, &y, &z );

    if( nMatches != 3 ) {
      return false;
    }
    positions.add( Point3( x, y, z ) );
    return true;
  }
  // vertex normal coords
  else if( *pos == 'n' ) {
    pos += 2;

    float x, y, z;
    int nMatches = sscanf( pos, "%f %f %f", &x, &y, &z );

    if( nMatches != 3 ) {
      return false;
    }
    normals.add( Vec3( x, y, z ) );
    return true;
  }
  // vertex texture coords
  else if( *pos == 't' ) {
    pos += 2;

    float u, v;
    int nMatches = sscanf( pos, "%f %f", &u, &v );

    if( nMatches != 2 ) {
      return false;
    }
    texCoords.add( TexCoord( u, v ) );
    return true;
  }
  return false;
}

bool OBJ::readFace( char* pos, int part )
{
  char* end;

  int vertIndex, normalIndex, texCoordIndex;

  pos = skipSpaces( pos );
  end = readWord( pos );

  int wordLength = int( end - pos );
  if( wordLength <= 0 ) {
    return false;
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
        return false;
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
        return false;
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
        return false;
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
        return false;
      }
      face.vertices.add( FaceVertex( vertIndex - 1, normalIndex - 1, texCoordIndex - 1 ) );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }

  if( face.vertices.length() < 3 ) {
    return false;
  }

  return true;
}

bool OBJ::loadMaterials( const String& path )
{
  FILE* file;
  char buffer[LINE_BUFFER_SIZE];

  file = fopen( path + "/data.mtl", "r" );
  if( file == null ) {
    return false;
  }

  String mtlName;
  Part   part;

  part.texture  = "";
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

          part.texture = path + "/" + String( pos );
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
  return true;
}

void OBJ::load()
{
  int currentMaterial = 0;

  String modelFile = path + "/data.obj";
  String configFile = path + "/config.rc";

  log.print( "Loading OBJ model '%s' ...", modelFile.cstr() );

  Config config;
  config.load( configFile );

  float  scaling     = config.get( "scale", 1.0f );
  Vec3   translation = Vec3( config.get( "translate.x", 0.0f ),
                             config.get( "translate.y", 0.0f ),
                             config.get( "translate.z", 0.0f ) );

  shader = config.get( "shader", "mesh" );

  config.clear( true );

  if( !loadMaterials( path ) ) {
    log.printEnd( " Material loading failed" );
    throw Exception( "OBJ model material loading error" );
  }

  FILE* file = fopen( modelFile.cstr(), "r" );
  if( file == null ) {
    log.printEnd( " No such file" );
    throw Exception( "OBJ model loading error" );
  }

  DArray<char> buffer( LINE_BUFFER_SIZE );

  char* pos = fgets( buffer, LINE_BUFFER_SIZE, file );
  char* end;

  // until EOF reached
  while( pos != null ) {
    pos = skipSpaces( pos );

    switch( *pos ) {
      case 'v': {
        if( !readVertexData( pos + 1 ) ) {
          fclose( file );
          throw Exception( "Invalid OBJ vertex line: %s", &buffer[0] );
        }
        break;
      }
      // face
      case 'f': {
        if( !readFace( pos + 1, currentMaterial ) ) {
          fclose( file );
          throw Exception( "Invalid OBJ face line: %s", &buffer[0] );
        }
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
