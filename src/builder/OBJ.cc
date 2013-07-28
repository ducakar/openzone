/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file builder/OBJ.cc
 */

#include <builder/OBJ.hh>

#include <client/Context.hh>
#include <builder/Compiler.hh>

#include <sstream>

namespace oz
{
namespace builder
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

void OBJ::readVertexData( const char* pos )
{
  // currently pos points to the char just after 'v'
  const char* beginPos = pos;

  // vertex coords
  if( *pos == ' ' ) {
    ++pos;

    std::istringstream ss( pos );
    float x, y, z;
    ss >> x >> y >> z;

    if( ss.fail() ) {
      OZ_ERROR( "Invalid OBJ vertex position specification: %s", beginPos );
    }

    positions.add( Point( x, y, z ) );
  }
  // vertex normal coords
  else if( *pos == 'n' ) {
    pos += 2;

    std::istringstream ss( pos );
    float x, y, z;
    ss >> x >> y >> z;

    if( ss.fail() ) {
      OZ_ERROR( "Invalid OBJ vertex normal specification: %s", beginPos );
    }

    normals.add( Vec3( x, y, z ) );
  }
  // vertex texture coords
  else if( *pos == 't' ) {
    pos += 2;

    float u, v;
    std::istringstream ss( pos );
    ss >> u >> v;

    if( ss.fail() ) {
      OZ_ERROR( "Invalid OBJ vertex texture coordinate specification: %s", beginPos );
    }

    texCoords.add( TexCoord( u, v ) );
  }
  else {
    OZ_ERROR( "Invalid OBJ vertex specification: %s", beginPos );
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
    OZ_ERROR( "Invalid OBJ face specification" );
  }

  int firstSlash = -1;
  int lastSlash = -1;

  parts[part].faces.add();
  Face& face = parts[part].faces.last();

  // find slashes and determine whether we have vert, vert/tex, vert//norm or vert/tex/norm
  for( int i = 0; i < wordLength; ++i ) {
    if( pos[i] == '/' ) {
      if( firstSlash < 0 ) {
        firstSlash = i;
      }
      else {
        lastSlash = i;
      }
    }
  }

  // vert
  if( firstSlash < 0 ) {
    do {
      if( sscanf( pos, "%5d", &vertIndex ) != 1 ) {
        OZ_ERROR( "Invalid OBJ face specification ('v')" );
      }
      face.vertices.add( { vertIndex - 1, -1, -1 } );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert/tex
  else if( lastSlash < 0 ) {
    do {
      if( sscanf( pos, "%5d/%5d", &vertIndex, &texCoordIndex ) != 2 ) {
        OZ_ERROR( "Invalid OBJ face specification ('v/t')" );
      }
      face.vertices.add( { vertIndex - 1, -1, texCoordIndex - 1 } );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert//norm
  else if( firstSlash + 1 == lastSlash ) {
    do {
      if( sscanf( pos, "%5d//%5d", &vertIndex, &normalIndex ) != 2 ) {
        OZ_ERROR( "Invalid OBJ face specification ('v//n')" );
      }
      face.vertices.add( { vertIndex - 1, normalIndex - 1, -1 } );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }
  // vert/tex/norm
  else {
    do {
      if( sscanf( pos, "%5d/%5d/%5d", &vertIndex, &texCoordIndex, &normalIndex ) != 3 ) {
        OZ_ERROR( "Invalid OBJ face specification ('v/t/n')" );
      }
      face.vertices.add( { vertIndex - 1, normalIndex - 1, texCoordIndex - 1 } );

      pos = skipSpaces( end );
      end = readWord( pos );
    }
    while( end - pos > 0 );
  }

  if( face.vertices.length() < 3 ) {
    OZ_ERROR( "Invalid OBJ face, must have at least 3 vertices" );
  }
}

void OBJ::loadMaterials( const char* filePath )
{
  char buffer[LINE_BUFFER_SIZE];

  FILE* fs = fopen( filePath, "r" );
  if( fs == nullptr ) {
    OZ_ERROR( "OBJ model must have a corresponding 'data.mtl' file." );
  }

  String mtlName;
  Part   part;

  part.texture  = "";
  part.masks    = "";
  part.alpha    = 1.0f;

  char* pos = fgets( buffer, LINE_BUFFER_SIZE, fs );
  char* end;

  // until EOF reached
  while( pos != nullptr ) {
    pos = skipSpaces( pos );

    switch( pos[0] ) {
      case 'n': {
        if( aEquals( pos, 6, "newmtl" ) ) {
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
        }
        break;
      }
      case 'd': {
        sscanf( pos + 1, "%17f", &part.alpha );
        break;
      }
      case 'm': {
        if( aEquals( pos, 6, "map_Kd" ) ) {
          end = readWord( pos );
          pos = skipSpaces( end );
          end = readWord( pos );
          *end = '\0';

          int dot = String::index( pos, '.' );
          String name = dot < 0 ? String( pos ) : String( pos, dot );

          part.texture = name;
        }
        break;
      }
      default: {
        break;
      }
    }
    // next line
    pos = fgets( buffer, LINE_BUFFER_SIZE, fs );
  }

  if( !mtlName.isEmpty() ) {
    materialIndices.add( mtlName, parts.length() );
    parts.add( part );
  }

  fclose( fs );
}

void OBJ::load()
{
  Log::print( "Loading OBJ model '%s' ...", path.cstr() );

  File modelFile( path + "/data.obj" );
  File configFile( path + "/config.json" );

  int currentMaterial = 0;

  JSON config( configFile );

  float scale       = config["scale"].get( 1.0f );
  Vec3  translation = config["translate"].get( Vec3::ZERO );

  shader = config["shader"].get( "mesh" );

  config.clear( true );

  String realPath = modelFile.realDirectory() + "/" + &path[1];

  loadMaterials( realPath + "/data.mtl" );

  FILE* fs = fopen( realPath + "/data.obj" , "r" );
  if( fs == nullptr ) {
    OZ_ERROR( "Cannot open OBJ data.obj file" );
  }

  DArray<char> buffer( LINE_BUFFER_SIZE );

  char* pos = fgets( buffer.begin(), LINE_BUFFER_SIZE, fs );
  char* end;

  // until EOF reached
  while( pos != nullptr ) {
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
        if( aEquals( pos, 6, "usemtl" ) ) {
          pos += 6;
          pos = skipSpaces( pos );
          end = readWord( pos );
          *end = '\0';

          const int* value = materialIndices.find( pos );
          if( value != nullptr ) {
            currentMaterial = *value;
          }
          else {
            OZ_ERROR( "Invalid OBJ material requested: %s", &buffer[0] );
          }
        }
        break;
      }
      default: {
        break;
      }
    }
    // next line
    pos = fgets( buffer.begin(), LINE_BUFFER_SIZE, fs );
  }

  fclose( fs );

  if( positions.isEmpty() ) {
    OZ_ERROR( "No vertices" );
  }
  for( int i = 0; i < positions.length(); ++i ) {
    positions[i] = Point::ORIGIN + translation + scale * ( positions[i] - Point::ORIGIN );
  }

  Log::printEnd( " OK" );
}

void OBJ::save()
{
  String sDestDir = &path[1];

  File::mkdir( sDestDir );
  File destFile( sDestDir + "/data.ozcSMM" );

  compiler.beginMesh();
  compiler.enable( Compiler::UNIQUE );

  for( int i = 0; i < parts.length(); ++i ) {
    compiler.texture( path + "/" + parts[i].texture );
    compiler.blend( parts[i].alpha != 1.0f );

    for( int j = 0; j < parts[i].faces.length(); ++j ) {
      const Face& face = parts[i].faces[j];

      compiler.begin( Compiler::POLYGON );

      for( int k = 0; k < face.vertices.length(); ++k ) {
        const FaceVertex& vertex = face.vertices[k];

        if( vertex.texCoord >= 0 ) {
          compiler.texCoord( texCoords[vertex.texCoord].u, 1.0f - texCoords[vertex.texCoord].v );
        }
        compiler.normal( normals[vertex.normal] );
        compiler.vertex( positions[vertex.position] );
      }

      compiler.end();
    }
  }

  compiler.endMesh();

  OutputStream os( 0, Endian::LITTLE );

  compiler.writeMesh( &os );
  compiler.buildMeshTextures( sDestDir );

  Log::print( "Writing to '%s' ...", destFile.path().cstr() );

  if( !destFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

void OBJ::build( const char* path_ )
{
  Log::println( "Prebuilding OBJ model '%s' {", path_ );
  Log::indent();

  path = path_;

  load();
  save();

  path   = "";
  shader = "";

  positions.clear();
  positions.deallocate();
  normals.clear();
  normals.deallocate();
  texCoords.clear();
  texCoords.deallocate();
  parts.clear();
  parts.deallocate();
  materialIndices.clear();
  materialIndices.deallocate();

  Log::unindent();
  Log::println( "}" );
}

OBJ obj;

}
}
