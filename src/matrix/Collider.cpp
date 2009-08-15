/*
 *  Collider.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Collider.h"

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

  Vec3 Collider::toStructCS( const Vec3 &v ) const
  {
    switch( str->rot ) {
      case Structure::R0: {
        return v;
      }
      case Structure::R90: {
        return Vec3( -v.y, v.x, v.z );
      }
      case Structure::R180: {
        return Vec3( -v.x, -v.y, v.z );
      }
      default:
      case Structure::R270: {
        return Vec3( v.y, -v.x, v.z );
      }
    }
  }

  Vec3 Collider::toAbsoluteCS( const Vec3 &v ) const
  {
    switch( str->rot ) {
      case Structure::R0: {
        return v;
      }
      case Structure::R90: {
        return Vec3( v.y, -v.x, v.z );
      }
      case Structure::R180: {
        return Vec3( -v.x, -v.y, v.z );
      }
      default:
      case Structure::R270: {
        return Vec3( -v.y, v.x, v.z );
      }
    }
  }

  //***********************************
  //*    POINT COLLISION DETECTION    *
  //***********************************

  // checks if AABB and Brush overlap
  bool Collider::testPointBrush( const BSP::Brush *brush )
  {
    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > EPSILON ) {
        return true;
      }
    }
    return false;
  }

  // recursively check nodes of BSP-tree for AABB-Brush overlapping
  bool Collider::testPointNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; i++ ) {
        BSP::Brush &brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( ( brush.content & BSP::SOLID_BIT ) && !testPointBrush( &brush ) ) {
          return false;
        }
      }
      return true;
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > 2.0f * EPSILON ) {
        return testPointNode( node.front );
      }
      else if( dist < -2.0f * EPSILON ) {
        return testPointNode( node.back );
      }
      else {
        return testPointNode( node.front ) && testPointNode( node.back );
      }
    }
  }

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::testPointWorld()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return false;
    }

    if( aabb.p.z - aabb.dim.z - world.terrain.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
      return false;
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->includes( point, EPSILON ) )
          {
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

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        const Sector &sector = world.sectors[x][y];

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->includes( point, EPSILON ) )
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::testPointWorldOSO()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return false;
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->includes( point, EPSILON ) )
          {
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

    float pX = (float) ( ( x - Terrain::MAX / 2 ) * TerraQuad::SIZE );
    float pY = (float) ( ( y - Terrain::MAX / 2 ) * TerraQuad::SIZE );

    float pX0 = pX - EPSILON;
    float pY0 = pY - EPSILON;

    float pX1 = pX + TerraQuad::SIZE + EPSILON;
    float pY1 = pY + TerraQuad::SIZE + EPSILON;

    float startDist = globalStartPos * quad.normal[0] - quad.distance[0];
    float endDist   = globalEndPos   * quad.normal[0] - quad.distance[0];

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = startDist / ( startDist - endDist + EPSILON );

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - pX <= impactY - pY0 &&
          pX0 <= impactX && impactX <= pX1 &&
          pY0 <= impactY && impactY <= pY1 &&
          Math::abs( ratio ) < Math::abs( hit.ratio ) )
      {
        hit.ratio   = ratio;
        hit.normal  = quad.normal[0];
        hit.obj     = null;
        hit.onSlick = false;

        return false;
      }
    }

    startDist = globalStartPos * quad.normal[1] - quad.distance[1];
    endDist   = globalEndPos   * quad.normal[1] - quad.distance[1];

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = startDist / ( startDist - endDist + EPSILON );

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - pX0 >= impactY - pY &&
          pX0 <= impactX && impactX <= pX1 &&
          pY0 <= impactY && impactY <= pY1 &&
          Math::abs( ratio ) < Math::abs( hit.ratio ) )
      {
        hit.ratio   = ratio;
        hit.normal  = quad.normal[1];
        hit.obj     = null;
        hit.onSlick = false;

        return false;
      }
    }
    return true;
  }

  // finds out if Ray-World bounding box collision occurs and the time when it occurs
  void Collider::trimPointVoid()
  {
    float  minRatio       = -1.0f;
    float  maxRatio       =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 3; i++ ) {
      int  iPos    = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iPos];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio   = minRatio;
      hit.normal  = *tmpNormal;
      hit.obj     = null;
    }
  }

  // finds out if Ray-AABB collision occurs and the time when it occurs
  void Collider::trimPointObj( Object *sObj )
  {
    float  minRatio       = -1.0f;
    float  maxRatio       =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int  j = i >> 1;
      const Vec3 &normal = bbNormals[i];

      float startDist = ( globalStartPos[j] - sObj->p[j] ) * normal[j] - sObj->dim[j];
      float endDist   = ( globalEndPos[j]   - sObj->p[j] ) * normal[j] - sObj->dim[j];

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio  = minRatio;
      hit.normal = *tmpNormal;
      hit.obj    = sObj;
    }
  }

  // finds out if Ray-Brush collision occurs and the time when it occurs
  void Collider::trimPointBrush( const BSP::Brush *brush )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float startDist = leafStartPos * plane.normal - plane.distance;
      float endDist   = leafEndPos   * plane.normal - plane.distance;

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &plane.normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < maxRatio ) {
      float newRatio = leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio );

      if( newRatio < hit.ratio ) {
        hit.ratio  = newRatio;
        hit.normal = toAbsoluteCS( *tmpNormal );
        hit.obj    = null;
      }
    }
  }

  // recursively check nodes of BSP-tree for AABB-Brush collisions
  void Collider::trimPointNode( int nodeIndex, float startRatio, float endRatio,
                                const Vec3 &startPos, const Vec3 &endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nBrushes; i++ ) {
        BSP::Brush &brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( brush.content & BSP::SOLID_BIT ) {
          trimPointBrush( &brush );
        }
      }
    }
    else {
      BSP::Node  &node  = bsp->nodes[nodeIndex];
      BSP::Plane &plane = bsp->planes[node.plane];

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

      float offset = 2.0f * EPSILON;

      if( startDist > offset && endDist > offset ) {
        trimPointNode( node.front, startRatio, endRatio, startPos, endPos );
      }
      else if( startDist < -offset && endDist < -offset ) {
        trimPointNode( node.back, startRatio, endRatio, startPos, endPos );
      }
      else {
        if( startDist < endDist ) {
          float invDist = 1.0f / ( startDist - endDist );

          float ratio1 = min( ( startDist - offset ) * invDist, 1.0f );
          float ratio2 = max( ( startDist + offset ) * invDist, 0.0f );

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

          float ratio1 = min( ( startDist + offset ) * invDist, 1.0f );
          float ratio2 = max( ( startDist - offset ) * invDist, 0.0f );

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

    world.terrain.getInters( minPosX, minPosY, maxPosX, maxPosY, EPSILON );

    for( int x = world.terrain.minX; x <= world.terrain.maxX; x++ ) {
      for( int y = world.terrain.minY; y <= world.terrain.maxY; y++ ) {
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

    globalStartPos = point;
    globalEndPos   = point + move;

    if( !world.includes( trace, EPSILON ) ) {
      trimPointVoid();
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace, EPSILON ) ) {
              trimPointNode( 0, 0.0f, 1.0f,
                             toStructCS( globalStartPos - str->p ),
                             toStructCS( globalEndPos - str->p ) );
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace, EPSILON ) )
          {
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

  // checks if AABB and Brush overlap
  bool Collider::testAABBBrush( const BSP::Brush *brush )
  {
    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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

  // recursively check nodes of BSP-tree for AABB-Brush overlapping
  bool Collider::testAABBNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; i++ ) {
        BSP::Brush &brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( ( brush.content & BSP::SOLID_BIT ) && !testAABBBrush( &brush ) ) {
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
          2.0f * EPSILON;

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

  // check for AABB-AABB, AABB-Brush and AABB-Terrain overlapping in the world
  bool Collider::testAABBWorld()
  {
    if( !world.includes( aabb, EPSILON ) ) {
      return false;
    }

    if( aabb.p.z - aabb.dim.z - world.terrain.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
      return false;
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

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

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

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

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::testAABBWorldOSO()
  {
    if( !world.includes( aabb, EPSILON ) ) {
      return false;
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

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
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 3; i++ ) {
      int  iPos    = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iPos];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i] - aabb.dim[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio   = minRatio;
      hit.normal  = *tmpNormal;
      hit.obj     = null;
      hit.onSlick = false;
    }
  }

  // finds out if AABB-AABB collision occurs and the time when it occurs
  void Collider::trimAABBObj( Object *sObj )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int  j = i >> 1;
      const Vec3 &normal = bbNormals[i];

      float startDist = ( globalStartPos[j] - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];
      float endDist   = ( globalEndPos[j]   - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio   = minRatio;
      hit.normal  = *tmpNormal;
      hit.obj     = sObj;
      hit.onSlick = false;
    }
  }

  // finds out if AABB-Brush collision occurs and the time when it occurs
  void Collider::trimAABBBrush( const BSP::Brush *brush )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float startDist = leafStartPos * plane.normal - plane.distance - offset;
      float endDist   = leafEndPos   * plane.normal - plane.distance - offset;

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &plane.normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < maxRatio ) {
      float newRatio = leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio );

      if( newRatio < hit.ratio ) {
        hit.ratio   = newRatio;
        hit.normal  = toAbsoluteCS( *tmpNormal );
        hit.obj     = null;
        hit.onSlick = ( brush->content & BSP::SLICK_BIT ) != 0;
      }
    }
  }

  // checks if AABB and Brush overlap and if AABB center is inside a brush
  void Collider::trimAABBWater( const BSP::Brush *brush )
  {
    bool isInside  = true;
    bool isUnder   = true;

    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float centerDist = leafEndPos * plane.normal - plane.distance;
      float outerDist  = centerDist - offset;
      float innerDist  = centerDist + offset;

      if( outerDist > 0.0f ) {
        return;
      }
      else if( centerDist > 0.0f ) {
        isInside = false;
        isUnder  = false;
      }
      else if( innerDist > 0.0f ) {
        isUnder = false;
      }
    }
    hit.onWater    |= true;
    hit.inWater    |= isInside;
    hit.underWater |= isUnder;
  }

  // checks if AABB and Brush overlap and if AABB center is inside a brush
  void Collider::trimAABBLadder( const BSP::Brush *brush )
  {
    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float dist = leafStartPos * plane.normal - plane.distance - offset;

      if( dist > 0.0f ) {
        return;
      }
    }
    hit.onLadder |= true;
  }

  // recursively check nodes of BSP-tree for AABB-Brush collisions
  void Collider::trimAABBNode( int nodeIndex, float startRatio, float endRatio,
                               const Vec3 &startPos, const Vec3 &endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf &leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nBrushes; i++ ) {
        BSP::Brush &brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( brush.content & BSP::SOLID_BIT ) {
          trimAABBBrush( &brush );
        }
        else if( brush.content & BSP::WATER_BIT ) {
          trimAABBWater( &brush );
        }
        else if( ( brush.content & BSP::LADDER_BIT ) &&
                 obj != null && ( obj->flags & Object::CLIMBER_BIT ) )
        {
          trimAABBLadder( &brush );
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
    hit.ratio      = 1.0f;
    hit.obj        = null;
    hit.onWater    = false;
    hit.inWater    = false;
    hit.underWater = false;
    hit.onLadder   = false;
    hit.onSlick    = false;

    globalStartPos = aabb.p;
    globalEndPos   = aabb.p + move;

    if( !world.includes( trace, EPSILON ) ) {
      trimAABBVoid();
    }

    const Structure *oldStr = null;

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        foreach( strIndex, sector.structures.iterator() ) {
          str = world.structures[*strIndex];

          // to prevent some of duplicated structure tests
          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace, EPSILON ) ) {
              trimAABBNode( 0, 0.0f, 1.0f,
                            toStructCS( globalStartPos - str->p ),
                            toStructCS( globalEndPos - str->p ) );
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

    hit.ratio = bound( hit.ratio, 0.0f, 1.0f );
  }

  //***********************************
  //*          OVERLAPPING            *
  //***********************************

  // get all objects and structures that overlap with our trace
  void Collider::getWorldOverlaps( Vector<Object*> *objects, Vector<const Structure*> *structs )
  {
    assert( objects != null || structs != null );

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        if( structs != null ) {
          foreach( strIndex, sector.structures.iterator() ) {
            str = world.structures[*strIndex];

            if( !structs->contains( str ) ) {
              bsp = world.bsps[str->bsp];

              globalStartPos = toStructCS( aabb.p - str->p );

              if( str->overlaps( aabb ) && !testAABBNode( 0 ) ) {
                structs->add( str );
              }
            }
          }
        }

        if( objects != null ) {
          foreach( sObj, sector.objects.iterator() ) {
            if( sObj->overlaps( aabb ) ) {
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

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {

        Sector &sector = world.sectors[x][y];

        if( objects != null ) {
          foreach( sObj, sector.objects.iterator() ) {
            if( aabb.includes( sObj->p ) ) {
              *objects << sObj;
            }
          }
        }
      }
    }
  }

}
