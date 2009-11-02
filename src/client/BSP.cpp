/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "BSP.h"

#include "Context.h"
#include "Frustum.h"
#include "Water.h"
#include "Render.h"

#include <GL/glext.h>

#ifdef OZ_MINGW32
static PFNGLACTIVETEXTUREPROC glActiveTexture             = null;
static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = null;
#endif

namespace oz
{
namespace client
{

  const float BSP::GAMMA_CORR = 1.0f;

  const Structure *BSP::str;
  Vec3  BSP::camPos;

  int   BSP::waterFlags;
  Vector<const oz::BSP::Face*> BSP::waterFaces;

  inline Bounds BSP::rotateBounds( const Bounds &bounds, Structure::Rotation rotation )
  {
    Bounds rotatedBounds;

    switch( rotation ) {
      default:
      case Structure::R0: {
        return bounds;
      }
      case Structure::R90: {
        return Bounds( Vec3( -bounds.maxs.y, bounds.mins.x, bounds.mins.z ),
                       Vec3( -bounds.mins.y, bounds.maxs.x, bounds.maxs.z ) );
      }
      case Structure::R180: {
        return Bounds( Vec3( -bounds.maxs.x, -bounds.maxs.y, bounds.mins.z ),
                       Vec3( -bounds.mins.x, -bounds.mins.y, bounds.maxs.z ) );
      }
      case Structure::R270: {
        return Bounds( Vec3( bounds.mins.y, -bounds.maxs.x, bounds.mins.z ),
                       Vec3( bounds.maxs.y, -bounds.mins.x, bounds.maxs.z ) );
      }
    }
  }

  int BSP::getLeaf() const
  {
    int nodeIndex = 0;
    do {
      oz::BSP::Node  &node  = bsp->nodes[nodeIndex];
      oz::BSP::Plane &plane = bsp->planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        nodeIndex = node.back;
      }
      else {
        nodeIndex = node.front;
      }
    }
    while( nodeIndex >= 0 );

    return ~nodeIndex;
  }

  void BSP::checkInWaterBrush( const oz::BSP::Leaf *leaf ) const
  {
    for( int i = 0; i < leaf->nBrushes; i++ ) {
      oz::BSP::Brush *brush = &bsp->brushes[ bsp->leafBrushes[leaf->firstBrush + i] ];

      if( brush->material & Material::WATER_BIT ) {
        for( int i = 0; i < brush->nSides; i++ ) {
          oz::BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

          if( ( camPos * plane.normal - plane.distance ) >= 0.0f ) {
            goto nextBrush;
          }
        }
        waterFlags |= IN_WATER_BRUSH;
        return;
      }
      nextBrush:;
    }
  }

  void BSP::drawFace( const oz::BSP::Face *face ) const
  {
    if( face->material & Material::WATER_BIT ) {
      waterFlags |= DRAW_WATER;
      return;
    }

    glVertexPointer( 3, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                     bsp->vertices[face->firstVertex].p );
    glNormal3fv( face->normal );

    glBindTexture( GL_TEXTURE_2D, textures[face->texture] );

    glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                       bsp->vertices[face->firstVertex].texCoord );

    if( lightMaps != null ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, lightMaps[face->lightmap] );

