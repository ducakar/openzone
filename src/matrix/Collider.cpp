/*
 *  Collider.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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

  inline Vec3 Collider::toStructCS( const Vec3& v ) const
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

  inline Vec3 Collider::toAbsoluteCS( const Vec3& v ) const
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
  bool Collider::testPointBrush( const BSP::Brush* brush ) const
  {
    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float dist = globalStartPos * plane.normal - plane.distance;

      if( dist > EPSILON ) {
        return true;
      }
    }
    return false;
  }

  // recursively check nodes of BSP-tree for AABB-Brush overlapping
  bool Collider::testPointNode( int nodeIndex ) const
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf& leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        BSP::Brush& brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( ( brush.material & Material::STRUCT_BIT ) && !testPointBrush( &brush ) ) {
          return false;
        }
      }
      return true;
    }
    else {
      BSP::Node&  node  = bsp->nodes[nodeIndex];
      BSP::Plane& plane = bsp->planes[node.plane];

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

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.iterator() ) {
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

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( sObj, cell.objects.iterator() ) {
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

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->includes( point, EPSILON ) && !testPointNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.iterator() ) {
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

  // terrain collision detection is penetration-safe
  bool Collider::trimTerraQuad( int x, int y )
  {
    const Terrain::Quad& quad = world.terra.quads[x][y];
    const Vec3& minVert = world.terra.vertices[x    ][y    ];
    const Vec3& maxVert = world.terra.vertices[x + 1][y + 1];

    float startDist = globalStartPos * quad.tri[0].normal - quad.tri[0].distance;
    float endDist   = globalEndPos   * quad.tri[0].normal - quad.tri[0].distance;

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = globalStartPos.x + ratio * move.x;
      float impactY = globalStartPos.y + ratio * move.y;

      if( impactX - minVert.x >= impactY - minVert.y &&
          minVert.x <= impactX && impactX <= maxVert.x &&
          minVert.y <= impactY && impactY <= maxVert.y &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.tri[0].normal;
        hit.obj      = null;
        hit.str      = null;
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

      if( impactX - minVert.x <= impactY - minVert.y &&
          minVert.x <= impactX && impactX <= maxVert.x &&
          minVert.y <= impactY && impactY <= maxVert.y &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.tri[1].normal;
        hit.obj      = null;
        hit.str      = null;
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

    Span terraSpan;
    world.terra.getInters( terraSpan, minPosX, minPosY, maxPosX, maxPosY );

    for( int x = terraSpan.minX; x <= terraSpan.maxX; ++x ) {
      for( int y = terraSpan.minY; y <= terraSpan.maxY; ++y ) {
        trimTerraQuad( x, y );
      }
    }
  }

  // finds out if Point-World bounding box collision occurs and the time when it occurs
  void Collider::trimPointVoid()
  {
    for( int i = 0; i < 3; ++i ) {
      int iSide = move[i] >= 0.0f;
      const Vec3& normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio < hit.ratio ) {
          hit.ratio    = ratio;
          hit.normal   = normal;
          hit.obj      = null;
          hit.str      = null;
          hit.material = Material::VOID_BIT;
        }
      }
    }
  }

  // finds out if Point-AABB collision occurs and the time when it occurs
  void Collider::trimPointObj( const Object* sObj )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < 6; ++i ) {
      int j = i >> 1;
      const Vec3& normal = bbNormals[i];

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
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio    = max( 0.0f, minRatio );
      hit.normal   = *tmpNormal;
      hit.obj      = sObj;
      hit.str      = null;
      hit.material = Material::OBJECT_BIT;
    }
  }

  // finds out if Ray-Brush collision occurs and the time when it occurs
  void Collider::trimPointBrush( const BSP::Brush* brush )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &plane.normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < maxRatio ) {
      float newRatio = leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio );

      if( newRatio < hit.ratio ) {
        hit.ratio    = max( 0.0f, newRatio );
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.str      = str;
        hit.material = brush->material;
      }
    }
  }

  // recursively check nodes of BSP-tree for Point-Brush collisions
  void Collider::trimPointNode( int nodeIndex, float startRatio, float endRatio,
                                const Vec3& startPos, const Vec3& endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf& leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        BSP::Brush& brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( brush.material & Material::STRUCT_BIT ) {
          trimPointBrush( &brush );
        }
      }
    }
    else {
      BSP::Node&  node  = bsp->nodes[nodeIndex];
      BSP::Plane& plane = bsp->planes[node.plane];

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
        trimPointNode( node.front, startRatio, endRatio, startPos, endPos );
        trimPointNode( node.back, startRatio, endRatio, startPos, endPos );
      }
    }
  }

  void Collider::trimPointWorld()
  {
    hit.ratio    = 1.0f;
    hit.obj      = null;
    hit.str      = null;
    hit.material = 0;

    globalStartPos = point;
    globalEndPos   = point + move;

    if( !world.includes( trace ) ) {
      trimPointVoid();
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

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

        foreach( sObj, cell.objects.iterator() ) {
          if( sObj != exclObj && ( sObj->flags & Object::CLIP_BIT ) &&
              sObj->overlaps( trace ) )
          {
            trimPointObj( &*sObj );
          }
        }
      }
    }

    trimPointTerra();

    assert( 0.0f <= hit.ratio && hit.ratio <= 1.0f );
    assert( bool( hit.material & Material::OBJECT_BIT ) == ( hit.obj != null ) );
  }

  //***********************************
  //*    AABB COLLISION DETECTION     *
  //***********************************

  // checks if AABB and Brush overlap
  bool Collider::testAABBBrush( const BSP::Brush* brush ) const
  {
    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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
  bool Collider::testAABBNode( int nodeIndex ) const
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf& leaf = bsp->leafs[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        BSP::Brush& brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

        if( ( brush.material & Material::STRUCT_BIT ) && !testAABBBrush( &brush ) ) {
          return false;
        }
      }
      return true;
    }
    else {
      BSP::Node&  node  = bsp->nodes[nodeIndex];
      BSP::Plane& plane = bsp->planes[node.plane];

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

    if( aabb.p.z - aabb.dim.z - world.terra.height( aabb.p.x, aabb.p.y ) < 0.0f ) {
      return false;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.iterator() ) {
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

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( sObj, cell.objects.iterator() ) {
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

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            globalStartPos = toStructCS( aabb.p - str->p );

            if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
              return false;
            }
            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.iterator() ) {
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
    for( int i = 0; i < 3; ++i ) {
      int iSide = move[i] >= 0.0f;
      const Vec3& normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + globalStartPos[i] * normal[i] - aabb.dim[i];
      float endDist   = world.maxs[i] + globalEndPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio < hit.ratio ) {
          hit.ratio    = ratio;
          hit.normal   = normal;
          hit.obj      = null;
          hit.str      = null;
          hit.material = Material::VOID_BIT;
        }
      }
    }
  }

  // finds out if AABB-AABB collision occurs and the time when it occurs
  void Collider::trimAABBObj( const Object* sObj )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < 6; ++i ) {
      int j = i >> 1;
      const Vec3& normal = bbNormals[i];

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
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < hit.ratio && minRatio < maxRatio ) {
      hit.ratio    = max( 0.0f, minRatio );
      hit.normal   = *tmpNormal;
      hit.obj      = sObj;
      hit.str      = null;
      hit.material = Material::OBJECT_BIT;
    }
  }

  // finds out if AABB-Brush collision occurs and the time when it occurs
  void Collider::trimAABBBrush( const BSP::Brush* brush )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + EPSILON );

        if( ratio > minRatio ) {
          minRatio  = ratio;
          tmpNormal = &plane.normal;
        }
      }
    }
    if( minRatio != -1.0f && minRatio < maxRatio ) {
      float newRatio = leafStartRatio + minRatio * ( leafEndRatio - leafStartRatio );

      if( newRatio < hit.ratio ) {
        hit.ratio    = max( 0.0f, newRatio );
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.str      = str;
        hit.material = brush->material;
      }
    }
  }

  // checks if AABB and Brush overlap and if AABB center is inside a brush
  void Collider::trimAABBWater( const BSP::Brush* brush )
  {
    float depth = Math::inf();

    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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
  void Collider::trimAABBLadder( const BSP::Brush* brush )
  {
    for( int i = 0; i < brush->nSides; ++i ) {
      BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

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
                               const Vec3& startPos, const Vec3& endPos )
  {
    if( nodeIndex < 0 ) {
      BSP::Leaf& leaf = bsp->leafs[~nodeIndex];

      leafStartRatio = startRatio;
      leafEndRatio   = endRatio;

      leafStartPos = startPos;
      leafEndPos   = endPos;

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        BSP::Brush& brush = bsp->brushes[ bsp->leafBrushes[leaf.firstBrush + i] ];

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
      BSP::Node&  node  = bsp->nodes[nodeIndex];
      BSP::Plane& plane = bsp->planes[node.plane];

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
        trimAABBNode( node.front, startRatio, endRatio, startPos, endPos );
        trimAABBNode( node.back, startRatio, endRatio, startPos, endPos );
      }
    }
  }

  // move AABB unil first collisons occurs
  void Collider::trimAABBWorld()
  {
    hit.ratio      = 1.0f;
    hit.obj        = null;
    hit.str        = null;
    hit.material   = 0;
    hit.waterDepth = 0.0f;
    hit.inWater    = false;
    hit.onLadder   = false;

    globalStartPos = aabb.p;
    globalEndPos   = aabb.p + move;

    if( !world.includes( trace ) ) {
      trimAABBVoid();
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.iterator() ) {
          str = world.structs[*strIndex];

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

        foreach( sObj, cell.objects.iterator() ) {
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

    assert( 0.0f <= hit.ratio && hit.ratio <= 1.0f );
    assert( bool( hit.material & Material::OBJECT_BIT ) == ( hit.obj != null ) );
  }

  //***********************************
  //*          OVERLAPPING            *
  //***********************************

  // get all objects and structures that overlap with our trace
  void Collider::getWorldOverlaps( Vector<Object*>* objects, Vector<Structure*>* structs )
  {
    assert( objects != null || structs != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        if( structs != null ) {
          foreach( strIndex, cell.structs.iterator() ) {
            Structure* str = world.structs[*strIndex];

            if( !structs->contains( str ) ) {
              bsp = world.bsps[str->bsp];

              globalStartPos = toStructCS( aabb.p - str->p );

              if( str->overlaps( trace ) && !testAABBNode( 0 ) ) {
                *structs << str;
              }
            }
          }
        }

        if( objects != null ) {
          foreach( sObj, cell.objects.iterator() ) {
            if( sObj->overlaps( trace ) ) {
              *objects << sObj;
            }
          }
        }
      }
    }
  }

  // get all objects which are included in our trace
  void Collider::getWorldIncludes( Vector<Object*>* objects ) const
  {
    assert( objects != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( sObj, cell.objects.iterator() ) {
          if( trace.includes( *sObj ) ) {
            *objects << sObj;
          }
        }
      }
    }
  }

  void Collider::touchWorldOverlaps() const
  {
    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        Cell& cell = world.cells[x][y];

        foreach( sObj, cell.objects.iterator() ) {
          if( ( sObj->flags & Object::DYNAMIC_BIT ) && trace.overlaps( *sObj ) ) {
            // clearing these two bits should do
            sObj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
          }
        }
      }
    }
  }

}
