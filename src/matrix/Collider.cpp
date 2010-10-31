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

  // checks if AABB and Brush overlap
  bool Collider::overlapsAABBBrush( const BSP::Brush* brush ) const
  {
    bool result = true;

    for( int i = 0; i < brush->nSides; ++i ) {
      const BSP::Plane& plane = bsp->planes[ bsp->brushSides[brush->firstSide + i] ];

      float offset = plane.normal.abs() * aabb.dim;
      float dist   = startPos * plane.normal - plane.distance - offset;

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

      float offset = plane.normal.abs() * aabb.dim + 2.0f * EPSILON;
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

  bool Collider::overlapsAABBEntities()
  {
    if( bsp->nEntityClasses == 1 ) {
      return false;
    }

    Vec3   originalStartPos = startPos;
    Bounds localTrace       = str->toStructCS( trace - str->p );

    for( int i = 1; i < bsp->nEntityClasses; ++i ) {
      const EntityClass& clazz = bsp->entityClasses[i];
      const Vec3& offset = str->entities[i].offset;

      if( clazz.overlaps( localTrace - offset ) ) {
        for( int j = 0; j < entityClazz->nBrushes; ++j ) {
          int index = entityClazz->firstBrush + j;
          const BSP::Brush& brush = bsp->brushes[index];

          assert( !visitedBrushes.get( index ) );

          startPos = originalStartPos - offset;

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

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          if( str != oldStr ) {
            bsp = orbis.bsps[str->bsp];

            startPos = str->toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) ) {
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
    if( !orbis.includes( aabb, -EPSILON ) ) {
      return true;
    }

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

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
    if( !orbis.includes( aabb, -EPSILON ) ) {
      return true;
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          if( str != oldStr ) {
            bsp = orbis.bsps[str->bsp];

            startPos = str->toStructCS( aabb.p - str->p );
            visitedBrushes.clearAll();

            if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) ) {
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

  //***********************************
  //*        STATIC ENTITY CD         *
  //***********************************

  // check for BSP Model-AABB overlapping in the world
  bool Collider::overlapsEntityOrbisOO()
  {
    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        for( const Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj->overlaps( trace ) ) {
            startPos = str->toStructCS( sObj->p - str->p ) - entity->offset;
            aabb.dim = sObj->dim;

            for( int i = 0; i < entityClazz->nBrushes; ++i ) {
              const BSP::Brush& brush = bsp->brushes[entityClazz->firstBrush + i];

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
        float ratio = Math::max( startDist - EPSILON, 0.0f ) / ( startDist - endDist + EPSILON );

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
    float minRatio        = -1.0f;
    float maxRatio        =  1.0f;
    const Vec3* tmpNormal = null;

    for( int i = 0; i < 6; ++i ) {
      int j = i / 2;
      const Vec3& normal = normals[i];

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
      hit.entity   = null;
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

      float offset    = plane.normal.abs() * aabb.dim;
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
        hit.normal   = str->toAbsoluteCS( *tmpNormal );
        hit.obj      = null;
        hit.str      = str;
        hit.entity   = entity;
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

      float offset = plane.normal.abs() * aabb.dim;
      float dist   = startPos * plane.normal - plane.distance - offset;

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

      float offset    = plane.normal.abs() * aabb.dim + 2.0f * EPSILON;
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

  void Collider::trimAABBEntities()
  {
    if( bsp->nEntityClasses == 1 ) {
      return;
    }

    Vec3   originalStartPos = startPos;
    Vec3   originalEndPos   = endPos;
    Bounds localTrace       = str->toStructCS( trace - str->p );

    for( int i = 1; i < bsp->nEntityClasses; ++i ) {
      entity = &str->entities[i];
      entityClazz = entity->clazz;

      if( localTrace.overlaps( *entityClazz + entity->offset ) ) {
        for( int j = 0; j < entityClazz->nBrushes; ++j ) {
          int index = entityClazz->firstBrush + j;
          const BSP::Brush& brush = bsp->brushes[index];

          assert( !visitBrush( index ) );

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
    const Terra::Quad& quad = orbis.terra.quads[x][y];
    const Vec3& minVert = orbis.terra.vertices[x    ][y    ];
    const Vec3& maxVert = orbis.terra.vertices[x + 1][y + 1];

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
        hit.entity   = null;
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
      hit.waterDepth = Math::max( hit.waterDepth, -startPos.z );
      hit.inWater = true;
    }

    float minPosX = Math::min( startPos.x, endPos.x );
    float minPosY = Math::min( startPos.y, endPos.y );
    float maxPosX = Math::max( startPos.x, endPos.x );
    float maxPosY = Math::max( startPos.y, endPos.y );

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

    Vec3 originalStartPos = aabb.p;
    Vec3 originalEndPos   = aabb.p + move;

    startPos = originalStartPos;
    endPos   = originalEndPos;

    if( !orbis.includes( trace ) ) {
      trimAABBVoid();
    }

    const Structure* oldStr = null;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        foreach( strIndex, cell.structs.citer() ) {
          str = orbis.structs[*strIndex];

          // to prevent some of duplicated structure tests
          if( str != oldStr ) {
            bsp = orbis.bsps[str->bsp];

            if( str->overlaps( trace ) ) {
              visitedBrushes.clearAll();

              startPos = str->toStructCS( originalStartPos - str->p );
              endPos   = str->toStructCS( originalEndPos - str->p );
              entity   = null;

              trimAABBNode( 0 );
              trimAABBEntities();
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

    trimAABBTerra();

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
        const Cell& cell = orbis.cells[x][y];

        if( structs != null ) {
          foreach( strIndex, cell.structs.citer() ) {
            Structure* str = orbis.structs[*strIndex];

            if( !structs->contains( str ) ) {
              bsp = orbis.bsps[str->bsp];

              startPos = str->toStructCS( aabb.p - str->p );

              if( str->overlaps( trace ) && ( overlapsAABBNode( 0 ) || overlapsAABBEntities() ) ) {
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
        const Cell& cell = orbis.cells[x][y];

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
        const Cell& cell = orbis.cells[x][y];

        for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( ( sObj->flags & Object::DYNAMIC_BIT ) && sObj->overlaps( trace ) ) {
            // clearing these two bits should do
            sObj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
          }
        }
      }
    }
  }

  void Collider::getEntityOverlaps( Vector<Object*>* objects, float margin )
  {
    assert( objects != null );

    Vec3 dimMargin = Vec3( margin, margin, margin );

    trace.mins -= dimMargin;
    trace.maxs += dimMargin;

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        const Cell& cell = orbis.cells[x][y];

        for( Object* sObj = cell.firstObject; sObj != null; sObj = sObj->next[0] ) {
          if( sObj->overlaps( trace ) ) {
            startPos = str->toStructCS( sObj->p - str->p ) - entity->offset;
            aabb.dim = sObj->dim + dimMargin;

            for( int i = 0; i < entityClazz->nBrushes; ++i ) {
              const BSP::Brush& brush = bsp->brushes[entityClazz->firstBrush + i];

              if( overlapsAABBBrush( &brush ) ) {
                objects->add( sObj );
              }
            }
          }
        }
      }
    }
  }

  bool Collider::overlaps( const Vec3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  bool Collider::overlapsOO( const Vec3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  bool Collider::overlapsOSO( const Vec3& point, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    exclObj = exclObj_;

    span = orbis.getInters( point, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  bool Collider::overlaps( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  bool Collider::overlapsOO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  bool Collider::overlapsOSO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  bool Collider::overlapsOO( const Entity* entity_ )
  {
    str = entity_->str;
    entity = entity_;
    bsp = entity_->clazz->bsp;
    entityClazz = entity_->clazz;

    trace = str->toAbsoluteCS( *entityClazz + entity->offset ) + str->p;
    span = orbis.getInters( trace, AABB::MAX_DIM );

    return overlapsEntityOrbisOO();
  }

  void Collider::getOverlaps( const AABB& aabb_, Vector<Object*>* objects,
                              Vector<Structure*>* structs, float eps )
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

  void Collider::getOverlaps( const Entity* entity_, Vector<Object*>* objects, float margin )
  {
    str = entity_->str;
    entity = entity_;
    bsp = entity_->clazz->bsp;
    entityClazz = entity_->clazz;

    trace = str->toAbsoluteCS( *entityClazz + entity->offset ) + str->p;
    span = orbis.getInters( trace, AABB::MAX_DIM );

    getEntityOverlaps( objects, margin );
  }

  void Collider::translate( const Vec3& point, const Vec3& move_, const Object* exclObj_ )
  {
    aabb = AABB( point, Vec3::ZERO );
    move = move_;
    exclObj = exclObj_;

    trace.fromPointMove( point, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

  void Collider::translate( const AABB& aabb_, const Vec3& move_, const Object* exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

  void Collider::translate( const Dynamic* obj_, const Vec3& move_ )
  {
    assert( obj_->cell != null );

    obj  = obj_;
    aabb = *obj_;
    move = move_;
    exclObj = obj_;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = orbis.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
  }

}