      glClientActiveTexture( GL_TEXTURE1 );
      glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                         bsp->vertices[face->firstVertex].lightmapCoord );

      glActiveTexture( GL_TEXTURE0 );
      glClientActiveTexture( GL_TEXTURE0 );
    }

    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );
  }

  void BSP::drawFaceWater( const oz::BSP::Face *face ) const
  {
    glColor4f( 1.0f, 1.0f, 1.0f, water.alpha1 );
    glBindTexture( GL_TEXTURE_2D, textures[face->texture] );

    glVertexPointer( 3, GL_FLOAT, sizeof( oz::BSP::Vertex ), bsp->vertices[face->firstVertex].p );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                       bsp->vertices[face->firstVertex].texCoord );

    if( lightMaps != null ) {
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, lightMaps[face->lightmap] );

      glClientActiveTexture( GL_TEXTURE1 );
      glTexCoordPointer( 2, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                         bsp->vertices[face->firstVertex].lightmapCoord );

      glActiveTexture( GL_TEXTURE0 );
      glClientActiveTexture( GL_TEXTURE0 );
    }

    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glFrontFace( GL_CCW );
    glNormal3f( -face->normal.x, -face->normal.y, -face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glBindTexture( GL_TEXTURE_2D, textures[face->texture] );

    glColor4f( 1.0f, 1.0f, 1.0f, water.alpha2 );

    glMatrixMode( GL_TEXTURE );
    glLoadMatrixf( Mat44( 1.0f,            0.0f,            0.0f, 0.0f,
                          0.0f,            1.0f,            0.0f, 0.0f,
                          0.0f,            0.0f,            1.0f, 0.0f,
                          Water::TEX_BIAS, Water::TEX_BIAS, 0.0f, 1.0f ) );


    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glFrontFace( GL_CW );
    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  }

  void BSP::drawNode( int nodeIndex )
  {
    if( nodeIndex >= 0 ) {
      oz::BSP::Node  &node  = bsp->nodes[nodeIndex];
      oz::BSP::Plane &plane = bsp->planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        drawNode( node.back );
        drawNode( node.front );
      }
      else {
        drawNode( node.front );
        drawNode( node.back );
      }
    }
    else {
      oz::BSP::Leaf &leaf = bsp->leafs[~nodeIndex];
      Bounds rotatedLeaf  = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + str->p ) ) {
        for( int i = 0; i < leaf.nFaces; i++ ) {
          int faceIndex = bsp->leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( faceIndex ) ) {
            drawFace( &bsp->faces[faceIndex] );
            drawnFaces.set( faceIndex );
          }
        }
      }
    }
  }

  void BSP::drawNodeWater( int nodeIndex )
  {
    if( nodeIndex >= 0 ) {
      oz::BSP::Node  &node  = bsp->nodes[nodeIndex];
      oz::BSP::Plane &plane = bsp->planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        drawNodeWater( node.back );
        drawNodeWater( node.front );
      }
      else {
        drawNodeWater( node.front );
        drawNodeWater( node.back );
      }
    }
    else {
      oz::BSP::Leaf &leaf = bsp->leafs[~nodeIndex];
      Bounds rotatedLeaf  = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + str->p ) ) {
        for( int i = 0; i < leaf.nFaces; i++ ) {
          int faceIndex = bsp->leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( faceIndex ) ) {
            drawFaceWater( &bsp->faces[faceIndex] );
            drawnFaces.set( faceIndex );
          }
        }
      }
    }
  }

  BSP::BSP( oz::BSP *bsp_ )
  {
    bsp = bsp_;

#ifdef OZ_MINGW32
    if( glActiveTexture == null ) {
      glActiveTexture       = reinterpret_cast<PFNGLACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glActiveTexture" ) );
      glClientActiveTexture = reinterpret_cast<PFNGLCLIENTACTIVETEXTUREPROC>( SDL_GL_GetProcAddress( "glClientActiveTexture" ) );
    }
#endif

    log.println( "Loading BSP structure {" );
    log.indent();

    textures = new uint[bsp->nTextures];
    for( int i = 0; i < bsp->nTextures; i++ ) {
      if( bsp->textures[i] >= 0 ) {
        textures[i] = context.requestTexture( bsp->textures[i] );
      }
    }

    if( bsp->nLightmaps == 0 ) {
      lightMaps = null;
    }
    else {
      lightMaps = new uint[bsp->nLightmaps];
      for( int i = 0; i < bsp->nLightmaps; i++ ) {
        ubyte *bits = reinterpret_cast<ubyte*>( bsp->lightmaps[i].bits );

        for( int j = 0; j < oz::BSP::LIGHTMAP_SIZE; j++ ) {
          bits[j] += static_cast<ubyte>( ( 255 - bits[j] ) * GAMMA_CORR );
        }
        lightMaps[i] = context.createTexture( bits,
                                              oz::BSP::LIGHTMAP_DIM,
                                              oz::BSP::LIGHTMAP_DIM,
                                              oz::BSP::LIGHTMAP_BPP );
      }
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

    log.unindent();
    log.println( "}" );
  }

  BSP::~BSP()
  {
    for( int i = 0; i < bsp->nTextures; i++ ) {
      if( bsp->textures[i] >= 0 ) {
        context.releaseTexture( bsp->textures[i] );
      }
    }
    delete[] textures;

    if( lightMaps != null ) {
      for( int i = 0; i < bsp->nLightmaps; i++ ) {
        context.freeTexture( lightMaps[i] );
      }
      delete[] lightMaps;
    }
  }

  int BSP::draw( const Structure *str_ )
  {
    str = str_;
    camPos = camera.p - str->p;

    if( lightMaps != null ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }

    glClientActiveTexture( GL_TEXTURE0 );

    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * str->rot, 0.0f, 0.0f, 1.0f );

    drawnFaces = hiddenFaces;

    int leafIndex = getLeaf();
    checkInWaterBrush( &bsp->leafs[leafIndex] );

    if( bsp->visual.bitsets != null ) {
      int    cluster = bsp->leafs[leafIndex].cluster;
      Bitset &bitset = bsp->visual.bitsets[cluster];

      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster < 0 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + str->p ) )
        {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face &face = bsp->faces[faceIndex];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + str->p ) ) {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face &face = bsp->faces[faceIndex];

            if( !drawnFaces.get( faceIndex ) ) {
              drawFace( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    glPopMatrix();

    return waterFlags;
  }

  void BSP::drawWater( const Structure *str_ )
  {
    str = str_;
    camPos = camera.p - str->p;

    if( lightMaps != null ) {
      glActiveTexture( GL_TEXTURE1 );
      glEnable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    else {
      glActiveTexture( GL_TEXTURE1 );
      glDisable( GL_TEXTURE_2D );
      glActiveTexture( GL_TEXTURE0 );
    }
    glPushMatrix();
    glTranslatef( str->p.x, str->p.y, str->p.z );
    glRotatef( 90.0f * str->rot, 0.0f, 0.0f, 1.0f );

    drawnFaces = hiddenFaces;

    int leafIndex = getLeaf();

    if( bsp->visual.bitsets != null ) {
      int    cluster = bsp->leafs[leafIndex].cluster;
      Bitset &bitset = bsp->visual.bitsets[cluster];

      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster < 0 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + str->p ) )
        {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face &face = bsp->faces[faceIndex];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( faceIndex ) ) {
              drawFaceWater( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < bsp->nLeafs; i++ ) {
        oz::BSP::Leaf &leaf = bsp->leafs[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + str->p ) ) {
          for( int j = 0; j < leaf.nFaces; j++ ) {
            int faceIndex = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face &face = bsp->faces[faceIndex];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( faceIndex ) ) {
              drawFaceWater( &face );
              drawnFaces.set( faceIndex );
            }
          }
        }
      }
    }
    glPopMatrix();
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
          drawFace( &bsp->faces[faceIndex] );
          drawnFaces.set( faceIndex );
        }
      }
    }
    glEndList();

    return list;
  }

  void BSP::beginRender()
  {
    waterFlags = 0;
    waterFaces.clear();

    glFrontFace( GL_CW );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  }

  void BSP::endRender()
  {
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );

    glFrontFace( GL_CCW );
  }

}
}
