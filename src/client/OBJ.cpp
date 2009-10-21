/*
 *  OBJ.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "OBJ.h"

#include "Context.h"

namespace oz
{
namespace client
{

  char *OBJ::skipSpaces( char *pos )
  {
    while( *pos == ' ' || *pos == '\t' ) {
      pos++;
    }
    return pos;
  }

  char *OBJ::readWord( char *pos )
  {
    while( *pos != ' ' && *pos != '\t' && *pos != '\n' && *pos != EOF ) {
      pos++;
    }
    return pos;
  }

  bool OBJ::readVertexData( char *pos,
                            Vector<Vec3> *tempVerts,
                            Vector<Vec3> *tempNormals,
                            Vector<TexCoord> *tempTexCoords ) const
  {
    // pos should be at position just after 'v'

    // vertex coords
    if( *pos == ' ' ) {
      pos++;

      float x, y, z;
      int nMatches = sscanf( pos, "%f %f %f", &x, &y, &z );

      if( nMatches != 3 ) {
        return false;
      }
      *tempVerts << Vec3( x, y, z );
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
      *tempNormals << Vec3( x, y, z );
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
      *tempTexCoords << TexCoord( u, v );
      return true;
    }
    return false;
  }

  bool OBJ::readFace( char *pos, Face *face ) const
  {
    char *end;

    Vector<uint> vertIndices;
    Vector<uint> normalIndices;
    Vector<uint> texCoordIndices;

    int vertIndex, normalIndex, texCoordIndex;

    pos = skipSpaces( pos );
    end = readWord( pos );

    int wordLength = end - pos;
    if( wordLength <= 0 ) {
      return false;
    }

    int firstSlash = -1;
    int lastSlash = -1;

    // find slashes and determine whether we have vert, vert/tex, vert//norm or vert/tex/norm
    for( int i = 0; i < wordLength; i++ ) {
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
        vertIndices << vertIndex - 1;

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
        vertIndices << vertIndex - 1;
        texCoordIndices << texCoordIndex - 1;

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
        vertIndices << vertIndex - 1;
        normalIndices << normalIndex - 1;

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
        vertIndices << vertIndex - 1;
        normalIndices << normalIndex - 1;
        texCoordIndices << texCoordIndex - 1;

        pos = skipSpaces( end );
        end = readWord( pos );
      }
      while( end - pos > 0 );
    }

    face->nVerts = vertIndices.length();

    if( face->nVerts < 3 ) {
      return false;
    }
    if( normalIndices.length() != 0 && normalIndices.length() != face->nVerts ) {
      return false;
    }
    if( texCoordIndices.length() != 0 && texCoordIndices.length() != face->nVerts ) {
      return false;
    }

    face->vertIndices = new uint[face->nVerts];
    aCopy<uint>( face->vertIndices, vertIndices, face->nVerts );

    if( !normalIndices.isEmpty() ) {
      face->normIndices = new uint[face->nVerts];
      aCopy<uint>( face->normIndices, normalIndices, face->nVerts );
    }
    else {
      face->normIndices = null;
    }

    if( !texCoordIndices.isEmpty() ) {
      face->texCoordIndices = new uint[face->nVerts];
      aCopy<uint>( face->texCoordIndices, texCoordIndices, face->nVerts );
    }
    else {
      face->texCoordIndices = null;
    }

    return true;
  }

  bool OBJ::loadMaterial( const String &path )
  {
    FILE *file;
    char buffer[LINE_BUFFER_SIZE];

    file = fopen( path + "/data.mtl", "r" );
    if( file == null ) {
      return false;
    }

    char *pos = fgets( buffer, LINE_BUFFER_SIZE, file );
    char *end;

    // until EOF reached
    while( pos != null ) {
      pos = skipSpaces( pos );

      switch( *pos ) {
        case 'm': {
          if( aEquals( pos, "map_Kd", 6 ) ) {
            end = readWord( pos );
            pos = skipSpaces( end );
            end = readWord( pos );
            *end = '\0';

            textureId = context.loadTexture( path + "/" + String( pos ), true );
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
    return true;
  }

  OBJ::OBJ( const char *name_ )
  {
    FILE *file;
    char buffer[LINE_BUFFER_SIZE];

    name = name_;

    // default texture if none loaded
    textureId = 0;

    String sPath = "mdl/" + name;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.xml";

    Config config;
    config.load( configFile );

    log.print( "Loading OBJ model '%s' ...", modelFile.cstr() );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation( config.get( "translate.x", 0.0f ),
                      config.get( "translate.y", 0.0f ),
                      config.get( "translate.z", 0.0f ) );
    config.clear();

    file = fopen( modelFile.cstr(), "r" );
    if( file == null ) {
      log.printEnd( "No such file" );
      throw Exception( 0, "OBJ model loading error" );
    }

    Vector<Vec3>     tempVerts;
    Vector<Vec3>     tempNormals;
    Vector<TexCoord> tempTexCoords;
    Vector<Face>     tempFaces;

    char *pos = fgets( buffer, LINE_BUFFER_SIZE, file );

    // until EOF reached
    while( pos != null ) {
      pos = skipSpaces( pos );

      switch( *pos ) {
        case 'v': {
          if( !readVertexData( pos + 1, &tempVerts, &tempNormals, &tempTexCoords ) ) {
            fclose( file );
            log.println( "invalid vertex line: %s", buffer );
            throw Exception( 0, "OBJ model loading error" );
          }
          break;
        }
        // face
        case 'f': {
          Face face;

          if( !readFace( pos + 1, &face ) ) {
            fclose( file );
            log.println( "invalid face line: %s", buffer );
            throw Exception( 0, "OBJ model loading error" );
          }
          tempFaces << face;
          break;
        }
        // usemtl
        case 'm': {
          if( aEquals( pos, "mtllib", 6 ) && !loadMaterial( sPath ) ) {
            fclose( file );
            log.println( "cannot load material at line: %s", buffer );
            throw Exception( 0, "OBJ model loading error" );
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

    // copy everything into arrays for memory optimization
    if( !tempVerts.isEmpty() ) {
      vertices( tempVerts.length() );
      for( int i = 0; i < vertices.length(); i++ ) {
        vertices[i] = translation + scaling * tempVerts[i];
      }
    }
    else {
      throw Exception( 0, "OBJ model loading error" );
    }

    if( !tempNormals.isEmpty() ) {
      normals( tempNormals.length() );
      aCopy<Vec3>( normals, tempNormals, normals.length() );
    }

    if( !tempTexCoords.isEmpty() ) {
      texCoords( tempTexCoords.length() );
      aCopy<TexCoord>( texCoords, tempTexCoords, texCoords.length() );
    }

    if( !tempFaces.isEmpty() ) {
      faces( tempFaces.length() );
      // we don't copy arrays, pointers in both containers point to the same data
      aCopy<Face>( faces, tempFaces, faces.length() );
    }
    else {
      throw Exception( 0, "OBJ model loading error" );
    }
    log.printEnd( " OK" );
  }

  OBJ::~OBJ()
  {
    log.print( "Unloading OBJ model '%s' ...", name.cstr() );
    trim();
    context.freeTexture( textureId );
    log.printEnd( " OK" );

    assert( glGetError() == GL_NO_ERROR );
  }

  void OBJ::scale( float scale )
  {
    for( int i = 0; i < vertices.length(); i++ ) {
      vertices[i] *= scale;
    }
  }

  void OBJ::translate( const Vec3 &t )
  {
    for( int i = 0; i < vertices.length(); i++ ) {
      vertices[i] += t;
    }
  }

  void OBJ::draw() const
  {
    glBindTexture( GL_TEXTURE_2D, textureId );

    for( int i = 0; i < faces.length(); i++ ) {
      const Face &face = faces[i];

      glBegin( GL_POLYGON );
        for( int j = 0; j < face.nVerts; j++ ) {
          if( !texCoords.isEmpty() ) {
            glTexCoord2fv( (float*) &texCoords[face.texCoordIndices[j]] );
          }
          if( !normals.isEmpty() ) {
            glNormal3fv( normals[face.normIndices[j]] );
          }
          glVertex3fv( vertices[face.vertIndices[j]] );
        }
      glEnd();
    }
  }

  void OBJ::genList()
  {
    list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
      draw();
    glEndList();
  }

  void OBJ::trim()
  {
    for( int i = 0; i < faces.length(); i++ ) {
      delete[] faces[i].vertIndices;

      if( faces[i].normIndices != null ) {
        delete[] faces[i].normIndices;
      }
      if( faces[i].texCoordIndices != null ) {
        delete[] faces[i].texCoordIndices;
      }
    }

    vertices.clear();
    normals.clear();
    texCoords.clear();
    faces.clear();
  }

}
}
