/*
 *  OBJ.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OBJ.hpp"

#ifdef OZ_TOOLS

#include "client/Context.hpp"
#include "client/Colours.hpp"
#include "client/Compiler.hpp"

namespace oz
{
namespace client
{

  String            OBJ::shaderName;
  Vector<Point3>    OBJ::positions;
  Vector<Vec3>      OBJ::normals;
  Vector<TexCoord>  OBJ::texCoords;
  Vector<OBJ::Part> OBJ::parts;
  HashString<int>   OBJ::materialIndices;

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

  void OBJ::loadOBJ( const char* path )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    int currentMaterial = 0;

    String sPath = path;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.rc";

    Config config;
    config.load( configFile );

    log.print( "Loading OBJ model '%s' ...", modelFile.cstr() );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation = Vec3( config.get( "translate.x", 0.0f ),
                             config.get( "translate.y", 0.0f ),
                             config.get( "translate.z", 0.0f ) );
    shaderName = config.get( "shader", "mesh" );

    if( !loadMaterials( sPath ) ) {
      log.printEnd( " Material loading failed" );
      throw Exception( "OBJ model material loading error" );
    }

    file = fopen( modelFile.cstr(), "r" );
    if( file == null ) {
      log.printEnd( " No such file" );
      throw Exception( "OBJ model loading error" );
    }

    char* pos = fgets( buffer, LINE_BUFFER_SIZE, file );
    char* end;

    // until EOF reached
    while( pos != null ) {
      pos = skipSpaces( pos );

      switch( *pos ) {
        case 'v': {
          if( !readVertexData( pos + 1 ) ) {
            fclose( file );
            log.printEnd( " Invalid vertex line: %s", buffer );
            throw Exception( "OBJ model loading error" );
          }
          break;
        }
        // face
        case 'f': {
          if( !readFace( pos + 1, currentMaterial ) ) {
            fclose( file );
            log.printEnd( " Invalid face line: %s", buffer );
            throw Exception( "OBJ model loading error" );
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
              log.printEnd( " Invalid material requested: %s", buffer );
              throw Exception( "Invalid material requested" );
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
      log.printEnd( " No vertices" );
      throw Exception( "OBJ model loading error" );
    }
    for( int i = 0; i < positions.length(); ++i ) {
      positions[i] = Point3::ORIGIN + translation + scaling * ( positions[i] - Point3::ORIGIN );
    }

    log.printEnd( " OK" );
  }

  void OBJ::save( const char* fileName )
  {
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

    Buffer buffer( 4 * 1024 * 1024 );
    OutputStream os = buffer.outputStream();

    os.writeString( shaderName );
    mesh.write( &os );

    shaderName = "";

    log.print( "Writing to '%s' ...", fileName );
    buffer.write( fileName, os.length() );
    log.printEnd( " OK" );
  }

  void OBJ::freeOBJ()
  {
    positions.clear();
    positions.dealloc();

    normals.clear();
    normals.dealloc();

    texCoords.clear();
    texCoords.dealloc();

    parts.clear();
    parts.dealloc();

    materialIndices.clear();
    materialIndices.dealloc();
  }

  void OBJ::prebuild( const char* name )
  {
    log.println( "Prebuilding OBJ model '%s' {", name );
    log.indent();

    try {
      loadOBJ( name );
      save( String( name ) + ".ozcSMM" );
      freeOBJ();
    }
    catch( ... ) {
      log.unindent();
      log.println( "}" );
      throw;
    }

    log.unindent();
    log.println( "}" );
  }

}
}

#endif
