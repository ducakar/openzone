/*
 *  Collider.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Collider.hpp"

namespace oz
{

  Collider collider;

  const float Collider::LOCAL_EPS = 256.0f * Math::EPSILON;

  const Vec3 Collider::normals[] =
  {
    Vec3(  1.0f,  0.0f,  0.0f ),
    Vec3( -1.0f,  0.0f,  0.0f ),
    Vec3(  0.0f,  1.0f,  0.0f ),
    Vec3(  0.0f, -1.0f,  0.0f ),
    Vec3(  0.0f,  0.0f,  1.0f ),
    Vec3(  0.0f,  0.0f, -1.0f )
  };

  Collider::Collider() : mask( Object::SOLID_BIT )
  {}

  inline bool Collider::visitBrush( int index )
  {
    bool isTested = visitedBrushes.get( index );
    visitedBrushes.set( index );
    return isTested;
  }

  //***********************************
  //*         STATIC AABB CD          *
  //***********************************

  // checks if AABB and Object overlap
  bool Collider::overlapsAABBObj( const Object* sObj ) const
  {
    if( flags & sObj->flags & Object::CYLINDER_BIT ) {
      Vec3  relPos  = aabb.p - sObj->p;
      Vec3  sumDim  = aabb.dim + sObj->dim;
      float distXY2 = relPos.x*relPos.x + relPos.y*relPos.y;
      float radius  = sumDim.x + EPSILON;

      return distXY2 <= radius*radius &&
          relPos.z <= +sumDim.z + EPSILON &&
          relPos.z >= -sumDim.z - EPSILON;
    }
    else {
      return sObj->overlaps( aabb, EPSILON );
    }
  }

  // checks if AABB and Brush overlap
  bool Collider::overlapsAABBBrush( const BSP::Brush* brush ) const
  {
    bool result = true;

    for( int i = 0; i < brush->nSides; ++i ) {
      const Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset = aabb.dim * plane.abs();
      float dist   = startPos * plane - offset;

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
      const BSP::Node& node  = bsp->nodes[nodeIndex];
      const Plane&     plane = bsp->planes[node.plane];

      float offset = aabb.dim * plane.abs() + 2.0f * EPSILON;
      float dist   = startPos * plane;

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

  bool Collider::overlapsAABBEntities()
  {
    if( str->nEntities == 0 ) {
      return false;
    }

    Point3 originalStartPos = startPos;
    Bounds localTrace       = str->toStructCS( trace );

    for( int i = 0; i < bsp->nModels; ++i ) {
      model  = &bsp->models[i];
      entity = &str->entities[i];

      if( localTrace.overlaps( *model + entity->offset ) ) {
        for( int j = 0; j < model->nBrushes; ++j ) {
          int index = model->firstBrush + j;
          const BSP::Brush& brush = bsp->brushes[index];

          hard_assert( !visitedBrushes.get( index ) );

          startPos = originalStartPos - entity->offset;

          if( ( brush.material & Material::STRUCT_BIT ) && overlapsAABBBrush( &brush ) ) {
            startPos = originalStartPos;
            return true;
          }
        }
      }
    }
    startPos = originalStartPos;
    return false;
  }

  // check for AABB-AABB, AABB-Brush and AABB-Terrain overlapping in the world
  bool Collider::overlapsAABBOrbis()
  {
    if( !orbis.includes( aabb, -EPSILON ) ) {
      return true;
    }

    if( aabb.p.z - aabb.dim.z - orbis.terra.height( aabb.p.x, aabb.p.y ) <= 0.0f ) {
      return true;
    }

    const Struct* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          if( str != oldStr && str->overlaps( trace ) ) {
            visitedBrushes.clearAll();

            startPos = str->toStructCS( aabb.p );
            bsp      = orbis.bsps[str->bsp];

            if( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) {
              return true;
            }

            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.citer() ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && overlapsAABBObj( sObj ) ) {
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
    if( !orbis.includes( aabb, -EPSILON ) ) {
      return true;
    }

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( sObj, cell.objects.citer() ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && overlapsAABBObj( sObj ) ) {
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
    if( !orbis.includes( aabb, -EPSILON ) ) {
      return true;
    }

    const Struct* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          if( str != oldStr && str->overlaps( trace ) ) {
            visitedBrushes.clearAll();

            startPos = str->toStructCS( aabb.p );
            bsp      = orbis.bsps[str->bsp];

            if( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) {
              return true;
            }

            oldStr = str;
          }
        }

        foreach( sObj, cell.objects.citer() ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && overlapsAABBObj( sObj ) ) {
            return true;
          }
        }
      }
    }
    return false;
  }

  //***********************************
  //*        STATIC ENTITY CD         *
  //***********************************

  // check for BSP Model-AABB overlapping in the world
  bool Collider::overlapsEntityOrbisOO()
  {
    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( sObj, cell.objects.citer() ) {
          if( sObj->overlaps( trace ) ) {
            startPos = str->toStructCS( sObj->p ) - entity->offset;
            aabb.dim = sObj->dim + Vec3( margin, margin, margin );

            for( int i = 0; i < model->nBrushes; ++i ) {
              const BSP::Brush& brush = bsp->brushes[model->firstBrush + i];

              if( ( brush.material & Material::STRUCT_BIT ) && ( sObj->flags & mask ) &&
                  overlapsAABBBrush( &brush ) )
              {
                return true;
              }
            }
          }
        }
      }
    }
    return false;
  }

  //***********************************
  //*        DYNAMIC AABB CD          *
  //***********************************

  // finds out if AABB-Orbis bounding box collision occurs and the time when it occurs
  void Collider::trimAABBVoid()
  {
    for( int i = 0; i < 3; ++i ) {
      int side = move[i] >= 0.0f;
      const Vec3& normal = normals[i * 2 + side];

      float startDist = orbis.maxs[i] + startPos[i] * normal[i] - aabb.dim[i];
      float endDist   = orbis.maxs[i] + endPos[i]   * normal[i] - aabb.dim[i];

      if( endDist <= EPSILON && endDist <= startDist ) {
        float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

        if( ratio < hit.ratio ) {
          hit.ratio    = ratio;
          hit.normal   = normal;
          hit.obj      = null;
          hit.str      = null;
          hit.entity   = null;
          hit.material = Material::VOID_BIT;
        }
      }
    }
  }

  // finds out if AABB-AABB collision occurs and the time when it occurs
  void Collider::trimAABBObj( const Object* sObj )
  {
    float minRatio = -1.0f;
    float maxRatio =  1.0f;
    Vec3  lastNormal;

    Vec3  relStartPos = startPos - sObj->p;
    Vec3  relEndPos   = endPos   - sObj->p;
    Vec3  sumDim      = aabb.dim + sObj->dim;

    int   firstPlane  = 0;

    if( flags & sObj->flags & Object::CYLINDER_BIT ) {
      float rx = relStartPos.x;
      float ry = relStartPos.y;
      float sx = move.x;
      float sy = move.y;

      if( rx*rx + ry*ry >= sumDim.x*sumDim.x ) {
        float moveDist2    = sx*sx + sy*sy;
        float rxsx_rysy    = rx*sx + ry*sy;
        float rxsy_rysx    = rx*sy - ry*sx;
        float radius       = sumDim.x + EPSILON;
        float discriminant = radius*radius * moveDist2 - rxsy_rysx * rxsy_rysx;

        if( discriminant > 0.0f ) {
          float sqrtDiscr = Math::sqrt( discriminant );
          float endRatio = ( -rxsx_rysy + sqrtDiscr ) / ( moveDist2 + LOCAL_EPS );

          if( endRatio > 0.0f ) {
            float startRatio = ( -rxsx_rysy - sqrtDiscr ) / ( moveDist2 + LOCAL_EPS );

            minRatio = max( minRatio, startRatio );
            maxRatio = min( maxRatio, endRatio );

            lastNormal = ~Vec3( rx + startRatio*sx, ry + startRatio*sy, 0.0f );
          }
        }
        else {
          return;
        }
      }

      firstPlane = 2;
    }

    for( int i = firstPlane; i < 3; ++i ) {
      float startDist, endDist;

      startDist = +relStartPos[i] - sumDim[i];
      endDist   = +relEndPos[i]   - sumDim[i];

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + LOCAL_EPS );

        if( ratio > minRatio ) {
          minRatio   = ratio;
          lastNormal = normals[2*i];
        }
      }

      startDist = -relStartPos[i] - sumDim[i];
      endDist   = -relEndPos[i]   - sumDim[i];

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + LOCAL_EPS );

        if( ratio > minRatio ) {
          minRatio   = ratio;
          lastNormal = normals[2*i + 1];
        }
      }
    }

    if( minRatio != -1.0f && minRatio <= maxRatio && minRatio < hit.ratio ) {
      hit.ratio    = max( 0.0f, minRatio );
      hit.normal   = lastNormal;
      hit.obj      = sObj;
      hit.str      = null;
      hit.entity   = null;
      hit.material = Material::OBJECT_BIT;
    }
  }

  // finds out if AABB-Brush collision occurs and the time when it occurs
  void Collider::trimAABBBrush( const BSP::Brush* brush )
  {
    float minRatio = -1.0f;
    float maxRatio =  1.0f;
    Vec3  lastNormal;

    for( int i = 0; i < brush->nSides; ++i ) {
      const Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset    = aabb.dim * plane.abs();
      float startDist = startPos * plane - offset;
      float endDist   = endPos   * plane - offset;

      if( endDist > EPSILON ) {
        if( startDist < 0.0f ) {
          maxRatio = min( maxRatio, startDist / ( startDist - endDist ) );
        }
        else {
          return;
        }
      }
      else if( startDist >= 0.0f && endDist <= startDist ) {
        float ratio = ( startDist - EPSILON ) / ( startDist - endDist + LOCAL_EPS );

        if( ratio > minRatio ) {
          minRatio   = ratio;
          lastNormal = plane.n;
        }
      }
    }
    if( minRatio != -1.0f && minRatio <= maxRatio && minRatio < hit.ratio ) {
      hit.ratio    = max( 0.0f, minRatio );
      hit.normal   = str->toAbsoluteCS( lastNormal );
      hit.obj      = null;
      hit.str      = str;
      hit.entity   = entity;
      hit.material = brush->material;
    }
  }

  // checks if AABB and Brush overlap and if AABB centre is inside a brush
  void Collider::trimAABBWater( const BSP::Brush* brush )
  {
    float depth = Math::INF;

    for( int i = 0; i < brush->nSides; ++i ) {
      const Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      if( plane.n.z <= 0.0f ) {
        float centreDist = startPos * plane;

        if( centreDist > -EPSILON ) {
          return;
        }
      }
      else {
        float dist = ( plane.d - startPos.x*plane.n.x + startPos.y*plane.n.y ) /
            plane.n.z - startPos.z + aabb.dim.z;

        if( dist < 0.0f ) {
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

  // checks if AABB and Brush overlap and if AABB centre is inside a brush
  void Collider::trimAABBLadder( const BSP::Brush* brush )
  {
    for( int i = 0; i < brush->nSides; ++i ) {
      const Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset = aabb.dim * plane.abs();
      float dist   = startPos * plane - offset;

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
      const BSP::Node& node  = bsp->nodes[nodeIndex];
      const Plane&     plane = bsp->planes[node.plane];

      float offset    = aabb.dim * plane.abs() + 2.0f * EPSILON;
      float startDist = startPos * plane;
      float endDist   = endPos   * plane;

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

  void Collider::trimAABBEntities()
  {
    if( str->nEntities == 0 ) {
      return;
    }

    Point3 originalStartPos = startPos;
    Point3 originalEndPos   = endPos;
    Bounds localTrace       = str->toStructCS( trace );

    for( int i = 0; i < bsp->nModels; ++i ) {
      model  = &bsp->models[i];
      entity = &str->entities[i];

      if( localTrace.overlaps( *model + entity->offset ) ) {
        for( int j = 0; j < model->nBrushes; ++j ) {
          int index = model->firstBrush + j;
          const BSP::Brush& brush = bsp->brushes[index];

          hard_assert( !visitBrush( index ) );

          startPos = originalStartPos - entity->offset;
          endPos   = originalEndPos   - entity->offset;

          trimAABBBrush( &brush );
        }
      }
    }
    startPos = originalStartPos;
    endPos   = originalEndPos;
  }

  // terrain collision detection is penetration-safe
  bool Collider::trimAABBTerraQuad( int x, int y )
  {
    const Terra::Quad& quad     = orbis.terra.quads[x    ][y    ];
    const Terra::Quad& nextQuad = orbis.terra.quads[x + 1][y + 1];

    const Point3& minVert = quad.vertex;
    const Point3& maxVert = nextQuad.vertex;

    Vec3 localStartPos = startPos - minVert;
    Vec3 localEndPos   = endPos   - minVert;

    float startDist = localStartPos * quad.triNormal[0];
    float endDist   = localEndPos   * quad.triNormal[0];

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = startPos.x + ratio * move.x;
      float impactY = startPos.y + ratio * move.y;

      if( impactX - minVert.x >= impactY - minVert.y &&
          minVert.x <= impactX && impactX <= maxVert.x &&
          minVert.y <= impactY && impactY <= maxVert.y &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.triNormal[0];
        hit.obj      = null;
        hit.str      = null;
        hit.entity   = null;
        hit.material = Material::TERRAIN_BIT;

        return false;
      }
    }

    startDist = localStartPos * quad.triNormal[1];
    endDist   = localEndPos   * quad.triNormal[1];

    if( endDist <= EPSILON && endDist <= startDist ) {
      float ratio = max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

      float impactX = startPos.x + ratio * move.x;
      float impactY = startPos.y + ratio * move.y;

      if( impactX - minVert.x <= impactY - minVert.y &&
          minVert.x <= impactX && impactX <= maxVert.x &&
          minVert.y <= impactY && impactY <= maxVert.y &&
          ratio < hit.ratio )
      {
        hit.ratio    = ratio;
        hit.normal   = quad.triNormal[1];
        hit.obj      = null;
        hit.str      = null;
        hit.entity   = null;
        hit.material = Material::TERRAIN_BIT;

        return false;
      }
    }
    return true;
  }

  void Collider::trimAABBTerra()
  {
    if( startPos.z < 0.0f ) {
      hit.waterDepth = max( hit.waterDepth, -startPos.z );
      hit.inWater = true;
    }

    float minPosX = min( startPos.x, endPos.x );
    float minPosY = min( startPos.y, endPos.y );
    float maxPosX = max( startPos.x, endPos.x );
    float maxPosY = max( startPos.y, endPos.y );

    Span terraSpan = orbis.terra.getInters( minPosX, minPosY, maxPosX, maxPosY );

    for( int x = terraSpan.minX; x <= terraSpan.maxX; ++x ) {
      for( int y = terraSpan.minY; y <= terraSpan.maxY; ++y ) {
        trimAABBTerraQuad( x, y );
      }
    }
  }

  // move AABB until first collisons occurs
  void Collider::trimAABBOrbis()
  {
    hit.ratio      = 1.0f;
    hit.obj        = null;
    hit.str        = null;
    hit.entity     = null;
    hit.material   = 0;
    hit.waterDepth = 0.0f;
    hit.inWater    = false;
    hit.onLadder   = false;

    Point3 originalStartPos = aabb.p;
    Point3 originalEndPos   = aabb.p + move;

    startPos = originalStartPos;
    endPos   = originalEndPos;

    if( !orbis.includes( trace ) ) {
      trimAABBVoid();
    }

    const Struct* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          // to prevent some of duplicated structure tests
          if( str != oldStr && str->overlaps( trace ) ) {
            visitedBrushes.clearAll();

            startPos = str->toStructCS( originalStartPos );
            endPos   = str->toStructCS( originalEndPos );
            bsp      = orbis.bsps[str->bsp];
            entity   = null;

            trimAABBNode( 0 );
            trimAABBEntities();

            oldStr = str;
          }
        }

        startPos = originalStartPos;
        endPos   = originalEndPos;

        foreach( sObj, cell.objects.citer() ) {
          if( sObj != exclObj && ( sObj->flags & mask ) && sObj->overlaps( trace ) ) {
            trimAABBObj( sObj );
          }
        }
      }
    }

    startPos.z -= aabb.dim.z;
    endPos.z   -= aabb.dim.z;

    trimAABBTerra();

    hard_assert( 0.0f <= hit.ratio && hit.ratio <= 1.0f );
    hard_assert( ( ( hit.material & Material::OBJECT_BIT ) != 0 ) == ( hit.obj != null ) );
  }

  //***********************************
  //*          OVERLAPPING            *
  //***********************************

  // get all objects and structures that overlap with our trace
  void Collider::getOrbisOverlaps( Vector<Object*>* objects, Vector<Struct*>* structs )
  {
    hard_assert( objects != null || structs != null );

    const Struct* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        if( structs != null ) {
          foreach( strIndex, cell.structs.citer() ) {
            Struct* str = orbis.structs[*strIndex];

            if( str != oldStr && str->overlaps( trace ) && !structs->contains( str ) ) {
              visitedBrushes.clearAll();

              startPos = str->toStructCS( aabb.p );
              bsp      = orbis.bsps[str->bsp];

              if( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) {
                structs->add( str );
              }
            }

            oldStr = str;
          }
        }

        if( objects != null ) {
          foreach( sObj, cell.objects.iter() ) {
            if( ( sObj->flags & mask ) && sObj->overlaps( trace ) ) {
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
    hard_assert( objects != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( sObj, cell.objects.iter() ) {
          if( ( sObj->flags & mask ) && trace.includes( *sObj ) ) {
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
        const Cell& cell = orbis.cells[x][y];

        foreach( sObj, cell.objects.iter() ) {
          if( ( sObj->flags & Object::DYNAMIC_BIT ) && sObj->overlaps( trace ) ) {
            // clearing these two bits should do
            sObj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
          }
        }
      }
    }
  }

  void Collider::getEntityOverlaps( Vector<Object*>* objects )
  {
    hard_assert( objects != null );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( sObj, cell.objects.iter() ) {
          if( ( sObj->flags & mask ) && sObj->overlaps( trace ) ) {
            startPos = str->toStructCS( sObj->p ) - entity->offset;
            aabb.dim = sObj->dim + Vec3( margin, margin, margin );

            for( int i = 0; i < model->nBrushes; ++i ) {
              const BSP::Brush& brush = bsp->brushes[model->firstBrush + i];

              if( overlapsAABBBrush( &brush ) ) {
                objects->add( sObj );
              }
            }
          }
        }
      }
    }
  }

  bool Collider::overlaps( const Point3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;
    flags = Object::CYLINDER_BIT;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  bool Collider::overlapsOO( const Point3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;
    flags = Object::CYLINDER_BIT;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  bool Collider::overlapsOSO( const Point3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;
    flags = Object::CYLINDER_BIT;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  bool Collider::overlaps( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;
    flags = 0;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  bool Collider::overlapsOO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;
    flags = 0;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  bool Collider::overlapsOSO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;
    flags = 0;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  bool Collider::overlaps( const Object* obj_, const Object* exclObj_ )
  {
    aabb = *obj_;
    exclObj = exclObj_;
    flags = obj_->flags;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  bool Collider::overlapsOO( const Object* obj_, const Object* exclObj_ )
  {
    aabb = *obj_;
    exclObj = exclObj_;
    flags = obj_->flags;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  bool Collider::overlapsOSO( const Object* obj_, const Object* exclObj_ )
  {
    aabb = *obj_;
    exclObj = exclObj_;
    flags = obj_->flags;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  bool Collider::overlapsOO( const Struct::Entity* entity_, float margin_ )
  {
    str = entity_->str;
    entity = entity_;
    bsp = entity_->model->bsp;
    model = entity_->model;
    margin = margin_;

    Bounds bounds = Bounds( model->mins - Vec3( margin, margin, margin ) + entity->offset,
                            model->maxs + Vec3( margin, margin, margin ) + entity->offset );

    trace = str->toAbsoluteCS( bounds );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsEntityOrbisOO();
  }

  void Collider::getOverlaps( const AABB& aabb_, Vector<Object*>* objects,
                              Vector<Struct*>* structs, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    getOrbisOverlaps( objects, structs );
  }

  void Collider::getIncludes( const AABB& aabb_, Vector<Object*>* objects, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    getOrbisIncludes( objects );
  }

  void Collider::touchOverlaps( const AABB& aabb_, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    touchOrbisOverlaps();
  }

  void Collider::getOverlaps( const Struct::Entity* entity_, Vector<Object*>* objects,
                              float margin_ )
  {
    str = entity_->str;
    entity = entity_;
    bsp = entity_->model->bsp;
    model = entity_->model;
    margin = margin_;

    Bounds bounds = Bounds( model->mins - Vec3( margin, margin, margin ) + entity->offset,
                            model->maxs + Vec3( margin, margin, margin ) + entity->offset );

    trace = str->toAbsoluteCS( bounds );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    getEntityOverlaps( objects );
  }

  void Collider::translate( const Point3& point, const Vec3& move_, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    move = move_;
    exclObj = exclObj_;
    flags = Object::CYLINDER_BIT;

    trace.fromPointMove( point, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

  void Collider::translate( const AABB& aabb_, const Vec3& move_, const Object* exclObj_ )
  {
    obj = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;
    flags = 0;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

  void Collider::translate( const Dynamic* obj_, const Vec3& move_ )
  {
    hard_assert( obj_->cell != null );

    obj = obj_;
    aabb = *obj_;
    move = move_;
    exclObj = obj_;
    flags = obj_->flags;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

}
