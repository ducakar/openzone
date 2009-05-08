/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "BSP.h"

#include "Context.h"
#include "Frustum.h"

#ifdef __WIN32__
static PFNGLACTIVETEXTUREPROC glActiveTexture = null;
static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = null;
#endif

namespace oz
{
namespace client
{

  BSP::BSP( oz::BSP *bsp )
  {
    textures = null;
    lightMaps = null;

    init( bsp );
  }

  BSP::~BSP()
  {
    free();
  }

  const oz::BSP::Leaf *BSP::getLeaf( const Vec3 &p ) const
  {
    int nodeIndex = 0;
    do {
      oz::BSP::Node  &node  = bsp->nodes[nodeIndex];
      oz::BSP::Plane &plane = bsp->planes[node.plane];

      if( ( p * plane.normal - plane.distance ) < 0.0f ) {
        nodeIndex = node.back;
      }
      else {
        nodeIndex = node.front;
      }
    }
    while( nodeIndex >= 0 );

    return &bsp->leafs[~nodeIndex];
  }

  bool BSP::isInWaterBrush( const Vec3 &p, const oz::BSP::Leaf *leaf ) const
  {
    for( int i = 0; i < leaf->nBrushes; i++ ) {
      oz::BSP::Brush *brush = &bsp->brushes[ bsp->leafBrushes[leaf->firstBrush + i] ];

      if( brush->content & oz::BSP::WATER_BIT ) {
        for( int i = 0; i < brush->nSides; i++ ) {
          oz::BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

          float dist = p * plane.normal - plane.distance;

          if( dist > EPSILON ) {
            goto nextBrush;
          }
        }
        return true;
      }
      nextBrush:;
    }
    return false;
  }

  void BSP::drawFace( int faceIndex ) const
  {
    oz::BSP::Face &face = bsp->faces[faceIndex];

    glVertexPointer( 3, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                     (float*) bsp->vertices[face.firstVertex].p );

    glActiveTexture( GL_TEXTURE0 );
    glClientActiveTexture( GL_TEXTURE0 );

    glBindTexture( GL_TEXTURE_2D, textures[face.texture] );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                       bsp->vertices[face.firstVertex].texCoord );

    glActiveTexture( GL_TEXTURE1 );
    glClientActiveTexture( GL_TEXTURE1 );

    glBindTexture( GL_TEXTURE_2D, lightMaps[face.lightmap] );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                       bsp->vertices[face.firstVertex].lightmapCoord );

    glNormal3fv( face.normal );
    glDrawElements( GL_TRIANGLES, face.nIndices, GL_UNSIGNED_INT, &bsp->indices[face.firstIndex] );
  }

  void BSP::init( oz::BSP *bsp_ )
  {
    bsp = bsp_;

#ifdef __WIN32__
    if( glActiveTexture == null ) {
      glActiveTexture = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress( "glActiveTexture" );
      glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)
          SDL_GL_GetProcAddress( "glClientActiveTexture" );
    }
#endif

    logFile.println( "Loading BSP structure {" );
    logFile.indent();

    textures = new uint[bsp->nTextures];
    for( int i = 0; i < bsp->nTextures; i++ ) {
      if( bsp->textures[i] >= 0 ) {
        textures[i] = context.requestTexture( bsp->textures[i] );
      }
    }

    lightMaps = new uint[bsp->nLightmaps];
    for( int i = 0; i < bsp->nLightmaps; i++ ) {
      ubyte *bits = (ubyte*) bsp->lightmaps[i].bits;

      for( int j = 0; j < oz::BSP::LIGHTMAP_SIZE; j++ ) {
        bits[j] += (ubyte) ( ( 255 - bits[j] ) * BSP_GAMMA_CORR );
      }
      lightMaps[i] = context.createTexture( bits,
                                            oz::BSP::LIGHTMAP_DIM,
                                            oz::BSP::LIGHTMAP_DIM,
                                            oz::BSP::LIGHTMAP_BPP );
    }

    hiddenFaces.setSize( bsp->nFaces );
    drawnFaces.setSize( bsp->nFaces );
    hiddenFaces.clearAll();

    if( bsp->visual.bitsets != null ) {
      visibleLeafs.setSize( bsp->visual.bitsets[0].length() );
    }

    for( int i = 0; i < bsp->nFaces; i++ ) {
      oz::BSP::Vertex *verts = &bsp->vertices[ bsp->faces[i].firstVertex ];

      for( int j = 0; j < bsp->faces[i].nVertices; j++ ) {
        if( verts[j].p.x < -bsp->maxDim || verts[j].p.x > bsp->maxDim ||
            verts[j].p.y < -bsp->maxDim || verts[j].p.y > bsp->maxDim ||
            verts[j].p.z < -bsp->maxDim || verts[j].p.z > bsp->maxDim )
        {
          hiddenFaces.set( i );
          break;
        }
      }
    }

    logFile.unindent();
    logFile.println( "}" );
  }

  bool BSP::draw( const Structure *str )
  {
    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * str->rot, 0.0f, 0.0f, 1.0f );

    drawnFaces = hiddenFaces;

    Vec3 relPos = camera.p - str->p;
    const oz::BSP::Leaf *leaf = getLeaf( relPos );
    bool isInWater = isInWaterBrush( relPos, leaf );

    if( bsp->visual.bitsets != null ) {
      // TODO: rotated BSPs
//       int    cluster = bsp->leafs[ getLeafIndex( camera.p ) ].cluster;
//       printf( "%d\n", getLeafIndex( camera.p ) );
//       Bitset &bitset = bsp->visual.bitsets[cluster];

      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];

//         if( ( cluster < 0 || bitset.get( leaf.cluster ) ) && frustum.isVisible( leaf + t.p() ) ) {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( faceIndex );
              drawnFaces.set( faceIndex );
            }
          }
//         }
      }
    }
    else {
      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];

//         if( frustum.isVisible( leaf + t.p() ) ) {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( faceIndex );
              drawnFaces.set( faceIndex );
            }
          }
//         }
      }
    }
    glPopMatrix();

    return isInWater;
  }

  uint BSP::genList()
  {
    uint list = context.genList();

    glNewList( list, GL_COMPILE );

    drawnFaces = hiddenFaces;

    for( int i = 0; i < bsp->nLeafs; i++ ) {
      oz::BSP::Leaf &leaf = bsp->leafs[i];

      for( int j = 0; j < leaf.nFaces; j++ ) {
        int faceIndex = bsp->leafFaces[leaf.firstFace + j];

        if( !drawnFaces.get( faceIndex ) ) {
          drawFace( faceIndex );
          drawnFaces.set( faceIndex );
        }
      }
    }
    glEndList();

    return list;
  }

  void BSP::beginRender()
  {
    glFrontFace( GL_CW );
    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  }

  void BSP::endRender()
  {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
    glFrontFace( GL_CCW );
  }

  void BSP::free()
  {
    if( textures != null ) {
      delete[] textures;
    }
    if( lightMaps != null ) {
      delete[] lightMaps;
    }
  }

}
}
