/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BSP.hpp"

#include "client/Context.hpp"
#include "client/Compiler.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Shape.hpp"
#include "client/Water.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  static const int QBSP_SLICK_BIT    = 0x00000002;
  static const int QBSP_LADDER_BIT   = 0x00000008;
  static const int QBSP_WATER_BIT    = 0x00000020;
  static const int QBSP_NONSOLID_BIT = 0x00004000;

  struct QBSPHeader
  {
    char id[4];
    int  version;
  };

  struct QBSPLump
  {
    int offset;
    int length;
  };

  enum QBSPLumpType : int
  {
    QBSP_LUMP_ENTITIES = 0,
    QBSP_LUMP_TEXTURES,
    QBSP_LUMP_PLANES,
    QBSP_LUMP_NODES,
    QBSP_LUMP_LEAFS,
    QBSP_LUMP_LEAFFACES,
    QBSP_LUMP_LEAFBRUSHES,
    QBSP_LUMP_MODELS,
    QBSP_LUMP_BRUSHES,
    QBSP_LUMP_BRUSHSIDES,
    QBSP_LUMP_VERTICES,
    QBSP_LUMP_INDICES,
    QBSP_LUMP_SHADERS,
    QBSP_LUMP_FACES,
    QBSP_LUMP_LIGHTMAPS,
    QBSP_LUMP_LIGHTVOLUMES,
    QBSP_LUMP_VISUALDATA,
    QBSP_LUMPS_NUM
  };

  struct QBSPTexture
  {
    char name[64];
    int  flags;
    int  type;
  };

  struct QBSPModel
  {
    float bb[2][3];

    int firstFace;
    int nFaces;

    int firstBrush;
    int nBrushes;
  };

  struct QBSPVertex
  {
    float p[3];
    float texCoord[2];
    float lightmapCoord[2];
    float normal[3];
    char  colour[4];
  };

  struct QBSPFace
  {
    int   texture;
    int   effect;
    int   type;

    int   firstVertex;
    int   nVertices;

    int   firstIndex;
    int   nIndices;

    int   lightmap;
    int   lightmapCorner[2];
    int   lightmapSize[2];

    float lightmapPos[3];
    float lightmapVecs[2][3];

    float normal[3];

    int   size[2];
  };

  Point3 BSP::camPos;

  static int nTextures;
  static int nModels;
  static int nVertices;
  static int nIndices;
  static int nFaces;

  static DArray<QBSPTexture> textures;
  static DArray<QBSPModel>   models;
  static DArray<QBSPVertex>  vertices;
  static DArray<int>         indices;
  static DArray<QBSPFace>    faces;

  bool BSP::isInWater() const
  {
    int nodeIndex = 0;
    do {
      const oz::BSP::Node& node  = bsp->nodes[nodeIndex];
      const Plane&         plane = bsp->planes[node.plane];

      if( plane * camPos < 0.0f ) {
        nodeIndex = node.back;
      }
      else {
        nodeIndex = node.front;
      }

      hard_assert( nodeIndex != 0 );
    }
    while( nodeIndex >= 0 );

    const oz::BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

    for( int i = 0; i < leaf.nBrushes; ++i ) {
      const oz::BSP::Brush* brush = &bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

      if( brush->material & Material::WATER_BIT ) {
        for( int i = 0; i < brush->nSides; ++i ) {
          const Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

          if( plane * camPos >= 0.0f ) {
            goto nextBrush;
          }
        }
        return true;
      }
      nextBrush:;
    }
    return false;
  }

  void BSP::loadQBSP( const char* fileName )
  {
    String rcFile = fileName + String( ".rc" );
    String bspFile = fileName + String( ".bsp" );

    Config bspConfig;
    if( !bspConfig.load( rcFile ) ) {
      throw Exception( "BSP config file cannot be read" );
    }

    float scale = bspConfig.get( "scale", 0.01f );
    float maxDim = bspConfig.get( "maxDim", Math::INF );

    if( Math::isNaN( scale ) ) {
      throw Exception( "BSP scale is NaN" );
    }

    FILE* file = fopen( bspFile, "rb" );
    if( file == null ) {
      throw Exception( "BSP file not found" );
    }

    QBSPHeader header;
    fread( &header, sizeof( QBSPHeader ), 1, file );

    if( header.id[0] != 'I' || header.id[1] != 'B' || header.id[2] != 'S' || header.id[3] != 'P' ||
        header.version != 46 )
    {
      throw Exception( "BSP file has invalid format/version figerprint" );
    }

    QBSPLump lumps[QBSP_LUMPS_NUM];
    fread( lumps, sizeof( QBSPLump ), QBSP_LUMPS_NUM, file );

    nTextures = int( lumps[QBSP_LUMP_TEXTURES].length / sizeof( QBSPTexture ) );
    textures.alloc( nTextures );
    fseek( file, lumps[QBSP_LUMP_TEXTURES].offset, SEEK_SET );
    fread( textures, sizeof( QBSPTexture ), nTextures, file );

    nModels = int( lumps[QBSP_LUMP_MODELS].length / sizeof( QBSPModel ) );
    models.alloc( nModels );
    fseek( file, lumps[QBSP_LUMP_MODELS].offset, SEEK_SET );
    fread( models, sizeof( QBSPModel ), nModels, file );

    nVertices = int( lumps[QBSP_LUMP_VERTICES].length / sizeof( QBSPVertex ) );
    vertices.alloc( nVertices );
    fseek( file, lumps[QBSP_LUMP_VERTICES].offset, SEEK_SET );
    fread( vertices, sizeof( QBSPVertex ), nVertices, file );

    foreach( vertex, vertices.iter() ) {
      vertex->p[0] *= scale;
      vertex->p[1] *= scale;
      vertex->p[2] *= scale;
    }

    nIndices = int( lumps[QBSP_LUMP_INDICES].length / sizeof( int ) );
    indices.alloc( nIndices );
    fseek( file, lumps[QBSP_LUMP_INDICES].offset, SEEK_SET );
    fread( indices, sizeof( int ), nIndices, file );

    nFaces = int( lumps[QBSP_LUMP_FACES].length / sizeof( QBSPFace ) );
    faces.alloc( nFaces );
    fseek( file, lumps[QBSP_LUMP_FACES].offset, SEEK_SET );
    fread( faces, sizeof( QBSPFace ), nFaces, file );

    foreach( face, faces.iter() ) {
      for( int i = 0; i < face->nVertices; ++i ) {
        const QBSPVertex& vertex = vertices[face->firstVertex + i];

        if( vertex.p[0] < -maxDim || vertex.p[0] > +maxDim ||
            vertex.p[1] < -maxDim || vertex.p[1] > +maxDim ||
            vertex.p[2] < -maxDim || vertex.p[2] > +maxDim )
        {
          face->nIndices = 0;
          break;
        }
      }
    }

    fclose( file );
  }

  void BSP::freeQBSP()
  {
    textures.dealloc();
    models.dealloc();
    vertices.dealloc();
    indices.dealloc();
    faces.dealloc();
  }

  void BSP::optimise()
  {
    log.println( "Optimising BSP model {" );
    log.indent();

    // remove faces that lay out of boundaries
    for( int i = 0; i < nFaces; ) {
      hard_assert( faces[i].nVertices > 0 && faces[i].nIndices >= 0 );

      if( faces[i].nIndices != 0 ) {
        ++i;
        continue;
      }

      aRemove<QBSPFace>( faces, i, nFaces );
      --nFaces;
      log.print( "outside face removed " );

      // adjust face references
      for( int j = 0; j < nModels; ++j ) {
        if( i < models[j].firstFace ) {
          --models[j].firstFace;
        }
        else if( i < models[j].firstFace + models[j].nFaces ) {
          hard_assert( models[j].nFaces > 0 );

          --models[j].nFaces;
        }
      }
      log.printEnd();
    }

    log.unindent();
    log.println( "}" );
  }

  void BSP::save( const char* file )
  {
    log.print( "Dumping BSP model to '%s' ...", file );

    Vector<MeshData> meshes( nModels );
    int size = sizeof( int );

    for( int i = 0; i < nModels; ++i ) {
      compiler.beginMesh();
      compiler.enable( CAP_UNIQUE );
      compiler.enable( CAP_CW );

      for( int j = 0; j < models[i].nFaces; ++j ) {
        const QBSPFace& face = faces[ models[i].firstFace + j ];

        const QBSPTexture& texture = textures[face.texture];
        String name = texture.name;

        if( name.length() <= 12 || name.equals( "textures/NULL" ) ||
            ( texture.flags & QBSP_LADDER_BIT ) )
        {
          name = "";
        }
        else {
          name = name.substring( 12 );
          int index = translator.textureIndex( name );

          if( index == -1 ) {
            name = "";
          }
          else {
            name = translator.textures[index].path;
          }
        }

        if( texture.type & QBSP_WATER_BIT ) {
          compiler.material( GL_FRONT, GL_DIFFUSE, Quat( 1.0f, 1.0f, 1.0f, 0.5f ) );
        }
        else {
          compiler.material( GL_FRONT, GL_DIFFUSE, Quat( 1.0f, 1.0f, 1.0f, 1.0f ) );
        }

        compiler.texture( 0, GL_TEXTURE_2D, name );
        compiler.begin( GL_TRIANGLES );

        for( int k = 0; k < face.nIndices; ++k ) {
          const QBSPVertex& vertex = vertices[ face.firstVertex + indices[face.firstIndex + k] ];

          compiler.texCoord( 0, vertex.texCoord );
          compiler.normal( face.normal );
          compiler.vertex( vertex.p );
        }

        compiler.end();
      }

      compiler.endMesh();

      meshes.add();
      compiler.getMeshData( &meshes.last() );

      size += meshes.last().getSize();
    }

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( nModels );

    foreach( mesh, meshes.citer() ) {
      mesh->write( &os );
    }

    hard_assert( !os.isAvailable() );
    buffer.write( file );

    log.printEnd( " OK" );
  }

  void BSP::prebuild( const char* name )
  {
    log.println( "Prebuilding Quake 3 BSP model '%s' {", name );
    log.indent();

    loadQBSP( String( "maps/" ) + name );
    optimise();
    save( String( "maps/" ) + name + String( ".ozcBSP" ) );
    freeQBSP();

    log.unindent();
    log.println( "}" );
  }

  BSP::BSP( int bspIndex ) :
      bsp( orbis.bsps[bspIndex] ), flags( 0 ), isLoaded( false )
  {}

  BSP::~BSP()
  {
    foreach( mesh, meshes.iter() ) {
      mesh->unload();
    }
  }

  void BSP::load()
  {
    hard_assert( bsp != null );

    const String& name = bsp->name.cstr();

    log.println( "Loading BSP model '%s' {", name.cstr() );
    log.indent();

    Buffer buffer;
    buffer.read( "maps/" + name + ".ozcBSP" );

    InputStream is = buffer.inputStream();

    int nMeshes = is.readInt();

    meshes.alloc( nMeshes );
    foreach( mesh, meshes.iter() ) {
      mesh->load( &is, GL_STATIC_DRAW );
    }

    log.unindent();
    log.println( "}" );

    isLoaded = true;
  }

  bool BSP::draw( const Struct* str ) const
  {
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    for( int i = 0; i < meshes.length(); ++i ) {
      const Vec3& entityPos = i == 0 ? Vec3::ZERO : str->entities[i - 1].offset;

      glPushMatrix();
      glTranslatef( entityPos.x, entityPos.y, entityPos.z );

      meshes[i].drawSolid();

      glPopMatrix();
    }

    glPopMatrix();

    return isInWater();
  }

  void BSP::drawWater( const Struct* str ) const
  {
    camPos = camera.p + ( Point3::ORIGIN - str->p );

    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    for( int i = 0; i < meshes.length(); ++i ) {
      const Vec3& entityPos = i == 0 ? Vec3::ZERO : str->entities[i - 1].offset;

      glPushMatrix();
      glTranslatef( entityPos.x, entityPos.y, entityPos.z );

      meshes[i].drawAlpha();

      glPopMatrix();
    }

    glPopMatrix();
  }

}
}
