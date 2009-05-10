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
      *tempVerts << Vec3( -x, -y, -z );
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

    assert( face->nVerts > 0 );

    face->vertIndices = new uint[face->nVerts];
    aCopy( face->vertIndices, vertIndices.dataPtr(), face->nVerts );

    if( !normalIndices.isEmpty() ) {
      face->normIndices = new uint[face->nVerts];
      aCopy( face->normIndices, normalIndices.dataPtr(), face->nVerts );
    }
    else {
      face->normIndices = null;
    }

    if( !texCoordIndices.isEmpty() ) {
      face->texCoordIndices = new uint[face->nVerts];
      aCopy( face->texCoordIndices, texCoordIndices.dataPtr(), face->nVerts );
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

  OBJ::OBJ() : nVertices( 0 ), vertices( null ), nNormals( 0 ), normals( null ),
      nTexCoords( 0 ), texCoords( null ), nFaces( 0 ), faces( null )
  {}

  OBJ::~OBJ()
  {
    free();
  }

  bool OBJ::load( const char *path )
  {
    FILE *file;
    char buffer[LINE_BUFFER_SIZE];

    // default texture if none loaded
    textureId = 0;

    String sPath = path;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.xml";

    Config config;
    config.load( configFile );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation( config.get( "translate.x", 0.0f ),
                      config.get( "translate.y", 0.0f ),
                      config.get( "translate.z", 0.0f ) );
    config.clear();

    file = fopen( modelFile.cstr(), "r" );
    if( file == null ) {
      return false;
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
            logFile.println( "invalid vertex line: %s", buffer );
            return false;
          }
          break;
        }
        // face
        case 'f': {
          Face face;

          if( !readFace( pos + 1, &face ) ) {
            fclose( file );
            logFile.println( "invalid face line: %s", buffer );
            return false;
          }
          tempFaces << face;
          break;
        }
        // usemtl
        case 'm': {
          if( aEquals( pos, "mtllib", 6 ) && !loadMaterial( sPath ) ) {
            fclose( file );
            logFile.println( "cannot load material at line: %s", buffer );
            return false;
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

    nVertices = tempVerts.length();
    if( nVertices > 0 ) {
      vertices = new Vec3[nVertices];

      for( int i = 0; i < nVertices; i++ ) {
        vertices[i] = translation + scaling * tempVerts[i];
      }
    }
    else {
      nVertices = 0;
      return false;
    }

    nNormals = tempNormals.length();
    if( nNormals > 0 ) {
      normals = new Vec3[nNormals];
      aCopy( normals, tempNormals.dataPtr(), nNormals );
    }

    nTexCoords = tempTexCoords.length();
    if( nTexCoords > 0 ) {
      texCoords = new TexCoord[nTexCoords];
      aCopy( texCoords, tempTexCoords.dataPtr(), nTexCoords );
    }

    nFaces = tempFaces.length();
    if( nFaces > 0 ) {
      faces = new Face[nFaces];
      // we don't copy arrays, pointers in both containers point to the same data
      aCopy( faces, tempFaces.dataPtr(), nFaces );
    }
    else {
      return false;
    }

    return true;
  }

  void OBJ::free()
  {
    for( int i = 0; i < nFaces; i++ ) {
      delete[] faces[i].vertIndices;

      if( faces[i].normIndices != null ) {
        delete[] faces[i].normIndices;
      }
      if( faces[i].texCoordIndices != null ) {
        delete[] faces[i].texCoordIndices;
      }
    }
    if( nFaces > 0 ) {
      nFaces = 0;
      delete[] faces;
      faces = null;
    }
    if( nTexCoords > 0 ) {
      nTexCoords = 0;
      delete[] texCoords;
      texCoords = null;
    }
    if( nNormals > 0 ) {
      nNormals = 0;
      delete[] normals;
      normals = null;
    }
    if( nVertices > 0 ) {
      nVertices = 0;
      delete[] vertices;
      vertices = null;
    }
  }

  void OBJ::draw() const
  {
    glBindTexture( GL_TEXTURE_2D, textureId );

    for( int i = 0; i < nFaces; i++ ) {
      Face &face = faces[i];

//       // draw polygon as a triangle strip
//       glBegin( GL_TRIANGLE_STRIP );
//         assert( face.nVerts >= 3 );
//
//         int outerMiddle = face.nVerts / 2 + 1;
//         int innerMiddle = ( face.nVerts - 1 ) / 2;
//         for( int j = 0; j < outerMiddle; j++ ) {
//
//           if( texCoords ) {
//             glTexCoord2fv( (float*) &texCoords[face.texCoordIndices[j]] );
//           }
//           if( normals ) {
//             glNormal3fv( normals[face.normIndices[j]] );
//           }
//           glVertex3fv( vertices[face.vertIndices[j]] );
//
//           if( j == 0 || j > innerMiddle ) {
//             continue;
//           }
//
//           if( texCoords ) {
//             glTexCoord2fv( (float*) &texCoords[face.texCoordIndices[face.nVerts - j]] );
//           }
//           if( normals ) {
//             glNormal3fv( normals[face.normIndices[face.nVerts - j]] );
//           }
//           glVertex3fv( vertices[face.vertIndices[face.nVerts - j]] );
//         }
//       glEnd();

      glBegin( GL_POLYGON );
        for( int j = 0; j < face.nVerts; j++ ) {
          if( texCoords ) {
            glTexCoord2fv( (float*) &texCoords[face.texCoordIndices[j]] );
          }
          if( normals ) {
            glNormal3fv( normals[face.normIndices[j]] );
          }
          glVertex3fv( vertices[face.vertIndices[j]] );
        }
      glEnd();
    }
  }

  uint OBJ::genList( const char *file )
  {
    OBJ obj;

    obj.load( file );

    uint list = context.genList();

    glNewList( list, GL_COMPILE );
      obj.draw();
    glEndList();

    return list;
  }

  void OBJ::scale( float scale )
  {
    for( int i = 0; i < nVertices; i++ ) {
      vertices[i] *= scale;
    }
  }

  void OBJ::translate( const Vec3 &t )
  {
    for( int i = 0; i < nVertices; i++ ) {
      vertices[i] += t;
    }
  }

}
}
