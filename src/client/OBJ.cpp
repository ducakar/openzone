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

  DArray<OBJ::Vertex>   OBJ::vertices;
  DArray<uint>          OBJ::indices;

  inline OBJ::Face::Vertex::Vertex()
  {}

  inline OBJ::Face::Vertex::Vertex( int pos_, int norm_, int texCoord_ ) :
      position( pos_ ), normal( norm_ ), texCoord( texCoord_ )
  {}

  inline bool OBJ::Face::Vertex::operator == ( const Vertex& v ) const
  {
    return position == v.position && normal == v.normal && texCoord == v.texCoord;
  }

  inline bool OBJ::Face::Vertex::operator != ( const Vertex& v ) const
  {
    return position != v.position || normal != v.normal || texCoord != v.texCoord;
  }

  inline bool OBJ::Face::Vertex::operator < ( const Vertex& v ) const
  {
    return position < v.position ||
        ( position == v.position && ( normal < v.normal ||
            ( normal == v.normal && texCoord < v.texCoord ) ) );
  }

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

    int vertIndex, normalIndex, texCoordIndex;

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
        if( sscanf( pos, "%d", &vertIndex ) != 1 ) {
          return false;
        }
        face->vertices.add( Face::Vertex( vertIndex - 1, -1, -1 ) );

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
        face->vertices.add( Face::Vertex( vertIndex - 1, -1, texCoordIndex - 1 ) );

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
        face->vertices.add( Face::Vertex( vertIndex - 1, normalIndex - 1, -1 ) );

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
        face->vertices.add( Face::Vertex( vertIndex - 1, normalIndex - 1, texCoordIndex - 1 ) );

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

  void OBJ::saveCached( const char* fileName )
  {
    log.print( "Dumping OBJ model to '%s' ...", fileName );

    DArray<int> faceVertices;

    Vector<Range> ranges;
    Map<Face::Vertex> uniqueVertices;
    int currentMaterial = faces[0].material;
    int nRanges = 1;
    // sum of face.vertices.length() for all faces
    int nVertexRefs = 0;

    foreach( face, faces.iter() ) {
      if( face->material != currentMaterial ) {
        currentMaterial = face->material;
        ++nRanges;
      }

      nVertexRefs += face->vertices.length();

      foreach( vertex, face->vertices.citer() ) {
        // index is only written to copy of vertex in faces array
        vertex->index = uniqueVertices.include( *vertex );
      }
    }

    // vertex buffer
    vertices.clear();
    vertices.setSize( uniqueVertices.length() );

    for( int i = 0; i < vertices.length(); ++i ) {
      const Face::Vertex& faceVertex = uniqueVertices[i];

      vertices[i].position = positions[faceVertex.position];
      vertices[i].norm     = faceVertex.normal == -1 ?
          Vec3::ZERO : normals[faceVertex.normal];
      vertices[i].texCoord = faceVertex.texCoord == -1 ?
          TexCoord::ZERO : texCoords[faceVertex.texCoord];
    }

    // index buffer
    indices.setSize( nVertexRefs + ( faces.length() - 1 ) * 2 );
    int k = 0;

    for( int i = 0; i < faces.length(); ++i ) {
      const Face& face = faces[i];

      // for a degenerated triangle, simulating restart-index
      if( i != 0 ) {
        indices[k] = face.vertices.first().index;
        ++k;
      }

      int n = face.vertices.length();
      int n_2 = ( n + 1 ) / 2;

      indices[k] = face.vertices[0].index;
      ++k;

      for( int j = 1; j < n_2; ++j ) {
        indices[k] = face.vertices[j].index;
        ++k;
        indices[k] = face.vertices[n - j].index;
        ++k;
      }

      if( n % 2 == 0 ) {
        indices[k] = face.vertices[n_2].index;
        ++k;
      }

      // for a degenerated triangle, simulating restart-index
      if( i != faces.length() - 1 ) {
        indices[k] = face.vertices.last().index;
        ++k;
      }
    }

    int size = 0;

    size += int( 2                 * sizeof( int ) );
    size += int( vertices.length() * sizeof( Vertex ) );
    size += int( indices.length()  * sizeof( int ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( vertices.length() );
    os.writeInt( indices.length() );

    for( int i = 0; i < vertices.length(); ++i ) {
      os.writeVec3( vertices[i].position );
      os.writeVec3( vertices[i].norm );
      os.writeFloat( vertices[i].texCoord.u );
      os.writeFloat( vertices[i].texCoord.v );
    }

    for( int i = 0; i < indices.length(); ++i ) {
      os.writeInt( indices[i] );
    }

    buffer.write( fileName );

    log.printEnd( " OK" );
  }

  OBJ::OBJ( const char* name_, int )
  {
    name = name_;

    String modelPath = "mdl/" + name + ".ozcOBJ";

    log.println( "Loading OBJ model '%s' {", modelPath.cstr() );
    log.indent();

    Buffer buffer( modelPath );
    InputStream is = buffer.inputStream();

    int nVertices = is.readInt();
    int nIndices = is.readInt();

    DArray<Vertex> vertices( nVertices );
    DArray<uint>   indices( nIndices );

    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].position = is.readVec3();
      vertices[i].norm = is.readVec3();
      vertices[i].texCoord.u = is.readFloat();
      vertices[i].texCoord.v = is.readFloat();
    }

    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = is.readInt();
    }

    glGenBuffers( 1, &arrayBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( Vertex ), vertices, GL_STATIC_DRAW );

    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof( uint ), indices, GL_STATIC_DRAW );

    log.unindent();
    log.println( "}" );
  }

  OBJ::OBJ( const char* name_ )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    int currentMaterial = 0;

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
          face.material = currentMaterial;

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
              currentMaterial = *value;
            }
            else {
              currentMaterial = 0;
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

    saveCached( sPath + "/" + name + ".ozcOBJ" );

    log.unindent();
    log.println( "}" );
  }

  OBJ::~OBJ()
  {
    glDeleteBuffers( 1, &arrayBuffer );
    glDeleteBuffers( 1, &indexBuffer );

    log.print( "Unloading OBJ model '%s' ...", name.cstr() );

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
    int  currentMaterial = -1;
    bool isTransfluent = false;
    bool isTextured = true;

    for( int i = 0; i < faces.length(); ++i ) {
      const Face& face = faces[i];

      if( currentMaterial != face.material ) {
        currentMaterial = face.material;
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

      glBegin( GL_POLYGON );
        foreach( vertex, face.vertices.citer() ) {
          if( !texCoords.isEmpty() ) {
            glTexCoord2fv( &texCoords[ vertex->texCoord ].u );
          }
          if( !normals.isEmpty() ) {
            glNormal3fv( normals[ vertex->normal ] );
          }
          glVertex3fv( positions[ vertex->position ] );
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

  void OBJ::trim()
  {
    positions.clear();
    normals.clear();
    texCoords.clear();
    faces.clear();
    materials.clear();
    materialIndices.clear();
  }

  void OBJ::prebuild( const char* )
  {}

}
}
