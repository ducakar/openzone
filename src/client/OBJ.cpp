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

#include "client/Context.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{

  struct FaceVertex
  {
    // vertex position index in positions array
    int position;
    // vertex normal in normals array
    int normal;
    // vertex texture coordinates in texCoords array
    int texCoord;

    explicit FaceVertex()
    {}

    explicit FaceVertex( int pos_, int norm_, int texCoord_ ) :
        position( pos_ ), normal( norm_ ), texCoord( texCoord_ )
    {}

    // lexicographical order
    bool operator == ( const FaceVertex& v ) const
    {
      return position == v.position && normal == v.normal && texCoord == v.texCoord;
    }

    bool operator < ( const FaceVertex& v ) const
    {
      return position < v.position ||
          ( position == v.position && ( normal < v.normal ||
              ( normal == v.normal && texCoord < v.texCoord ) ) );
    }
  };

  struct Face
  {
    Vector<FaceVertex> vertices;
  };

  struct Part
  {
    Vector<Face> faces;

    Quat   specular;
    Quat   diffuse;
    String texture;
  };

  static Vector<Point3>   positions;
  static Vector<Vec3>     normals;
  static Vector<TexCoord> texCoords;
  static Vector<Part>     parts;
  static HashString<int>  materialIndices;

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

    part.diffuse  = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
    part.specular = Quat( 0.5f, 0.5f, 0.5f, 1.0f );
    part.texture  = "";

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
            part.diffuse  = Quat( 1.0f, 1.0f, 1.0f, 1.0f );
            part.specular = Quat( 0.5f, 0.5f, 0.5f, 1.0f );
            part.texture  = "";
          }
          break;
        }
        case 'K': {
          if( pos[1] == 'd' ) {
            sscanf( pos + 2, "%f %f %f", &part.diffuse.x, &part.diffuse.y, &part.diffuse.z );
          }
          else if( pos[1] == 's' ) {
            sscanf( pos + 2, "%f %f %f", &part.specular.x, &part.specular.y, &part.specular.z );
          }
          break;
        }
        case 'd': {
          sscanf( pos + 1, "%f", &part.diffuse.w );
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

  void OBJ::loadOBJ( const char* name )
  {
    FILE* file;
    char buffer[LINE_BUFFER_SIZE];

    int currentMaterial = 0;

    String sPath = String( "mdl/" ) + name;
    String modelFile = sPath + "/data.obj";
    String configFile = sPath + "/config.rc";

    Config config;
    config.load( configFile );

    log.print( "Loading OBJ model '%s' ...", modelFile.cstr() );

    float scaling = config.get( "scale", 1.0f );
    Vec3 translation = Vec3( config.get( "translate.x", 0.0f ),
                             config.get( "translate.y", 0.0f ),
                             config.get( "translate.z", 0.0f ) );

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
    log.print( "Dumping OBJ model to '%s' ...", fileName );

    Map<FaceVertex> faceVertices;
    Vector<uint>    indices;
    Vector<Segment> segments;

    // build vertex array
    foreach( part, parts.citer() ) {
      foreach( face, part->faces.citer() ) {
        foreach( vertex, face->vertices.citer() ) {
          faceVertices.include( *vertex );
        }
      }
    }

    // build index array
    for( int i = 0; i < parts.length(); ++i ) {
      segments.add();

      const Part& part = parts[i];
      Segment& segment = segments.last();

      segment.diffuse    = part.diffuse;
      segment.specular   = part.specular;
      segment.firstIndex = indices.length();

      for( int j = 0; j < part.faces.length(); ++ j ) {
        const Face& face = part.faces[j];

        int index = faceVertices.index( face.vertices.first() );

        // for a degenerated triangle to restart strip
        if( j != 0 ) {
          indices.add( index );
        }

        // generate triangle strip for a polygon
        int n = face.vertices.length();
        int n_2 = ( n + 1 ) / 2;

        indices.add( index );

        for( int k = 1; k < n_2; ++k ) {
          index = faceVertices.index( face.vertices[k] );
          indices.add( index );
          index = faceVertices.index( face.vertices[n - k] );
          indices.add( index );
        }

        if( n % 2 == 0 ) {
          index = faceVertices.index( face.vertices[n_2] );
          indices.add( index );
        }

        // for a degenerated triangle to restart strip
        if( j != part.faces.length() - 1 ) {
          indices.add( index );
        }
      }
      segment.nIndices = indices.length() - segment.firstIndex;
    }

    int size = 0;

    size += int( 3                     * sizeof( int ) );
    size += int( faceVertices.length() * sizeof( Vertex ) );
    size += int( indices.length()      * sizeof( ushort ) );
    size += int( segments.length()     * sizeof( Segment ) );
    size -= int( segments.length()     * sizeof( uint ) );

    for( int i = 0; i < parts.length(); ++i ) {
      size += int( parts[i].texture.length() + 1 );
    }

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    try {
      os.writeInt( faceVertices.length() );
      os.writeInt( indices.length() );
      os.writeInt( segments.length() );

      for( int i = 0; i < faceVertices.length(); ++i ) {
        int posIndex = faceVertices[i].position;
        int normIndex = faceVertices[i].normal;
        int texIndex = faceVertices[i].texCoord;

        os.writePoint3( positions[posIndex] );
        os.writeVec3( normIndex == -1 ? Vec3::ZERO : normals[normIndex] );
        os.writeFloat( texIndex == -1 ? 0.0f : texCoords[texIndex].u );
        os.writeFloat( texIndex == -1 ? 0.0f : texCoords[texIndex].v );
      }

      for( int i = 0; i < indices.length(); ++i ) {
        os.writeShort( ushort( indices[i] ) );
      }

      hard_assert( segments.length() == parts.length() );

      for( int i = 0; i < segments.length(); ++i ) {
        os.writeQuat( segments[i].diffuse );
        os.writeQuat( segments[i].specular );
        os.writeInt( segments[i].firstIndex );
        os.writeInt( segments[i].nIndices );
        os.writeString( parts[i].texture );
      }
    }
    catch( const Exception& e ) {
      log.printEnd( " Failed: %s", e.what() );
      return;
    }

    hard_assert( !os.isAvailable() );
    if( !buffer.write( fileName ) ) {
      log.printEnd( " Failed to write buffer" );
    }
    else {
      log.printEnd( " OK" );
    }
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

    loadOBJ( name );
    save( "mdl/" + String( name ) + ".ozcOBJ" );
    freeOBJ();

    log.unindent();
    log.println( "}" );
  }

  void OBJ::load()
  {
    String modelPath = "mdl/" + name + ".ozcOBJ";

    log.println( "Loading OBJ model '%s' {", modelPath.cstr() );
    log.indent();

    Buffer buffer;
    if( !buffer.read( modelPath ) ) {
      throw Exception( "Cannot read model file" );
    }
    InputStream is = buffer.inputStream();

    int nVertices = is.readInt();
    int nIndices = is.readInt();
    int nSegments = is.readInt();

    segments.alloc( nSegments );

    Vertex* vertices = new Vertex[nVertices];
    ushort* indices  = new ushort[nIndices];

    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].pos[0] = is.readFloat();
      vertices[i].pos[1] = is.readFloat();
      vertices[i].pos[2] = is.readFloat();

      vertices[i].normal[0] = is.readFloat();
      vertices[i].normal[1] = is.readFloat();
      vertices[i].normal[2] = is.readFloat();

      vertices[i].texCoord[0] = is.readFloat();
      vertices[i].texCoord[1] = is.readFloat();
    }

    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = is.readShort();
    }

    for( int i = 0; i < segments.length(); ++i ) {
      segments[i].diffuse = is.readQuat();
      segments[i].specular = is.readQuat();
      segments[i].firstIndex = is.readInt();
      segments[i].nIndices = is.readInt();

      String texPath = is.readString();
      segments[i].texId = texPath.isEmpty() ? GL_NONE : context.loadTexture( texPath );
    }

    hard_assert( !is.isAvailable() );

    arrayId = context.genArray( VAO_INDEXED | VAO_NORMAL_BIT | VAO_TEXCOORD0_BIT, GL_STATIC_DRAW,
                                vertices, nVertices,
                                indices, nIndices );

    delete[] vertices;
    delete[] indices;

    isLoaded = true;

    log.unindent();
    log.println( "}" );
  }

  OBJ::OBJ( const char* name_ ) : name( name_ ), isLoaded( false )
  {}

  OBJ::~OBJ()
  {
    context.deleteArray( arrayId );

    log.print( "Unloading OBJ model '%s' ...", name.cstr() );

    for( int i = 0; i < segments.length(); ++i ) {
      if( segments[i].texId != GL_NONE ) {
        context.deleteTexture( segments[i].texId );
      }
    }

    log.printEnd( " OK" );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void OBJ::draw() const
  {
    bool isBlendEnabled = false;

    context.bindArray( arrayId );

    for( int i = 0; i < segments.length(); ++i ) {
      const Segment& segment = segments[i];

      glMaterialfv( GL_FRONT, GL_DIFFUSE, segment.diffuse );
      glMaterialfv( GL_FRONT, GL_SPECULAR, segment.specular );
      glBindTexture( GL_TEXTURE_2D, segment.texId );

      if( segment.diffuse.w != 1.0f ) {
        if( !isBlendEnabled ) {
          isBlendEnabled = true;
          glEnable( GL_BLEND );
        }
      }
      else {
        if( isBlendEnabled ) {
          isBlendEnabled = false;
          glDisable( GL_BLEND );
        }
      }

      context.drawIndexedArray( GL_TRIANGLE_STRIP, segments[i].firstIndex, segments[i].nIndices );
    }

    if( isBlendEnabled ) {
      glDisable( GL_BLEND );
    }

    glMaterialfv( GL_FRONT, GL_DIFFUSE, Colours::WHITE );
    glMaterialfv( GL_FRONT, GL_SPECULAR, Colours::BLACK );

    context.unbindArray();
  }

}
}
