/*
 *  BSP.cpp
 *
 *  Data structure for Quake3 BSP level
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "BSP.hpp"

#include "Translator.hpp"

#define fourC( a, b, c, d ) \
  ( ( a ) | ( ( b ) << 8 ) | ( ( c ) <<16 ) | ( ( d ) << 24 ) )

namespace oz
{

  struct Header
  {
    int id;
    int version;
  };

  struct Lump
  {
    int offset;
    int length;
  };

  //***********************************
  //*       QUAKE 3 BSP FORMAT        *
  //***********************************

  enum QBSPLumpType
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

  struct QBSPNode
  {
    int plane;

    int front;
    int back;

    int bb[2][3];
  };

  struct QBSPLeaf
  {
    int cluster;
    int area;

    int bb[2][3];

    int firstFace;
    int nFaces;

    int firstSimplex;
    int nSimplexes;
  };

  struct QBSPSimplex
  {
    int firstSide;
    int nSides;
    int texture;
  };

  struct QBSPSimplexSide
  {
    int plane;
    int texture;
  };

  struct QBSPVertex
  {
    Vec3  p;
    float texCoord[2];
    float lightmapCoord[2];
    Vec3  normal;
    ubyte color[4];
  };

  struct QBSPFace
  {
    int  texture;
    int  effect;
    int  type;

    int  firstVertex;
    int  nVertices;

    int  firstIndex;
    int  nIndices;

    int  lightmap;
    int  lightmapCorner[2];
    int  lightmapSize[2];

    Vec3 lightmapPos;
    Vec3 lightmapVecs[2];

    Vec3 normal;

    int  size[2];
  };

  const float BSP::BSP_SCALE = 0.01f;
  const float BSP::QBSP_SCALE = 0.25f;

  BSP::VisualData::VisualData()
  {
    bitsets = null;
  }

  BSP::VisualData::~VisualData()
  {
    if( bitsets != null ) {
      delete[] bitsets;
    }
  }

  BSP::BSP() : textures( null ), planes( null ), nodes( null ), leafs( null ), leafFaces( null ),
    simplices( null ), simplexSides( null ), vertices( null ), indices( null ), faces( null ), lightmaps( null )
  {
  }

  BSP::~BSP()
  {
    free();
  }

  void BSP::loadQBSP( const char *fileName, float scale, float maxDim_ )
  {
    maxDim = maxDim_;

    logFile.print( "Loading Quake 3 BSP structure '%s' ...", fileName );

    FILE *f = fopen( fileName, "rb" );
    if( f == null ) {
      logFile.printRaw( " Not found\n" );
      return;
    }

    Header header;
    fread( &header, sizeof( Header ), 1, f );

    if( header.id != fourC( 'I', 'B', 'S', 'P' ) || header.version != 46 ) {
      logFile.printRaw( " Wrong format\n" );
      return;
    }

    Lump lumps[QBSP_LUMPS_NUM];
    fread( lumps, sizeof( Lump ), QBSP_LUMPS_NUM, f );

    nTextures = lumps[QBSP_LUMP_TEXTURES].length / sizeof( QBSPTexture );
    textures = new int[nTextures];
    fseek( f, lumps[QBSP_LUMP_TEXTURES].offset, SEEK_SET );

    for( int i = 0; i < nTextures; i++ ) {
      QBSPTexture texture;

      fread( &texture, sizeof( QBSPTexture ), 1, f );

      if( String::length( texture.name ) <= 9 ||
          String::equals( texture.name, "textures/NULL" ) )
      {
        textures[i] = -1;
      }
      else {
        String name = "tex/" + String( texture.name ).substring( 9 ) + ".jpg";
        textures[i] = translator.getTexture( name );

        logFile.println( "%s %d", name.cstr(), textures[i] );
      }
      if( texture.flags != 0 ) {
        textures[i] = ~textures[i];
      }
    }

    int nPlanes = lumps[QBSP_LUMP_PLANES].length / sizeof( BSP::Plane );
    planes = new BSP::Plane[nPlanes];
    fseek( f, lumps[QBSP_LUMP_PLANES].offset, SEEK_SET );
    fread( planes, sizeof( BSP::Plane ), nPlanes, f );

    // rescale plane data
    for( int i = 0; i < nPlanes; i++ ) {
      planes[i].distance *= scale;
    }

    int nNodes = lumps[QBSP_LUMP_NODES].length / sizeof( QBSPNode );
    nodes = new BSP::Node[nNodes];
    fseek( f, lumps[QBSP_LUMP_NODES].offset, SEEK_SET );

    for( int i = 0; i < nNodes; i++ ) {
      QBSPNode node;

      fread( &node, sizeof( QBSPNode ), 1, f );

      nodes[i].plane = node.plane;
      nodes[i].front = node.front;
      nodes[i].back  = node.back;
    }

    mins.x = Math::INF;
    mins.y = Math::INF;
    mins.z = Math::INF;

    maxs.x = -Math::INF;
    maxs.y = -Math::INF;
    maxs.z = -Math::INF;

    nLeafs = lumps[QBSP_LUMP_LEAFS].length / sizeof( QBSPLeaf );
    leafs = new BSP::Leaf[nLeafs];
    fseek( f, lumps[QBSP_LUMP_LEAFS].offset, SEEK_SET );

    for( int i = 0; i < nLeafs; i++ ) {
      QBSPLeaf leaf;

      fread( &leaf, sizeof( QBSPLeaf ), 1, f );

      leafs[i].mins.x = (float) leaf.bb[0][0] * scale;
      leafs[i].mins.y = (float) leaf.bb[0][1] * scale;
      leafs[i].mins.z = (float) leaf.bb[0][2] * scale;

      leafs[i].maxs.x = (float) leaf.bb[1][0] * scale;
      leafs[i].maxs.y = (float) leaf.bb[1][1] * scale;
      leafs[i].maxs.z = (float) leaf.bb[1][2] * scale;

      leafs[i].cluster    = leaf.cluster;
      leafs[i].firstFace  = leaf.firstFace;
      leafs[i].nFaces     = leaf.nFaces;
      leafs[i].firstSimplex = leaf.firstSimplex;
      leafs[i].nSimplexes   = leaf.nSimplexes;

      if( leafs[i].mins.x < -maxDim || leafs[i].maxs.x > maxDim ||
          leafs[i].mins.y < -maxDim || leafs[i].maxs.y > maxDim ||
          leafs[i].mins.z < -maxDim || leafs[i].maxs.z > maxDim )
      {
        leafs[i].nSimplexes = 0;
      }
      else {
        mins.x = min( mins.x, leafs[i].mins.x );
        mins.y = min( mins.y, leafs[i].mins.y );
        mins.z = min( mins.z, leafs[i].mins.z );

        maxs.x = max( maxs.x, leafs[i].maxs.x );
        maxs.y = max( maxs.y, leafs[i].maxs.y );
        maxs.z = max( maxs.z, leafs[i].maxs.z );
      }
    }

    int nLeafFaces = lumps[QBSP_LUMP_LEAFFACES].length / sizeof( int );
    leafFaces = new int[nLeafFaces];
    fseek( f, lumps[QBSP_LUMP_LEAFFACES].offset, SEEK_SET );
    fread( leafFaces, sizeof( int ), nLeafFaces, f );

    int nLeafSimplexes = lumps[QBSP_LUMP_LEAFBRUSHES].length / sizeof( int );
    leafSimplexes = new int[nLeafSimplexes];
    fseek( f, lumps[QBSP_LUMP_LEAFBRUSHES].offset, SEEK_SET );
    fread( leafSimplexes, sizeof( int ), nLeafSimplexes, f );

    int nSimplices = lumps[QBSP_LUMP_BRUSHES].length / sizeof( QBSPSimplex );
    simplices = new BSP::Simplex[nSimplices];
    fseek( f, lumps[QBSP_LUMP_BRUSHES].offset, SEEK_SET );

    for( int i = 0; i < nSimplices; i++ ) {
      QBSPSimplex simplex;

      fread( &simplex, sizeof( QBSPSimplex ), 1, f );

      simplices[i].firstSide = simplex.firstSide;
      simplices[i].nSides    = simplex.nSides;
      simplices[i].flags     = textures[simplex.texture] >= 0 ? BSP::COLLIDABLE_BIT : 0;
    }

    int nSimplexSides = lumps[QBSP_LUMP_BRUSHSIDES].length / sizeof( QBSPSimplexSide );
    simplexSides = new int[nSimplexSides];
    fseek( f, lumps[QBSP_LUMP_BRUSHSIDES].offset, SEEK_SET );

    for( int i = 0; i < nSimplexSides; i++ ) {
      QBSPSimplexSide simplexSide;

      fread( &simplexSide, sizeof( QBSPSimplexSide ), 1, f );

      simplexSides[i] = simplexSide.plane;
    }

    int nVertices = lumps[QBSP_LUMP_VERTICES].length / sizeof( QBSPVertex );
    vertices = new BSP::Vertex[nVertices];
    fseek( f, lumps[QBSP_LUMP_VERTICES].offset, SEEK_SET );

    for( int i = 0; i < nVertices; i++ ) {
      QBSPVertex vertex;

      fread( &vertex, sizeof( QBSPVertex ), 1, f );

      vertices[i].p                = vertex.p * scale;
      vertices[i].texCoord[0]      = vertex.texCoord[0];
      vertices[i].texCoord[1]      = vertex.texCoord[1];
      vertices[i].lightmapCoord[0] = vertex.lightmapCoord[0];
      vertices[i].lightmapCoord[1] = vertex.lightmapCoord[1];
    }

    int nIndices = lumps[QBSP_LUMP_INDICES].length / sizeof( int );
    indices = new int[nIndices];
    fseek( f, lumps[QBSP_LUMP_INDICES].offset, SEEK_SET );
    fread( indices, sizeof( int ), nIndices, f );

    nFaces = lumps[QBSP_LUMP_FACES].length / sizeof( QBSPFace );
    faces = new BSP::Face[nFaces];
    fseek( f, lumps[QBSP_LUMP_FACES].offset, SEEK_SET );

    for( int i = 0; i < nFaces; i++ ) {
      QBSPFace face;

      fread( &face, sizeof( QBSPFace ), 1, f );

      faces[i].texture     = face.texture;
      faces[i].firstVertex = face.firstVertex;
      faces[i].nVertices   = face.nVertices;
      faces[i].firstIndex  = face.firstIndex;
      faces[i].nIndices    = face.nIndices;
      faces[i].lightmap    = face.lightmap;
      faces[i].normal      = face.normal;
    }

    nLightmaps = lumps[QBSP_LUMP_LIGHTMAPS].length / sizeof( BSP::Lightmap );
    lightmaps = new BSP::Lightmap[nLightmaps];
    fseek( f, lumps[QBSP_LUMP_LIGHTMAPS].offset, SEEK_SET );
    fread( lightmaps, sizeof( BSP::Lightmap ), nLightmaps, f );

    fseek( f, lumps[QBSP_LUMP_VISUALDATA].offset, SEEK_SET );
    fread( &visual.nClusters, sizeof( int ), 1, f );
    fread( &visual.clusterLength, sizeof( int ), 1, f );

    assert( visual.nClusters > 0 );

    visual.bitsets = new Bitset[visual.nClusters];
    for( int i = 0; i < visual.nClusters; i++ ) {
      visual.bitsets[i].setSize( visual.clusterLength * 8 );
      fread( visual.bitsets[i].dataPtr(), sizeof( char ), visual.clusterLength, f );
    }

    fclose( f );

    for( int i = 0; i < nTextures; i++ ) {
      if( textures[i] < 0 ) {
        textures[i] = ~textures[i];
      }
    }

    logFile.printRaw( " OK\n" );
  }

  void BSP::free()
  {
    if( textures != null ) {
      delete[] textures;
      textures = null;
    }
    if( planes != null ) {
      delete[] planes;
      planes = null;
    }

    if( nodes != null ) {
      delete[] nodes;
      nodes = null;
    }
    if( leafs != null ) {
      delete[] leafs;
      leafs = null;
    }
    if( leafFaces != null ) {
      delete[] leafFaces;
      leafFaces = null;
    }
    if( leafSimplexes != null ) {
      delete[] leafSimplexes;
      leafSimplexes = null;
    }

    if( simplices != null ) {
      delete[] simplices;
      simplices = null;
    }
    if( simplexSides != null ) {
      delete[] simplexSides;
      simplexSides = null;
    }

    if( vertices != null ) {
      delete[] vertices;
      vertices = null;
    }
    if( indices != null ) {
      delete[] indices;
      indices = null;
    }
    if( faces != null ) {
      delete[] faces;
      faces = null;
    }
    if( lightmaps != null ) {
      delete[] lightmaps;
      lightmaps = null;
    }
  }

}
