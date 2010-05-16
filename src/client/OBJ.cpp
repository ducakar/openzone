/*
 *  OBJ.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "client/OBJ.h"

#include "client/Context.h"
#include "client/Colors.h"

namespace oz
{
namespace client
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

  bool OBJ::readVertexData( char* pos,
                            Vector<Vec3>* tempVerts,
                            Vector<Vec3>* tempNormals,
                            Vector<TexCoord>* tempTexCoords ) const
  {
    // pos should be at position just after 'v'

    // vertex coords
    if( *pos == ' ' ) {
      ++pos;

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

  bool OBJ::readFace( char* pos, Face* face ) const
  {
    char* end;

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

  bool OBJ::loadMaterial( const String& path, HashString<int, 32>* materialIndices )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    file = fopen( path + "/data.mtl", "r" );
    if( file == null ) {
      return false;
    }

    String   mtlName;
    Material material;
    Vector<Material> tempMaterials;

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
              materialIndices->add( mtlName, tempMaterials.length() );
              tempMaterials << material;
            }

            end = readWord( pos );
            *end = '\0';

            mtlName = pos;
            material.ambient  = Vec3::zero();
            material.diffuse  = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
            material.specular = Vec3( 0.5f, 0.5f, 0.5f );
            material.texId    = 0;
          }
          break;
        }
        case 'K': {
          if( pos[1] == 'a' ) {
            sscanf( pos + 2, "%f %f %f", &material.ambient.x, &material.ambient.y, &material.ambient.z );
          }
          else if( pos[1] == 'd' ) {
            sscanf( pos + 2, "%f %f %f", &material.diffuse.x, &material.diffuse.y, &material.diffuse.z );
          }
          else if( pos[1] == 's' ) {
            sscanf( pos + 2, "%f %f %f", &material.specular.x, &material.specular.y, &material.specular.z );
          }
          break;
        }
        case 'd': {
          sscanf( pos + 1, "%f", &material.diffuse.w );
          break;
        }
        case 'm': {
          if( aEquals( pos, "map_Kd", 6 ) ) {
            end = readWord( pos );
            pos = skipSpaces( end );
            end = readWord( pos );
            *end = '\0';

            material.texId = context.loadTexture( path + "/" + String( pos ), true );
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
      materialIndices->add( mtlName, tempMaterials.length() );
      tempMaterials << material;
    }

    materials( tempMaterials.length() );
    iCopy( materials.iter(), tempMaterials.citer() );

    fclose( file );
    return true;
  }

  OBJ::OBJ( const char* name_ )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    name = name_;

    String sPath = "mdl/" + name;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.rc";

    Config config;
    config.load( configFile );

    log.println( "Loading OBJ model '%s' {", modelFile.cstr() );
    log.indent();

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation( config.get( "translate.x", 0.0f ),
                      config.get( "translate.y", 0.0f ),
                      config.get( "translate.z", 0.0f ) );

    HashString<int, 32> materialIndices;

    if( !loadMaterial( sPath, &materialIndices ) ) {
      throw Exception( "OBJ model material loading error" );
    }

    file = fopen( modelFile.cstr(), "r" );
    if( file == null ) {
      log.println( "No such file" );
      log.unindent();
      log.println( "}" );
      throw Exception( "OBJ model loading error" );
    }

    Vector<Vec3>     tempVerts;
    Vector<Vec3>     tempNormals;
    Vector<TexCoord> tempTexCoords;
    Vector<Face>     tempFaces;

    char* pos = fgets( buffer, LINE_BUFFER_SIZE, file );
    char* end;

    // until EOF reached
    while( pos != null ) {
      pos = skipSpaces( pos );

      switch( *pos ) {
        case 'v': {
          if( !readVertexData( pos + 1, &tempVerts, &tempNormals, &tempTexCoords ) ) {
            fclose( file );
            log.println( "invalid vertex line: %s", buffer );
            log.unindent();
            log.println( "}" );
            throw Exception( "OBJ model loading error" );
          }
          break;
        }
        // face
        case 'f': {
          Face face;

          if( !readFace( pos + 1, &face ) ) {
            fclose( file );
            log.println( "invalid face line: %s", buffer );
            log.unindent();
            log.println( "}" );
            throw Exception( "OBJ model loading error" );
          }
          tempFaces << face;
          break;
        }
        // usemtl
        case 'u': {
          if( aEquals( pos, "usemtl", 6 ) ) {
            pos += 6;
            pos = skipSpaces( pos );
            end = readWord( pos );
            *end = '\0';

            Face materialFace;
            const int* value = materialIndices.find( pos );
            if( value != null ) {
              materialFace.nVerts = ~*value;
            }
            else {
              materialFace.nVerts = 0;
            }
            tempFaces << materialFace;
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
      for( int i = 0; i < vertices.length(); ++i ) {
        vertices[i] = translation + scaling * tempVerts[i];
      }
    }
    else {
      throw Exception( "OBJ model loading error" );
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
      log.println( "no faces" );
      log.unindent();
      log.println( "}" );
      throw Exception( "OBJ model loading error" );
    }

    materialIndices.clear();

    log.unindent();
    log.println( "}" );
  }

  OBJ::~OBJ()
  {
    log.print( "Unloading OBJ model '%s' ...", name.cstr() );
    trim();

    foreach( material, materials.citer() ) {
      if( material->texId != GL_NONE ) {
        context.freeTexture( material->texId );
      }
    }
    materials.clear();

    log.printEnd( " OK" );

    assert( glGetError() == GL_NO_ERROR );
  }

  void OBJ::scale( float scale )
  {
    for( int i = 0; i < vertices.length(); ++i ) {
      vertices[i] *= scale;
    }
  }

  void OBJ::translate( const Vec3& t )
  {
    for( int i = 0; i < vertices.length(); ++i ) {
      vertices[i] += t;
    }
  }

  void OBJ::draw() const
  {
    int  currentMaterial = -1;
    bool isTransfluent = false;
    bool isTextured = true;

    for( int i = 0; i < faces.length(); ++i ) {
      const Face& face = faces[i];

      if( face.nVerts < 0 ) {
        if( currentMaterial != ~face.nVerts ) {
          currentMaterial = ~face.nVerts;
          const Material& material = materials[currentMaterial];

          if( !isTransfluent && material.diffuse.w != 1.0f ) {
            glEnable( GL_BLEND );
            isTransfluent = true;
          }
          else if( isTransfluent && material.diffuse.w == 1.0f ) {
            glDisable( GL_BLEND );
            isTransfluent = false;
          }
          if( isTextured && material.texId == 0 ) {
            glDisable( GL_TEXTURE_2D );
            isTextured = false;
          }
          else if( !isTextured && material.texId != 0 ) {
            glEnable( GL_TEXTURE_2D );
            isTextured = true;
          }
          if( material.texId != 0 ) {
            glBindTexture( GL_TEXTURE_2D, material.texId );
          }
          glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material.diffuse );
        }
      }

      glBegin( GL_POLYGON );
        for( int j = 0; j < face.nVerts; ++j ) {
          if( !texCoords.isEmpty() ) {
            glTexCoord2fv( &texCoords[face.texCoordIndices[j]].u );
          }
          if( !normals.isEmpty() ) {
            glNormal3fv( normals[face.normIndices[j]] );
          }
          glVertex3fv( vertices[face.vertIndices[j]] );
        }
      glEnd();
    }

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colors::WHITE );

    if( !isTextured ) {
      glEnable( GL_TEXTURE_2D );
    }
    if( isTransfluent ) {
      glDisable( GL_BLEND );
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
    for( int i = 0; i < faces.length(); ++i ) {
      if( faces[i].nVerts > 0 ) {
        delete[] faces[i].vertIndices;

        if( faces[i].normIndices != null ) {
          delete[] faces[i].normIndices;
        }
        if( faces[i].texCoordIndices != null ) {
          delete[] faces[i].texCoordIndices;
        }
      }
    }

    vertices.clear();
    normals.clear();
    texCoords.clear();
    faces.clear();
  }

}
}
