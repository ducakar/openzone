/*
 *  Collider.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Collider.hpp"

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

  const Mat44 Collider::structRotations[] =
  {
    Mat44::id(),
    Mat44::rotZ(  Math::PI_2 ),
    Mat44::rotZ(  Math::PI ),
    Mat44::rotZ( -Math::PI_2 )
  };

  const Mat44 Collider::structInvRotations[] =
  {
    Mat44::id(),
    Mat44::rotZ( -Math::PI_2 ),
    Mat44::rotZ(  Math::PI ),
    Mat44::rotZ(  Math::PI_2 )
  };

  Collider::Collider() : mask( Object::SOLID_BIT )
  {}

  inline bool Collider::visitBrush( int index )
  {
    bool isTested = visitedBrushes.get( index );
    visitedBrushes.set( index );
    return isTested;
  }

  inline Vec3 Collider::toStructCS( const Vec3& v ) const
  {
    return structRotations[ int( str->rot ) ] * v;
  }

  inline Vec3 Collider::toAbsoluteCS( const Vec3& v ) const
  {
    return structInvRotations[ int( str->rot ) ] * v;
  }

  //***********************************
  //*    POINT COLLISION DETECTION    *
  //***********************************

  // checks if AABB and Brush overlap
  bool Collider::overlapsPointBrush( const BSP::Brush* brush ) const
  {
    bool result = true;

    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float dist = startPos * plane.normal - plane.distance;

      result &= dist <= EPSILON;
    }
    return result;
  }

  // recursively check nodes of BSP-tree for AABB-Brush overlapping
  bool Collider::overlapsPointNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        int index = bsp->leafBrushes[leaf.firstBrush + i];
        const BSP::Brush& brush = bsp->brushes[index];

        if( !visitBrush( index ) && ( brush.material & Material::STRUCT_BIT ) &&
            overlapsPointBrush( &brush ) )
        {
          return true;
        }
      }
      return false;
    }
    else {
      const BSP::Node&  node  = bsp->nodes[nodeIndex];
      const BSP::Plane& plane = bsp->planes[node.plane];

      float dist = startPos * plane.normal - plane.distance;

      if( dist > 2.0f * EPSILON ) {
        return overlapsPointNode( node.front );
      }
      else if( dist < -2.0f * EPSILON ) {
        return overlapsPointNode( node.back );
      }
      else {
        return overlapsPointNode( node.front ) || overlapsPointNode( node.back );
      }
    }
  }

  bool Collider::overlapsPointModels() const
  {
    for( int i = 1; i < bsp->nModels; ++i ) {
      const BSP::Model& model = bsp->models[i];

      for( int j = 0; j < model.nBrushes; ++j ) {
        int index = model.firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        assert( !visitedBrushes.get( index ) );

        if( ( brush.material & Material::STRUCT_BIT ) && overlapsPointBrush( &brush ) ) {
          return true;
        }
      }
    }
    return false;
  }

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::overlapsPointOrbis()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return true;
    }

    if( aabb.p.z - aabb.dim.z - world.terra.height( aabb.p.x, aabb.p.y ) <= 0.0f ) {
      return true;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            startPos = toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsPointNode( 0 ) || overlapsPointModels() ) ) {
              return true;
            }
            oldStr = str;
          }
        }

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->includes( point, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool Collider::overlapsPointOrbisOO()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return true;
    }

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->includes( point, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::overlapsPointOrbisOSO()
  {
    if( !world.includes( point, -EPSILON ) ) {
      return true;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            startPos = toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsPointNode( 0 ) || overlapsPointModels() ) ) {
              return true;
            }
            oldStr = str;
          }
        }

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->includes( point, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  // terrain collision detection is penetration-safe
  bool Collider::trimTerraQuad( int x, int y )
  {
    const Terra::Quad& quad = world.terra.quads[x][y];
    const Vec3& minVert = world.terra.vertices[x    ][y    ];
    const Vec3& maxVert = world.terra.vertices[x + 1][y + 1];

    float startDist = startPos * quad.tri[0].normal - quad.tri[0].distance;
    float endDist   = endPos   * quad.tri[0].normal - quad.tri[0].distance;

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = Math::max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = startPos.x + ratio * move.x;
      float impactY = startPos.y + ratio * move.y;

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

    startDist = startPos * quad.tri[1].normal - quad.tri[1].distance;
    endDist   = endPos   * quad.tri[1].normal - quad.tri[1].distance;

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = Math::max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = startPos.x + ratio * move.x;
      float impactY = startPos.y + ratio * move.y;

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
    if( startPos.z < 0.0f ) {
      hit.waterDepth = Math::max( hit.waterDepth, -startPos.z );
      hit.inWater = true;
    }

    float minPosX = Math::min( startPos.x, endPos.x );
    float minPosY = Math::min( startPos.y, endPos.y );
    float maxPosX = Math::max( startPos.x, endPos.x );
    float maxPosY = Math::max( startPos.y, endPos.y );

    Span terraSpan = world.terra.getInters( minPosX, minPosY, maxPosX, maxPosY );

    for( int x = terraSpan.minX; x <= terraSpan.maxX; ++x ) {
      for( int y = terraSpan.minY; y <= terraSpan.maxY; ++y ) {
        trimTerraQuad( x, y );
      }
    }
  }

  // finds out if Point-Orbis bounding box collision occurs and the time when it occurs
  void Collider::trimPointVoid()
  {
    for( int i = 0; i < 3; ++i ) {
      int iSide = move[i] >= 0.0f;
      const Vec3& normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + startPos[i] * normal[i];
      float endDist   = world.maxs[i] + endPos[i]   * normal[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = Math::max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

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
#ifdef OZ_ENABLE_CYLINDER

    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < 6; ++i ) {
      int j = i / 2;
      const Vec3& normal = bbNormals[i];

      float startDist = ( startPos[j] - sObj->p[j] ) * normal[j] - sObj->dim[j];
      float endDist   = ( endPos[j]   - sObj->p[j] ) * normal[j] - sObj->dim[j];

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
      hit.ratio    = Math::max( 0.0f, minRatio );
      hit.normal   = *tmpNormal;
      hit.obj      = sObj;
      hit.str      = null;
      hit.material = Material::OBJECT_BIT;
    }

#else

    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < 6; ++i ) {
      int j = i / 2;
      const Vec3& normal = bbNormals[i];

      float startDist = ( startPos[j] - sObj->p[j] ) * normal[j] - sObj->dim[j];
      float endDist   = ( endPos[j]   - sObj->p[j] ) * normal[j] - sObj->dim[j];

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
      hit.ratio    = Math::max( 0.0f, minRatio );
      hit.normal   = *tmpNormal;
      hit.obj      = sObj;
      hit.str      = null;
      hit.material = Material::OBJECT_BIT;
    }

#endif
  }

  // finds out if Ray-Brush collision occurs and the time when it occurs
  void Collider::trimPointBrush( const BSP::Brush* brush )
  {
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

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
      if( minRatio < hit.ratio ) {
        hit.ratio    = Math::max( 0.0f, minRatio );
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.str      = str;
        hit.material = brush->material;
      }
    }
  }

  // recursively check nodes of BSP-tree for Point-Brush collisions
  void Collider::trimPointNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        int index = bsp->leafBrushes[leaf.firstBrush + i];
        const BSP::Brush& brush = bsp->brushes[index];

        if( !visitBrush( index ) && ( brush.material & Material::STRUCT_BIT ) ) {
          trimPointBrush( &brush );
        }
      }
    }
    else {
      const BSP::Node&  node  = bsp->nodes[nodeIndex];
      const BSP::Plane& plane = bsp->planes[node.plane];

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

      float offset = 2.0f * EPSILON;

      if( startDist > offset && endDist > offset ) {
        trimPointNode( node.front );
      }
      else if( startDist < -offset && endDist < -offset ) {
        trimPointNode( node.back );
      }
      else {
        trimPointNode( node.front );
        trimPointNode( node.back );
      }
    }
  }

  void Collider::trimPointModels()
  {
    for( int i = 1; i < bsp->nModels; ++i ) {
      const BSP::Model& model = bsp->models[i];

      for( int j = 0; j < model.nBrushes; ++j ) {
        int index = model.firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        assert( !visitBrush( index ) );

        trimPointBrush( &brush );
      }
    }
  }

  void Collider::trimPointOrbis()
  {
    hit.ratio    = 1.0f;
    hit.obj      = null;
    hit.str      = null;
    hit.material = 0;

    Vec3 originalStartPos = point;
    Vec3 originalEndPos   = point + move;

    startPos = originalStartPos;
    endPos   = originalEndPos;

    if( !world.includes( trace ) ) {
      trimPointVoid();
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace ) ) {
              visitedBrushes.clearAll();

              startPos = toStructCS( originalStartPos - str->p );
              endPos   = toStructCS( originalEndPos - str->p );

              trimPointNode( 0 );
              trimPointModels();
            }
            oldStr = str;
          }
        }

        startPos = originalStartPos;
        endPos   = originalEndPos;

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) &&
              sObj->overlaps( trace ) )
          {
            trimPointObj( &*sObj );
          }
        }
      }
    }

    trimPointTerra();

    assert( 0.0f <= hit.ratio && hit.ratio <= 1.0f );
    assert( ( ( hit.material & Material::OBJECT_BIT ) != 0 ) == ( hit.obj != null ) );
  }

  //***********************************
  //*    AABB COLLISION DETECTION     *
  //***********************************

  // checks if AABB and Brush overlap
  bool Collider::overlapsAABBBrush( const BSP::Brush* brush ) const
  {
    bool result = true;

    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float dist = startPos * plane.normal - plane.distance - offset;

      result &= dist <= EPSILON;
    }
    return result;
  }

  // recursively check nodes of BSP-tree for AABB-Brush overlapping
  bool Collider::overlapsAABBNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        int index = bsp->leafBrushes[leaf.firstBrush + i];
        const BSP::Brush& brush = bsp->brushes[index];

        if( !visitBrush( index ) && ( brush.material & Material::STRUCT_BIT ) &&
            overlapsAABBBrush( &brush ) )
        {
          return true;
        }
      }
      return false;
    }
    else {
      const BSP::Node&  node  = bsp->nodes[nodeIndex];
      const BSP::Plane& plane = bsp->planes[node.plane];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z ) +
          2.0f * EPSILON;

      float dist = startPos * plane.normal - plane.distance;

      if( dist > offset ) {
        return overlapsAABBNode( node.front );
      }
      else if( dist < -offset ) {
        return overlapsAABBNode( node.back );
      }
      else {
        return overlapsAABBNode( node.front ) || overlapsAABBNode( node.back );
      }
    }
  }

  bool Collider::overlapsAABBModels() const
  {
    for( int i = 1; i < bsp->nModels; ++i ) {
      const BSP::Model& model = bsp->models[i];

      for( int j = 0; j < model.nBrushes; ++j ) {
        int index = model.firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        assert( !visitedBrushes.get( index ) );

        if( ( brush.material & Material::STRUCT_BIT ) && overlapsAABBBrush( &brush ) ) {
          return true;
        }
      }
    }
    return false;
  }

  // check for AABB-AABB, AABB-Brush and AABB-Terrain overlapping in the world
  bool Collider::overlapsAABBOrbis()
  {
    if( !world.includes( aabb, -EPSILON ) ) {
      return true;
    }

    if( aabb.p.z - aabb.dim.z - world.terra.height( aabb.p.x, aabb.p.y ) <= 0.0f ) {
      return true;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            startPos = toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBModels() ) ) {
              return true;
            }
            oldStr = str;
          }
        }

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->overlaps( aabb, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  // check for AABB-AABB overlapping in the world
  bool Collider::overlapsAABBOrbisOO()
  {
    if( !world.includes( aabb, -EPSILON ) ) {
      return true;
    }

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->overlaps( aabb, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  // check for AABB-AABB and AABB-Brush overlapping in the world
  bool Collider::overlapsAABBOrbisOSO()
  {
    if( !world.includes( aabb, -EPSILON ) ) {
      return true;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            startPos = toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBModels() ) ) {
              return true;
            }
            oldStr = str;
          }
        }

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->overlaps( aabb, EPSILON ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  // finds out if AABB-Orbis bounding box collision occurs and the time when it occurs
  void Collider::trimAABBVoid()
  {
    for( int i = 0; i < 3; ++i ) {
      int iSide = move[i] >= 0.0f;
      const Vec3& normal = bbNormals[i * 2 + iSide];

      float startDist = world.maxs[i] + startPos[i] * normal[i] - aabb.dim[i];
      float endDist   = world.maxs[i] + endPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = Math::max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

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
      int j = i / 2;
      const Vec3& normal = bbNormals[i];

      float startDist = ( startPos[j] - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];
      float endDist   = ( endPos[j]   - sObj->p[j] ) * normal[j] - aabb.dim[j] - sObj->dim[j];

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
      hit.ratio    = Math::max( 0.0f, minRatio );
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
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float startDist = startPos * plane.normal - plane.distance - offset;
      float endDist   = endPos   * plane.normal - plane.distance - offset;

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
      if( minRatio < hit.ratio ) {
        hit.ratio    = Math::max( 0.0f, minRatio );
        hit.normal   = toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.str      = str;
        hit.material = brush->material;
      }
    }
  }

  // checks if AABB and Brush overlap and if AABB centre is inside a brush
  void Collider::trimAABBWater( const BSP::Brush* brush )
  {
    float depth = Math::inf();

    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      if( plane.normal.z <= 0.0f ) {
        float centreDist = startPos * plane.normal - plane.distance;

        if( centreDist > -EPSILON ) {
          return;
        }
      }
      else {
        float dist = ( plane.distance - plane.normal.x*startPos.x + plane.normal.y*startPos.y ) /
            plane.normal.z - startPos.z + aabb.dim.z;

        if( dist < 0.0f ) {
          return;
        }
        else {
          depth = Math::min( depth, dist );
        }
      }
    }
    hit.waterDepth = Math::max( hit.waterDepth, depth );
    hit.inWater    = true;
  }

  // checks if AABB and Brush overlap and if AABB centre is inside a brush
  void Collider::trimAABBLadder( const BSP::Brush* brush )
  {
    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z );

      float dist = startPos * plane.normal - plane.distance - offset;

      if( dist > 0.0f ) {
        return;
      }
    }
    hit.onLadder = true;
  }

  // recursively check nodes of BSP-tree for AABB-Brush collisions
  void Collider::trimAABBNode( int nodeIndex )
  {
    if( nodeIndex < 0 ) {
      const BSP::Leaf& leaf = bsp->leaves[~nodeIndex];

      for( int i = 0; i < leaf.nBrushes; ++i ) {
        int index = bsp->leafBrushes[leaf.firstBrush + i];
        const BSP::Brush& brush = bsp->brushes[index];

        if( !visitBrush( index ) ) {
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
    }
    else {
      const BSP::Node&  node  = bsp->nodes[nodeIndex];
      const BSP::Plane& plane = bsp->planes[node.plane];

      float offset =
          Math::abs( plane.normal.x * aabb.dim.x ) +
          Math::abs( plane.normal.y * aabb.dim.y ) +
          Math::abs( plane.normal.z * aabb.dim.z ) +
          2.0f * EPSILON;

      float startDist = startPos * plane.normal - plane.distance;
      float endDist   = endPos   * plane.normal - plane.distance;

      if( startDist > offset && endDist > offset ) {
        trimAABBNode( node.front );
      }
      else if( startDist < -offset && endDist < -offset ) {
        trimAABBNode( node.back );
      }
      else {
        trimAABBNode( node.front );
        trimAABBNode( node.back );
      }
    }
  }

  void Collider::trimAABBModels()
  {
    for( int i = 1; i < bsp->nModels; ++i ) {
      const BSP::Model& model = bsp->models[i];

      for( int j = 0; j < model.nBrushes; ++j ) {
        int index = model.firstBrush + j;
        const BSP::Brush& brush = bsp->brushes[index];

        assert( !visitBrush( index ) );

        trimAABBBrush( &brush );
      }
    }
  }

  // move AABB unil first collisons occurs
  void Collider::trimAABBOrbis()
  {
    hit.ratio      = 1.0f;
    hit.obj        = null;
    hit.str        = null;
    hit.material   = 0;
    hit.waterDepth = 0.0f;
    hit.inWater    = false;
    hit.onLadder   = false;

    Vec3 originalStartPos = aabb.p;
    Vec3 originalEndPos   = aabb.p + move;

    startPos = originalStartPos;
    endPos   = originalEndPos;

    if( !world.includes( trace ) ) {
      trimAABBVoid();
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = world.structs[*strIndex];

          // to prevent some of duplicated structure tests
          if( str != oldStr ) {
            bsp = world.bsps[str->bsp];

            if( str->overlaps( trace ) ) {
              visitedBrushes.clearAll();

              startPos = toStructCS( originalStartPos - str->p );
              endPos   = toStructCS( originalEndPos - str->p );

              trimAABBNode( 0 );
              trimAABBModels();
            }
            oldStr = str;
          }
        }

        startPos = originalStartPos;
        endPos   = originalEndPos;

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj != exclObj && ( sObj->flags & mask ) &&
              sObj->overlaps( trace ) )
          {
            trimAABBObj( &*sObj );
          }
        }
      }
    }

    startPos.z -= aabb.dim.z;
    endPos.z   -= aabb.dim.z;

    trimPointTerra();

    assert( 0.0f <= hit.ratio && hit.ratio <= 1.0f );
    assert( ( ( hit.material & Material::OBJECT_BIT ) != 0 ) == ( hit.obj != null ) );
  }

  //***********************************
  //*          OVERLAPPING            *
  //***********************************

  // get all objects and structures that overlap with our trace
  void Collider::getOrbisOverlaps( Vector<Object*>* objects, Vector<Structure*>* structs )
  {
    assert( objects != null || structs != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        if( structs != null ) {
          foreach( strIndex, cell.structs.citer() ) {
            Structure* str = world.structs[*strIndex];

            if( !structs->contains( str ) ) {
              bsp = world.bsps[str->bsp];

              startPos = toStructCS( aabb.p - str->p );

              if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBModels() ) ) {
                structs->add( str );
              }
            }
          }
        }

        if( objects != null ) {
          for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
            if( sObj->overlaps( trace ) ) {
              objects->add( sObj );
            }
          }
        }
      }
    }
  }

  // get all objects which are included in our trace
  void Collider::getOrbisIncludes( Vector<Object*>* objects ) const
  {
    assert( objects != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( trace.includes( *sObj ) ) {
            objects->add( sObj );
          }
        }
      }
    }
  }

  void Collider::touchOrbisOverlaps() const
  {
    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( ( sObj->flags & Object::DYNAMIC_BIT ) && trace.overlaps( *sObj ) ) {
            // clearing these two bits should do
            sObj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
          }
        }
      }
    }
  }

  void Collider::getModelOverlaps( Vector<Object*>* objects )
  {
    assert( objects != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = world.cells[x][y];

        if( objects != null ) {
          for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
            aabb = *sObj;

            for( int i = 0; i < model->nBrushes; ++i ) {
              int index = model->firstBrush + i;
              const BSP::Brush& brush = bsp->brushes[index];

              if( overlapsAABBBrush( &brush ) ) {
                objects->add( sObj );
              }
            }
          }
        }
      }
    }
  }

}
