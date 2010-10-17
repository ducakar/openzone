/*
 *  BSP.cpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BSP.hpp"

#include "client/Context.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"
#include "client/Water.hpp"
#include "client/Render.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  const Structure* BSP::str;
  Vec3  BSP::camPos;
  int   BSP::waterFlags;

  inline Bounds BSP::rotateBounds( const Bounds& bounds, Structure::Rotation rotation )
  {
    Bounds rotatedBounds;

    switch( rotation ) {
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
      default:
      case Structure::R270: {
        return Bounds( Vec3( bounds.mins.y, -bounds.maxs.x, bounds.mins.z ),
                       Vec3( bounds.maxs.y, -bounds.mins.x, bounds.maxs.z ) );
      }
    }
  }

  int BSP::getLeaf() const
  {
    int iNode = 0;
    do {
      const oz::BSP::Node&  node  = bsp->nodes[iNode];
      const oz::BSP::Plane& plane = bsp->planes[node.plane];

      if( ( camPos * plane.normal - plane.distance ) < 0.0f ) {
        iNode = node.back;
      }
      else {
        iNode = node.front;
      }

      assert( iNode != 0 );
    }
    while( iNode >= 0 );

    return ~iNode;
  }

  void BSP::checkInWaterBrush( const oz::BSP::Leaf* leaf ) const
  {
    for( int i = 0; i < leaf->nBrushes; ++i ) {
      const oz::BSP::Brush* brush = &bsp->brushes[ bsp->leafBrushes[leaf->firstBrush + i] ];

      if( brush->material & Material::WATER_BIT ) {
        for( int i = 0; i < brush->nSides; ++i ) {
          const oz::BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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

  void BSP::drawFace( const oz::BSP::Face* face ) const
  {
    if( face->material & Material::WATER_BIT ) {
      waterFlags |= DRAW_WATER;
      return;
    }

    glVertexPointer( 3, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                     bsp->vertices[face->firstVertex].p );

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

    glNormal3fv( face->normal );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );
  }

  void BSP::drawFaceWater( const oz::BSP::Face* face ) const
  {
    glVertexPointer( 3, GL_FLOAT, sizeof( oz::BSP::Vertex ),
                     bsp->vertices[face->firstVertex].p );

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend1 );
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

    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glFrontFace( GL_CCW );
    glNormal3f( -face->normal.x, -face->normal.y, -face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glMatrixMode( GL_TEXTURE );
    glLoadMatrixf( Mat44( 1.0f,            0.0f,            0.0f, 0.0f,
                          0.0f,            1.0f,            0.0f, 0.0f,
                          0.0f,            0.0f,            1.0f, 0.0f,
                          Water::TEX_BIAS, Water::TEX_BIAS, 0.0f, 1.0f ) );

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend2 );
    glBindTexture( GL_TEXTURE_2D, textures[face->texture] );

    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glFrontFace( GL_CW );
    glNormal3f( face->normal.x, face->normal.y, face->normal.z );
    glDrawElements( GL_TRIANGLES, face->nIndices, GL_UNSIGNED_INT, &bsp->indices[face->firstIndex] );

    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
  }

  void BSP::drawNode( int iNode )
  {
    if( iNode >= 0 ) {
      const oz::BSP::Node&  node  = bsp->nodes[iNode];
      const oz::BSP::Plane& plane = bsp->planes[node.plane];

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
      const oz::BSP::Leaf& leaf = bsp->leaves[~iNode];
      Bounds rotatedLeaf  = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + str->p ) ) {
        for( int i = 0; i < leaf.nFaces; ++i ) {
          int iFace = bsp->leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( iFace ) ) {
            drawFace( &bsp->faces[iFace] );
            drawnFaces.set( iFace );
          }
        }
      }
    }
  }

  void BSP::drawNodeWater( int iNode )
  {
    if( iNode >= 0 ) {
      const oz::BSP::Node&  node  = bsp->nodes[iNode];
      const oz::BSP::Plane& plane = bsp->planes[node.plane];

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
      const oz::BSP::Leaf& leaf = bsp->leaves[~iNode];
      Bounds rotatedLeaf  = rotateBounds( leaf, str->rot );

      if( frustum.isVisible( leaf + str->p ) ) {
        for( int i = 0; i < leaf.nFaces; ++i ) {
          int iFace = bsp->leafFaces[leaf.firstFace + i];

          if( !drawnFaces.get( iFace ) ) {
            drawFaceWater( &bsp->faces[iFace] );
            drawnFaces.set( iFace );
          }
        }
      }
    }
  }

  BSP::BSP( int iBsp ) : isUpdated( false )
  {
    bsp = orbis.bsps[iBsp];

    log.println( "Loading BSP model '%s' {", translator.bsps[iBsp].name.cstr() );
    log.indent();

    textures = new uint[bsp->nTextures];
    for( int i = 0; i < bsp->nTextures; ++i ) {
      if( bsp->textures[i] != -1 ) {
        textures[i] = context.requestTexture( bsp->textures[i] );
      }
    }

    if( bsp->nLightmaps == 0 ) {
      lightMaps = null;
    }
    else {
      lightMaps = new uint[bsp->nLightmaps];
      for( int i = 0; i < bsp->nLightmaps; ++i ) {
        lightMaps[i] = context.createTexture( bsp->lightmaps[i].bits,
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

    for( int i = 0; i < bsp->nFaces; ++i ) {
      const oz::BSP::Vertex* verts = &bsp->vertices[ bsp->faces[i].firstVertex ];

      for( int j = 0; j < bsp->faces[i].nVertices; ++j ) {
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
    for( int i = 0; i < bsp->nTextures; ++i ) {
      if( bsp->textures[i] != -1 ) {
        context.releaseTexture( bsp->textures[i] );
      }
    }
    delete[] textures;

    if( lightMaps != null ) {
      for( int i = 0; i < bsp->nLightmaps; ++i ) {
        context.freeTexture( lightMaps[i] );
      }
      delete[] lightMaps;
    }
  }

  int BSP::draw( const Structure* str_ )
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
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    int iLeaf = getLeaf();
    checkInWaterBrush( &bsp->leaves[iLeaf] );

    if( bsp->visual.bitsets != null ) {
      int     cluster = bsp->leaves[iLeaf].cluster;
      Bitset& bitset  = bsp->visual.bitsets[cluster];

      for( int i = 0; i < bsp->nLeaves; ++i ) {
        const oz::BSP::Leaf& leaf = bsp->leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster == -1 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + str->p ) )
        {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int iFace = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face& face = bsp->faces[iFace];

            if( !drawnFaces.get( iFace ) ) {
              drawFace( &face );
              drawnFaces.set( iFace );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < bsp->nLeaves; ++i ) {
        const oz::BSP::Leaf& leaf = bsp->leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + str->p ) ) {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int iFace = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face& face = bsp->faces[iFace];

            if( !drawnFaces.get( iFace ) ) {
              drawFace( &face );
              drawnFaces.set( iFace );
            }
          }
        }
      }
    }
    glPopMatrix();

    return waterFlags;
  }

  void BSP::drawWater( const Structure* str_ )
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
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    int iLeaf = getLeaf();

    if( bsp->visual.bitsets != null ) {
      int     cluster = bsp->leaves[iLeaf].cluster;
      Bitset& bitset  = bsp->visual.bitsets[cluster];

      for( int i = 0; i < bsp->nLeaves; ++i ) {
        const oz::BSP::Leaf& leaf = bsp->leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( ( cluster == -1 || bitset.get( cluster ) ) &&
            frustum.isVisible( rotatedLeaf + str->p ) )
        {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int iFace = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face& face = bsp->faces[iFace];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( iFace ) ) {
              drawFaceWater( &face );
              drawnFaces.set( iFace );
            }
          }
        }
      }
    }
    else {
      for( int i = 0; i < bsp->nLeaves; ++i ) {
        const oz::BSP::Leaf& leaf = bsp->leaves[i];
        Bounds rotatedLeaf = rotateBounds( leaf, str->rot );

        if( frustum.isVisible( rotatedLeaf + str->p ) ) {
          for( int j = 0; j < leaf.nFaces; ++j ) {
            int iFace = bsp->leafFaces[leaf.firstFace + j];
            const oz::BSP::Face& face = bsp->faces[iFace];

            if( ( face.material & Material::WATER_BIT ) && !drawnFaces.get( iFace ) ) {
              drawFaceWater( &face );
              drawnFaces.set( iFace );
            }
          }
        }
      }
    }
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );
    glPopMatrix();
  }

  int BSP::fullDraw( const Structure* str_ )
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
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    int iLeaf = getLeaf();
    checkInWaterBrush( &bsp->leaves[iLeaf] );

    for( int i = 0; i < bsp->nModels; ++i ) {
      const oz::BSP::Model& model = bsp->models[i];
      const Vec3& entityPos = str->entities[i].offset;

      glPushMatrix();
      glTranslatef( entityPos.x, entityPos.y, entityPos.z );

      for( int j = 0; j < model.nFaces; ++j ) {
        const oz::BSP::Face& face = bsp->faces[ model.firstFace + j ];

        if( !hiddenFaces.get( model.firstFace + j ) ) {
          drawFace( &face );
        }
      }
      glPopMatrix();
    }
    glPopMatrix();

    return waterFlags;
  }

  void BSP::fullDrawWater( const Structure* str_ )
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
    glRotatef( 90.0f * float( str->rot ), 0.0f, 0.0f, 1.0f );

    for( int i = 0; i < bsp->nFaces; ++i ) {
      const oz::BSP::Face& face = bsp->faces[i];

      if( ( face.material & Material::WATER_BIT ) && !hiddenFaces.get( i ) ) {
        drawFaceWater( &face );
      }
    }
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::WHITE );
    glPopMatrix();
  }

  uint BSP::genList()
  {
    uint list = context.genList();

    glNewList( list, GL_COMPILE );

    aCopy<ulong>( drawnFaces, hiddenFaces, hiddenFaces.unitLength() );

    for( int i = 0; i < bsp->nLeaves; ++i ) {
      const oz::BSP::Leaf& leaf = bsp->leaves[i];

      for( int j = 0; j < leaf.nFaces; ++j ) {
        int iFace = bsp->leafFaces[leaf.firstFace + j];

        if( !drawnFaces.get( iFace ) ) {
          drawFace( &bsp->faces[iFace] );
          drawnFaces.set( iFace );
        }
      }
    }
    glEndList();

    return list;
  }

  void BSP::beginRender()
  {
    waterFlags = 0;

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
