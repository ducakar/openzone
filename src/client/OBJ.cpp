/*
 *  OBJ.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OBJ.hpp"

#include "client/Context.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{

  Vector<Vec3>          OBJ::positions;
  Vector<Vec3>          OBJ::normals;
  Vector<TexCoord>      OBJ::texCoords;
  Vector<OBJ::Face>     OBJ::faces;
  Vector<OBJ::Material> OBJ::materials;
  HashString<int>       OBJ::materialIndices;

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

  bool OBJ::readVertexData( char* pos ) const
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
      positions.add( Vec3( x, y, z ) );
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

  bool OBJ::readFace( char* pos, Face* face ) const
  {
    char* end;

    int iVertex, iNormal, iTexCoord;

    pos = skipSpaces( pos );
    end = readWord( pos );

    int wordLength = int( end - pos );
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
        if( sscanf( pos, "%d", &iVertex ) != 1 ) {
          return false;
        }
        face->vertices.add( Face::Vertex( iVertex - 1, -1, -1 ) );

        pos = skipSpaces( end );
        end = readWord( pos );
      }
      while( end - pos > 0 );
    }
    // vert/tex
    else if( lastSlash == -1 ) {
      do {
        if( sscanf( pos, "%d/%d", &iVertex, &iTexCoord ) != 2 ) {
          return false;
        }
        face->vertices.add( Face::Vertex( iVertex - 1, -1, iTexCoord - 1 ) );

        pos = skipSpaces( end );
        end = readWord( pos );
      }
      while( end - pos > 0 );
    }
    // vert//norm
    else if( firstSlash + 1 == lastSlash ) {
      do {
        if( sscanf( pos, "%d//%d", &iVertex, &iNormal ) != 2 ) {
          return false;
        }
        face->vertices.add( Face::Vertex( iVertex - 1, iNormal - 1, -1 ) );

        pos = skipSpaces( end );
        end = readWord( pos );
      }
      while( end - pos > 0 );
    }
    // vert/tex/norm
    else {
      do {
        if( sscanf( pos, "%d/%d/%d", &iVertex, &iTexCoord, &iNormal ) != 3 ) {
          return false;
        }
        face->vertices.add( Face::Vertex( iVertex - 1, iNormal - 1, iTexCoord - 1 ) );

        pos = skipSpaces( end );
        end = readWord( pos );
      }
      while( end - pos > 0 );
    }

    if( face->vertices.length() < 3 ) {
      return false;
    }

    return true;
  }

  bool OBJ::loadMaterial( const String& path )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    file = fopen( path + "/data.mtl", "r" );
    if( file == null ) {
      return false;
    }

    String   mtlName;
    Material material;

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
              materialIndices.add( mtlName, materials.length() );
              materials.add( material );
            }

            end = readWord( pos );
            *end = '\0';

            mtlName = pos;
            material.ambient  = Vec3::ZERO;
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
      materialIndices.add( mtlName, materials.length() );
      materials.add( material );
    }

    fclose( file );
    return true;
  }

  OBJ::OBJ( const char* name_ )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    int iCurrentMaterial = 0;

    name = name_;

    String sPath = "mdl/" + name;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.rc";

    Config config;
    config.load( configFile );

    log.println( "Loading OBJ model '%s' {", modelFile.cstr() );
    log.indent();

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation = Vec3( config.get( "translate.x", 0.0f ),
                             config.get( "translate.y", 0.0f ),
                             config.get( "translate.z", 0.0f ) );

    if( !loadMaterial( sPath ) ) {
      throw Exception( "OBJ model material loading error" );
    }

    file = fopen( modelFile.cstr(), "r" );
    if( file == null ) {
      log.println( "No such file" );
      log.unindent();
      log.println( "}" );
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
          face.iMaterial = iCurrentMaterial;

          if( !readFace( pos + 1, &face ) ) {
            fclose( file );
            log.println( "invalid face line: %s", buffer );
            log.unindent();
            log.println( "}" );
            throw Exception( "OBJ model loading error" );
          }
          faces.add( face );
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
              iCurrentMaterial = *value;
            }
            else {
              iCurrentMaterial = 0;
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

    // copy everything into arrays for memory optimisation
    if( positions.isEmpty() ) {
      throw Exception( "OBJ model loading error" );
    }
    for( int i = 0; i < positions.length(); ++i ) {
      positions[i] = translation + scaling * positions[i];
    }

    if( faces.isEmpty() ) {
      log.println( "no faces" );
      log.unindent();
      log.println( "}" );
      throw Exception( "OBJ model loading error" );
    }

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

  void OBJ::draw() const
  {
    int  iCurrentMaterial = -1;
    bool isTransfluent = false;
    bool isTextured = true;

    for( int i = 0; i < faces.length(); ++i ) {
      const Face& face = faces[i];

      if( iCurrentMaterial != face.iMaterial ) {
        iCurrentMaterial = face.iMaterial;
        const Material& material = materials[iCurrentMaterial];

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

      glBegin( GL_POLYGON );
        foreach( vertex, face.vertices.citer() ) {
          if( !texCoords.isEmpty() ) {
            glTexCoord2fv( &texCoords[ vertex->iTexCoord ].u );
          }
          if( !normals.isEmpty() ) {
            glNormal3fv( normals[ vertex->iNorm ] );
          }
          glVertex3fv( positions[ vertex->iPos ] );
        }
      glEnd();
    }

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );

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

  void OBJ::saveCached( const char* fileName )
  {
    // for now, ranges will contain indices of faces that begin and end the range
    Vector<Range> ranges;
    Map<Face::Vertex> uniqueVertices;
    int currentMaterial = faces[0].iMaterial;
    // actually number of ranges - 1
    int nRanges = 0;
    int nVertexRefs = 0;

    foreach( face, faces.iter() ) {
      if( face->iMaterial != currentMaterial ) {
        currentMaterial = face->iMaterial;
        ++nRanges;
      }

      foreach( vertex, face->vertices.iter() ) {
        // index is only written to copy of vertex in faces array
        vertex->iVertex = uniqueVertices.include( *vertex );
        ++nVertexRefs;
      }
    }

    // vertex buffer
    DArray<Vertex> vertices( uniqueVertices.length() );

    for( int i = 0; i < vertices.length(); ++i ) {
      const Face::Vertex& faceVertex = uniqueVertices[i];

      vertices[i].pos      = positions[faceVertex.iPos];
      vertices[i].norm     = normals[faceVertex.iNorm];
      vertices[i].texCoord = texCoords[faceVertex.iTexCoord];
    }

    // index buffer
    DArray<int> indices( nVertexRefs + nRanges * 2 );

    for( int i = 0; i < faces.length(); ++i ) {
      const Face& face = faces[i];

      for( int j = 0; j < face.vertices.length(); ++j ) {
        const Face::Vertex& vertex = face.vertices[j];


      }
    }
  }

  void OBJ::trim()
  {
    positions.clear();
    normals.clear();
    texCoords.clear();
    faces.clear();
    materials.clear();
    materialIndices.clear();
  }

}
}
