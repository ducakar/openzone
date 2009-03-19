/*
 *  Collider.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Collider.hpp"

namespace oz
{

  Collider collider;

  const Vec3 Collider::bbNormals[] =
  {
    Vec3(  1.0f,  0.0f,  0.0f ),
    Vec3( -1.0f,  0.0f,  0.0f ),
    Vec3(  0.0f,  1.0f,  0.0f ),
    Vec3(  0.0f, -1.0f,  0.0f ),
    Vec3(  0.0f,  0.0f,  1.0f ),
    Vec3(  0.0f,  0.0f, -1.0f )
  };

  //***********************************
  //*    POINT COLLISION DETECTION     *
  //***********************************

  // checks if AABB and Simplex overlap
  bool Collider::testPointSimplex( const BSP::Simplex *simplex )
  {
    for( int i = 0; i < simplex->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->simplexSides[simplex->firstSide + i] ];

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > EPSILON ) {
        return true;
      }
    }
    return false;
  }

  // recursively check nodes of BSP-tree for AABB-Simplex overlapping
  bool Collider::testPointNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nSimplexes; i++ ) {
        BSP::Simplex &simplex = bsp->simplices[ bsp->leafSimplices[leaf.firstSimplex + i] ];

        if( ( simplex.flags & BSP::COLLIDABLE_BIT ) && !testPointSimplex( &simplex ) ) {
          return false;
        }
      }
      return true;
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > EPSILON ) {
        return testPointNode( node.front );
      }
      else if( dist < -EPSILON ) {
        return testPointNode( node.back );
      }
      else {
        return testPointNode( node.front ) && testPointNode( node.back );
      }
    }
  }

  // check for AABB-AABB and AABB-Simplex overlapping in the world
  bool Collider::testPointWorld()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return false;
    }

    if( aabb.p.z - aabb.dim.z - world.terrain.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
      return false;
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = aabb.p - str->p;

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( ( sObj->flags & Object::CLIP_BIT ) && sObj->includes( point, EPSILON ) ) {
            return false;
          }
        }
      }
    }
    return true;
  }

  bool Collider::testPointWorldOO()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return false;
    }

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        const Sector &sector = world.sectors[x][y];

        foreach( sObj, sector.objects.iterator() ) {
          if( ( sObj->flags & Object::CLIP_BIT ) && sObj->includes( point, EPSILON ) ) {
            return false;
          }
        }
      }
    }
    return true;
  }

  // check for AABB-AABB and AABB-Simplex overlapping in the world
  bool Collider::testPointWorldOSO()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return false;
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = aabb.p - str->p;

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( ( sObj->flags & Object::CLIP_BIT ) && sObj->includes( point, EPSILON ) ) {
            return false;
          }
        }
      }
    }
    return true;
  }

  bool Collider::trimTerraQuad( int x, int y )
  {
    TerraQuad &quad = world.terrain.terra[x][y];

    float pX = (float)( ( x - Terrain::MAX / 2 ) * TerraQuad::SIZE );
    float pY = (float)( ( y - Terrain::MAX / 2 ) * TerraQuad::SIZE );

    float pX0 = pX - EPSILON;
    float pY0 = pY - EPSILON;

    float pX1 = pX + TerraQuad::SIZE + EPSILON;
    float pY1 = pY + TerraQuad::SIZE + EPSILON;

    float startDist = globalStartPos * quad.normal[0] - quad.distance[0];
    float endDist   = globalEndPos   * quad.normal[0] - quad.distance[0];

    if( startDist >= 0.0f && endDist <= EPSILON ) {
      float ratio =
          startDist > endDist ?
          max( startDist - EPSILON, 0.0f ) / ( startDist - endDist ) :
          0.0f;

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - pX <= impactY - pY0 &&
          pX0 <= impactX && impactX <= pX1 &&
          pY0 <= impactY && impactY <= pY1 &&
          ratio < hit.ratio )
      {
        hit.ratio  = ratio;
        hit.normal = quad.normal[0];
        hit.sObj   = null;

        return false;
      }
    }

    startDist = globalStartPos * quad.normal[1] - quad.distance[1];
    endDist   = globalEndPos   * quad.normal[1] - quad.distance[1];

    if( startDist >= 0.0f && endDist <= EPSILON ) {
      float ratio =
          startDist > endDist ?
          max( startDist - EPSILON, 0.0f ) / ( startDist - endDist ) :
          0.0f;

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - pX0 >= impactY - pY &&
          pX0 <= impactX && impactX <= pX1 &&
          pY0 <= impactY && impactY <= pY1 &&
          ratio < hit.ratio )
      {
        hit.ratio  = ratio;
        hit.normal = quad.normal[1];
        hit.sObj   = null;

        return false;
      }
    }
    return true;
  }

  // finds out if Ray-World bounding box collision occurs and the time when it occurs
  void Collider::trimPointVoid()
  {

    for( int i = 0; i < 3; i++ ) {
      int  iPos    = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iPos];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i];

      if( endDist <= EPSILON ) {
        if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );

          if( ratio < hit.ratio ) {
            hit.ratio  = ratio;
            hit.normal = normal;
            hit.sObj   = null;
          }
        }
        else {
          hit.ratio  = 0.0f;
          hit.normal = normal;
          hit.sObj   = null;
        }
      }
    }

    trace.fromPointMove( globalStartPos, move * hit.ratio, EPSILON );
  }

  // finds out if Ray-AABB collision occurs and the time when it occurs
  void Collider::trimPointObj( Object *sObj )
  {
    float  minRatio   = -1.0f;
    float  maxRatio   =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int  j = i >> 1;
      const Vec3 &normal = bbNormals[i];

      float startDist = ( globalStartPos[j] - sObj->p[j] ) * normal[j] - sObj->dim[j];
      float endDist   = ( globalEndPos[j]   - sObj->p[j] ) * normal[j] - sObj->dim[j];

      if( startDist >= 0.0f ) {
        if( endDist > EPSILON ) {
          return;
        }
        else if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );
          assert( !Math::isNaN( ratio ) );

          if( ratio > minRatio ) {
            minRatio  = ratio;
            tmpNormal = &normal;
          }
        }
        else {
          minRatio  = 0.0f;
          tmpNormal = &normal;
        }
      }
      else if( endDist > EPSILON ) {
        maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
      }
    }

    if( minRatio < hit.ratio && minRatio < maxRatio && minRatio != -1.0f ) {
      hit.ratio  = minRatio;
      hit.normal = *tmpNormal;
      hit.sObj   = sObj;

      trace.fromPointMove( globalStartPos, move * hit.ratio, EPSILON );
    }
  }

  // finds out if Ray-Simplex collision occurs and the time when it occurs
  void Collider::trimPointSimplex( const BSP::Simplex *simplex )
  {
    float minRatio   = -1.0f;
    float maxRatio   =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < simplex->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->simplexSides[simplex->firstSide + i] ];

      float startDist = leafStartPos * plane.normal - plane.distance;
      float endDist   = leafEndPos   * plane.normal - plane.distance;

      if( startDist >= 0.0f ) {
        if( endDist > EPSILON ) {
          return;
        }
        else if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );

          if( ratio > minRatio ) {
            minRatio  = ratio;
            tmpNormal = &plane.normal;
          }
        }
        else {
          minRatio  = 0.0f;
          tmpNormal = &plane.normal;
        }
      }
      else if( endDist > EPSILON ) {
        maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
      }
    }
    if( minRatio < maxRatio && minRatio != -1.0f ) {
      float newRatio = max( leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio ), 0.0f );

      if( newRatio < hit.ratio ) {
        hit.ratio  = newRatio;
        hit.normal = *tmpNormal;
        hit.sObj   = null;

        trace.fromPointMove( globalStartPos, move * hit.ratio, EPSILON );
      }
    }
  }

  // recursively check nodes of BSP-tree for AABB-Simplex collisions
  void Collider::trimPointNode( int nodeIndex, float startRatio, float endRatio,
                                const Vec3 &startPos, const Vec3 &endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nSimplexes; i++ ) {
        BSP::Simplex &simplex = bsp->simplices[ bsp->leafSimplices[leaf.firstSimplex + i] ];

        if( simplex.flags & BSP::COLLIDABLE_BIT ) {
          trimPointSimplex( &simplex );
        }
      }
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

      if( startDist > 0.0f && endDist > 0.0f ) {
        trimPointNode( node.front, startRatio, endRatio, startPos, endPos );
      }
      else if( startDist < 0.0f && endDist < 0.0f ) {
        trimPointNode( node.back, startRatio, endRatio, startPos, endPos );
      }
      else {
        if( startDist < endDist ) {
          float invDist = 1.0f / ( startDist - endDist );

          float ratio1 = min( ( startDist - 2.0f * EPSILON ) * invDist, 1.0f );
          float ratio2 = max( ( startDist + 2.0f * EPSILON ) * invDist, 0.0f );

          assert( 0.0f <= ratio1 && ratio1 <= 1.0f );
          assert( 0.0f <= ratio2 && ratio2 <= 1.0f );

          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );

          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );

          trimPointNode( node.back, startRatio, middleRatio1, startPos, middlePos1 );
          trimPointNode( node.front, middleRatio2, endRatio, middlePos2, endPos );
        }
        else if( endDist < startDist ) {
          float invDist = 1.0f / ( startDist - endDist );

          float ratio1 = min( ( startDist + 2.0f * EPSILON ) * invDist, 1.0f );
          float ratio2 = max( ( startDist - 2.0f * EPSILON ) * invDist, 0.0f );

          assert( 0.0f <= ratio1 && ratio1 <= 1.0f );
          assert( 0.0f <= ratio2 && ratio2 <= 1.0f );

          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );

          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );

          trimPointNode( node.front, startRatio, middleRatio1, startPos, middlePos1 );
          trimPointNode( node.back, middleRatio2, endRatio, middlePos2, endPos );
        }
        else {
          trimPointNode( node.front, startRatio, endRatio, startPos, endPos );
          trimPointNode( node.back, startRatio, endRatio, startPos, endPos );
        }
      }
    }
  }

  void Collider::trimPointTerra()
  {
    float minPosX, minPosY;
    float maxPosX, maxPosY;

    if( globalStartPos.x < globalEndPos.x ) {
      minPosX = globalStartPos.x;
      maxPosX = globalEndPos.x;
    }
    else {
      minPosX = globalEndPos.x;
      maxPosX = globalStartPos.x;
    }

    if( globalStartPos.y < globalEndPos.y ) {
      minPosY = globalStartPos.y;
      maxPosY = globalEndPos.y;
    }
    else {
      minPosY = globalEndPos.y;
      maxPosY = globalStartPos.y;
    }

    int minX = max( (int)( minPosX + Terrain::DIM - EPSILON ) / TerraQuad::SIZEI, 0 );
    int minY = max( (int)( minPosY + Terrain::DIM - EPSILON ) / TerraQuad::SIZEI, 0 );

    int maxX = min( (int)( maxPosX + Terrain::DIM + EPSILON ) / TerraQuad::SIZEI, Terrain::MAX - 1 );
    int maxY = min( (int)( maxPosY + Terrain::DIM + EPSILON ) / TerraQuad::SIZEI, Terrain::MAX - 1 );

    for( int x = minX; x <= maxX; x++ ) {
      for( int y = minY; y <= maxY; y++ ) {
        if( !trimTerraQuad( x, y ) ) {
          return;
        }
      }
    }
  }

  void Collider::trimPointWorld()
  {
    hit.ratio = 1.0f;
    hit.obj   = null;
    hit.sObj  = null;

    globalStartPos = point;
    globalEndPos   = point + move;

    if( !world.includes( trace, EPSILON ) ) {
      trimPointVoid();
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace, EPSILON ) ) {
              trimPointNode( 0, 0.0f, 1.0f, globalStartPos - str->p, globalEndPos - str->p );
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( ( sObj->flags & Object::CLIP_BIT ) && sObj->overlaps( trace, EPSILON ) ) {
            trimPointObj( &*sObj );
          }
        }
      }
    }

    trimPointTerra();
  }

  //***********************************
  //*    AABB COLLISION DETECTION     *
  //***********************************

  // checks if AABB and Simplex overlap
  bool Collider::testAABBSimplex( const BSP::Simplex *simplex )
  {
    for( int i = 0; i < simplex->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->simplexSides[simplex->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float dist = globalStartPos * plane.normal - plane.distance - offset;

      if( dist > EPSILON ) {
        return true;
      }
    }
    return false;
  }

  // recursively check nodes of BSP-tree for AABB-Simplex overlapping
  bool Collider::testAABBNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nSimplexes; i++ ) {
        BSP::Simplex &simplex = bsp->simplices[ bsp->leafSimplices[leaf.firstSimplex + i] ];

        if( ( simplex.flags & BSP::COLLIDABLE_BIT ) && !testAABBSimplex( &simplex ) ) {
          return false;
        }
      }
      return true;
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z ) +
          EPSILON;

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > offset ) {
        return testAABBNode( node.front );
      }
      else if( dist < -offset ) {
        return testAABBNode( node.back );
      }
      else {
        return testAABBNode( node.front ) && testAABBNode( node.back );
      }
    }
  }

  // check for AABB-AABB, AABB-Simplex and AABB-Terrain overlapping in the world
  bool Collider::testAABBWorld()
  {
    if( !world.includes( aabb, EPSILON ) ) {
      return false;
    }

    if( aabb.p.z - aabb.dim.z - world.terrain.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
      return false;
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = aabb.p - str->p;

            if( str->overlaps( aabb, EPSILON ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( aabb, EPSILON ) )
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  // check for AABB-AABB overlapping in the world
  bool Collider::testAABBWorldOO()
  {
    if( !world.includes( aabb, EPSILON ) ) {
      return false;
    }

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( aabb, EPSILON ) )
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  // check for AABB-AABB and AABB-Simplex overlapping in the world
  bool Collider::testAABBWorldOSO()
  {
    if( !world.includes( aabb, EPSILON ) ) {
      return false;
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = aabb.p - str->p;

            if( str->overlaps( aabb, EPSILON ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( aabb, EPSILON ) )
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  // finds out if AABB-World bounding box collision occurs and the time when it occurs
  void Collider::trimAABBVoid()
  {
    for( int i = 0; i < 3; i++ ) {
      int  iPos    = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iPos];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i] - aabb.dim[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON ) {
        if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );

          if( ratio < hit.ratio ) {
            hit.ratio  = ratio;
            hit.normal = normal;
            hit.sObj   = null;
          }
        }
        else {
          hit.ratio  = 0.0f;
          hit.normal = normal;
          hit.sObj   = null;
        }
      }
    }
  }

  // finds out if AABB-AABB collision occurs and the time when it occurs
  void Collider::trimAABBObj( Object *sObj )
  {
    float minRatio   = -1.0f;
    float maxRatio   =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int  j = i >> 1;
      const Vec3 &normal = bbNormals[i];

      float startDist = ( globalStartPos[j] - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];
      float endDist   = ( globalEndPos[j]   - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];

      if( startDist >= 0.0f ) {
        if( endDist > EPSILON ) {
          return;
        }
        else if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );

          if( ratio > minRatio ) {
            minRatio  = ratio;
            tmpNormal = &normal;
          }
        }
        else {
          minRatio  = 0.0f;
          tmpNormal = &normal;
        }
      }
      else if( endDist > EPSILON ) {
        maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
      }
    }

    if( minRatio < hit.ratio && minRatio < maxRatio && minRatio != -1.0f ) {
      hit.ratio  = minRatio;
      hit.normal = *tmpNormal;
      hit.sObj   = sObj;
    }
  }

  // finds out if AABB-Simplex collision occurs and the time when it occurs
  void Collider::trimAABBSimplex( const BSP::Simplex *simplex )
  {
    float minRatio   = -1.0f;
    float maxRatio   =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < simplex->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->simplexSides[simplex->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float startDist = leafStartPos * plane.normal - plane.distance - offset;
      float endDist   = leafEndPos   * plane.normal - plane.distance - offset;

      if( startDist >= 0.0f ) {
        if( endDist > EPSILON ) {
          return;
        }
        else if( startDist > endDist ) {
          float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist );

          if( ratio > minRatio ) {
            minRatio  = ratio;
            tmpNormal = &plane.normal;
          }
        }
        else {
          minRatio  = 0.0f;
          tmpNormal = &plane.normal;
        }
      }
      else if( endDist > EPSILON ) {
        maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
      }
    }
    if( minRatio < maxRatio && minRatio != -1.0f ) {
      float newRatio = max( leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio ), 0.0f );

      if( newRatio < hit.ratio ) {
        hit.ratio  = newRatio;
        hit.normal = *tmpNormal;
        hit.sObj   = null;
      }
    }
  }

  // recursively check nodes of BSP-tree for AABB-Simplex collisions
  void Collider::trimAABBNode( int nodeIndex, float startRatio, float endRatio,
      const Vec3 &startPos, const Vec3 &endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nSimplexes; i++ ) {
        BSP::Simplex &simplex = bsp->simplices[ bsp->leafSimplices[leaf.firstSimplex + i] ];

        if( simplex.flags & BSP::COLLIDABLE_BIT ) {
          trimAABBSimplex( &simplex );
        }
      }
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z ) +
          2.0f * EPSILON;

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

      if( startDist > offset && endDist > offset ) {
        trimAABBNode( node.front, startRatio, endRatio, startPos, endPos );
      }
      else if( startDist < -offset && endDist < -offset ) {
        trimAABBNode( node.back, startRatio, endRatio, startPos, endPos );
      }
      else {
        if( startDist < endDist ) {
          float invDist = 1.0f / ( startDist - endDist );

          float ratio1 = bound( ( startDist - offset ) * invDist, 0.0f, 1.0f );
          float ratio2 = bound( ( startDist + offset ) * invDist, 0.0f, 1.0f );

          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );

          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );

          trimAABBNode( node.back, startRatio, middleRatio1, startPos, middlePos1 );
          trimAABBNode( node.front, middleRatio2, endRatio, middlePos2, endPos );
        }
        else if( endDist < startDist ) {
          float invDist = 1.0f / ( startDist - endDist );

          float ratio1 = bound( ( startDist + offset ) * invDist, 0.0f, 1.0f );
          float ratio2 = bound( ( startDist - offset ) * invDist, 0.0f, 1.0f );

          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );

          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );

          trimAABBNode( node.front, startRatio, middleRatio1, startPos, middlePos1 );
          trimAABBNode( node.back, middleRatio2, endRatio, middlePos2, endPos );
        }
        else {
          trimAABBNode( node.front, startRatio, endRatio, startPos, endPos );
          trimAABBNode( node.back, startRatio, endRatio, startPos, endPos );
        }
      }
    }
  }

  // move AABB unil first collisons occurs
  void Collider::trimAABBWorld()
  {
    hit.ratio = 1.0f;
    hit.obj   = obj;
    hit.sObj  = null;

    globalStartPos = aabb.p;
    globalEndPos   = aabb.p + move;

    if( !world.includes( trace, EPSILON ) ) {
      trimAABBVoid();
    }

    Structure *oldStr = null;

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          Structure *str = world.structures[*strIndex];

          // to prevent some of duplicated structure tests
          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace, EPSILON ) ) {
              trimAABBNode( 0, 0.0f, 1.0f, globalStartPos - str->p, globalEndPos - str->p );
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace, EPSILON ) )
          {
            trimAABBObj( &*sObj );
          }
        }
      }
    }

    globalStartPos.z -= aabb.dim.z;
    globalEndPos.z   -= aabb.dim.z;

    trimPointTerra();
  }

  //***********************************
  //*          OVERLAPPING            *
  //***********************************

  // get all objects and structures that overlap with our trace
  void Collider::getWorldOverlaps( Vector<Object*> *objects, Vector<Structure*> *structs )
  {
    assert( objects != null || structs != null );

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        if( structs != null ) {
          foreach( strIndex, sector.structures.iterator() ) {
            Structure *str = world.structures[*strIndex];

            if( !structs->contains( str ) ) {
              bsp = world.bsps[str->bsp];

              globalStartPos = aabb.p - str->p;

              if( str->overlaps( aabb ) && !testAABBNode( 0 ) ) {
                structs->add( str );
              }
            }
          }
        }

        if( objects != null ) {
          foreach( sObj, sector.objects.iterator() ) {
            if( ( sObj->flags & Object::CLIP_BIT ) && sObj->overlaps( aabb ) ) {
              *objects << &*sObj;
            }
          }
        }
      }
    }
  }

  // get all objects which centres are included in our trace
  void Collider::getWorldIncludes( Vector<Object*> *objects )
  {
    assert( objects != null );

    for( int x = world.minSectX; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {

        Sector &sector = world.sectors[x][y];

        if( objects != null ) {
          for( Object *sObj = sector.objects.first(); sObj != null; sObj = sObj->next[0] ) {
            if( ( sObj->flags & Object::CLIP_BIT ) && aabb.includes( sObj->p ) ) {
              *objects << sObj;
            }
          }
        }
      }
    }
  }

}
