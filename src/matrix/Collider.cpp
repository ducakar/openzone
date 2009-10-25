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

  inline Vec3 Collider::toStructCS( const Vec3 &v ) const
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

  inline Vec3 Collider::toAbsoluteCS( const Vec3 &v ) const
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

        if( ( brush.material & Material::STRUCT_BIT ) && !testPointBrush( &brush ) ) {
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

    if( aabb.p.z - aabb.dim.z - world.terra.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
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
    const Terrain::Quad &quad = world.terra.quads[x][y];
    const Vec3 &minVert = world.terra.vertices[x    ][y    ];
    const Vec3 &maxVert = world.terra.vertices[x + 1][y + 1];

    float minX = minVert.x - EPSILON;
    float minY = minVert.y - EPSILON;
    float maxX = maxVert.x + EPSILON;
    float maxY = maxVert.y + EPSILON;

    float startDist = globalStartPos * quad.tri[0].normal - quad.tri[0].distance;
    float endDist   = globalEndPos   * quad.tri[0].normal - quad.tri[0].distance;

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - minX >= impactY - minY &&
          minX <= impactX && impactX <= maxX &&
          minY <= impactY && impactY <= maxY &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.tri[0].normal;
        hit.obj      = null;
        hit.material = Material::TERRAIN_BIT;

        return false;
      }
    }

    startDist = globalStartPos * quad.tri[1].normal - quad.tri[1].distance;
    endDist   = globalEndPos   * quad.tri[1].normal - quad.tri[1].distance;

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - minX <= impactY - minY &&
          minX <= impactX && impactX <= maxX &&
          minY <= impactY && impactY <= maxY &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.tri[1].normal;
        hit.obj      = null;
        hit.material = Material::TERRAIN_BIT;

        return false;
      }
    }
    return true;
  }

  void Collider::trimPointTerra()
  {
    if( globalStartPos.z < 0.0f ) {
      hit.waterDepth = max( hit.waterDepth, -globalStartPos.z );
      hit.inWater = true;
    }

    float minPosX = min( globalStartPos.x, globalEndPos.x );
    float minPosY = min( globalStartPos.y, globalEndPos.y );
    float maxPosX = max( globalStartPos.x, globalEndPos.x );
    float maxPosY = max( globalStartPos.y, globalEndPos.y );

    world.terra.getInters( minPosX, minPosY, maxPosX, maxPosY, EPSILON );

    for( int x = world.terra.minX; x <= world.terra.maxX; x++ ) {
      for( int y = world.terra.minY; y <= world.terra.maxY; y++ ) {
        trimTerraQuad( x, y );
      }
    }
  }

  // finds out if Ray-World bounding box collision occurs and the time when it occurs
  void Collider::trimPointVoid()
  {
    for( int i = 0; i < 3; i++ ) {
      int iSide = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio < hit.ratio ) {
          hit.ratio    = ratio;
          hit.normal   = normal;
          hit.obj      = null;
          hit.material = Material::VOID_BIT;
        }
      }
    }
  }

  // finds out if Ray-AABB collision occurs and the time when it occurs
  void Collider::trimPointObj( Object *sObj )
  {
    float  minRatio       = -1.0f;
    float  maxRatio       =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int j = i >> 1;
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
      hit.ratio    = minRatio;
      hit.normal   = *tmpNormal;
      hit.obj      = sObj->index < 0 ? null : sObj;
      hit.material = 0;
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
        hit.ratio    = newRatio;
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.material = brush->material;
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

        if( brush.material & Material::STRUCT_BIT ) {
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
//        if( startDist < endDist ) {
//          float invDist = 1.0f / ( startDist - endDist );
//
//          float ratio1 = min( ( startDist - offset ) * invDist, 1.0f );
//          float ratio2 = max( ( startDist + offset ) * invDist, 0.0f );
//
//          assert( 0.0f <= ratio1 && ratio1 <= 1.0f );
//          assert( 0.0f <= ratio2 && ratio2 <= 1.0f );
//
//          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
//          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );
//
//          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
//          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );
//
//          trimPointNode( node.back, startRatio, middleRatio1, startPos, middlePos1 );
//          trimPointNode( node.front, middleRatio2, endRatio, middlePos2, endPos );
//        }
//        else if( endDist < startDist ) {
//          float invDist = 1.0f / ( startDist - endDist );
//
//          float ratio1 = min( ( startDist + offset ) * invDist, 1.0f );
//          float ratio2 = max( ( startDist - offset ) * invDist, 0.0f );
//
//          assert( 0.0f <= ratio1 && ratio1 <= 1.0f );
//          assert( 0.0f <= ratio2 && ratio2 <= 1.0f );
//
//          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
//          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );
//
//          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
//          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );
//
//          trimPointNode( node.front, startRatio, middleRatio1, startPos, middlePos1 );
//          trimPointNode( node.back, middleRatio2, endRatio, middlePos2, endPos );
//        }
//        else {
          trimPointNode( node.front, startRatio, endRatio, startPos, endPos );
          trimPointNode( node.back, startRatio, endRatio, startPos, endPos );
//        }
      }
    }
  }

  void Collider::trimPointWorld()
  {
    hit.ratio    = 1.0f;
    hit.obj      = null;
    hit.material = 0;

    globalStartPos = point;
    globalEndPos   = point + move;

    if( !world.includes( trace ) ) {
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

            if( str->overlaps( trace ) ) {
              trimPointNode( 0, 0.0f, 1.0f,
                             toStructCS( globalStartPos - str->p ),
                             toStructCS( globalEndPos - str->p ) );
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
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

        if( ( brush.material & Material::STRUCT_BIT ) && !testAABBBrush( &brush ) ) {
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
    if( !world.includes( trace ) ) {
      return false;
    }

    if( aabb.p.z - aabb.dim.z - world.terra.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
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

            if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
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
    if( !world.includes( trace ) ) {
      return false;
    }

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {
        Sector &sector = world.sectors[x][y];

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
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
    if( !world.includes( trace ) ) {
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

            if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
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
      int iSide = move[i] >= 0.0f;
      const Vec3 &normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i] - aabb.dim[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio < hit.ratio ) {
          hit.ratio    = ratio;
          hit.normal   = normal;
          hit.obj      = null;
          hit.material = Material::VOID_BIT;
        }
      }
    }
  }

  // finds out if AABB-AABB collision occurs and the time when it occurs
  void Collider::trimAABBObj( Object *sObj )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3 *tmpNormal = null;

    for( int i = 0; i < 6; i++ ) {
      int j = i >> 1;
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
      hit.ratio    = minRatio;
      hit.normal   = *tmpNormal;
      hit.obj      = sObj->index < 0 ? null : sObj;
      hit.material = 0;
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
        hit.ratio    = newRatio;
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.material = brush->material;
      }
    }
  }

  // checks if AABB and Brush overlap and if AABB center is inside a brush
  void Collider::trimAABBWater( const BSP::Brush *brush )
  {
    float depth = Math::inf();

    for( int i = 0; i < brush->nSides; i++ ) {
      BSP::Plane &plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      if( plane.normal.z <= 0.0f ) {
        float centerDist = leafStartPos * plane.normal - plane.distance;

        if( centerDist > -EPSILON ) {
          return;
        }
      }
      else {
        float dist = ( plane.distance - plane.normal.x*leafStartPos.x +
            plane.normal.y*leafStartPos.y ) / plane.normal.z - leafStartPos.z + aabb.dim.z;

        if( dist <= 0.0f ) {
          return;
        }
        else {
          depth = min( depth, dist );
        }
      }
    }
    hit.waterDepth = max( hit.waterDepth, depth );
    hit.inWater    = true;
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
    hit.onLadder = true;
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

        if( brush.material & Material::STRUCT_BIT ) {
          trimAABBBrush( &brush );
        }
        else if( brush.material & Material::WATER_BIT ) {
          trimAABBWater( &brush );
        }
        else if( ( brush.material & Material::LADDER_BIT ) &&
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
//        if( startDist < endDist ) {
//          float invDist = 1.0f / ( startDist - endDist );
//
//          float ratio1 = bound( ( startDist - offset ) * invDist, 0.0f, 1.0f );
//          float ratio2 = bound( ( startDist + offset ) * invDist, 0.0f, 1.0f );
//
//          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
//          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );
//
//          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
//          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );
//
//          trimAABBNode( node.back, startRatio, middleRatio1, startPos, middlePos1 );
//          trimAABBNode( node.front, middleRatio2, endRatio, middlePos2, endPos );
//        }
//        else if( endDist < startDist ) {
//          float invDist = 1.0f / ( startDist - endDist );
//
//          float ratio1 = bound( ( startDist + offset ) * invDist, 0.0f, 1.0f );
//          float ratio2 = bound( ( startDist - offset ) * invDist, 0.0f, 1.0f );
//
//          float middleRatio1 = startRatio + ratio1 * ( endRatio - startRatio );
//          float middleRatio2 = startRatio + ratio2 * ( endRatio - startRatio );
//
//          Vec3  middlePos1 = startPos + ratio1 * ( endPos - startPos );
//          Vec3  middlePos2 = startPos + ratio2 * ( endPos - startPos );
//
//          trimAABBNode( node.front, startRatio, middleRatio1, startPos, middlePos1 );
//          trimAABBNode( node.back, middleRatio2, endRatio, middlePos2, endPos );
//        }
//        else {
          trimAABBNode( node.front, startRatio, endRatio, startPos, endPos );
          trimAABBNode( node.back, startRatio, endRatio, startPos, endPos );
//        }
      }
    }
  }

  // move AABB unil first collisons occurs
  void Collider::trimAABBWorld()
  {
    hit.ratio      = 1.0f;
    hit.material   = 0;
    hit.waterDepth = 0.0f;
    hit.obj        = null;
    hit.inWater    = false;
    hit.onLadder   = false;

    globalStartPos = aabb.p;
    globalEndPos   = aabb.p + move;

    if( !world.includes( trace ) ) {
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

            if( str->overlaps( trace ) ) {
              trimAABBNode( 0, 0.0f, 1.0f,
                            toStructCS( globalStartPos - str->p ),
                            toStructCS( globalEndPos - str->p ) );
            }
            oldStr = str;
          }
        }

        foreach( sObj, sector.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
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

              if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
                structs->add( str );
              }
            }
          }
        }

        if( objects != null ) {
          foreach( sObj, sector.objects.iterator() ) {
            if( sObj->overlaps( trace ) ) {
              *objects << &*sObj;
            }
          }
        }
      }
    }
  }

  // get all objects which are included in our trace
  void Collider::getWorldIncludes( Vector<Object*> *objects )
  {
    assert( objects != null );

    for( int x = world.minX; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {
        Sector &sector = world.sectors[x][y];

        if( objects != null ) {
          foreach( sObj, sector.objects.iterator() ) {
            if( trace.includes( *sObj ) ) {
              *objects << sObj;
            }
          }
        }
      }
    }
  }

}
