/*
 *  Collider.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "World.h"

namespace oz
{

  struct Hit
  {
    float  ratio;

    Vec3   normal;
    Object *obj;
    int    material;
    float  waterDepth;

    // only set for object trim
    bool   inWater;
    bool   onLadder;
  };

  class Collider
  {
    private:

      static const Vec3 bbNormals[];

      Vec3            point;
      AABB            aabb;
      const DynObject *obj;
      Vec3            move;

      Bounds          trace;

      Vec3            leafStartPos;
      Vec3            leafEndPos;
      Vec3            globalStartPos;
      Vec3            globalEndPos;
      float           leafStartRatio;
      float           leafEndRatio;

      const BSP       *bsp;
      const Structure *str;
      const Object    *exclObj;

      /**
       * Rotate vector from absolute coordinate system to structure coordinate system. Do not
       * translate (because of normals).
       * @param v
       * @return
       */
      Vec3 toStructCS( const Vec3 &v ) const;

      /**
       * Rotate vector from structure coordinate system to absolute coordinate system. Do not
       * translate (because of normals).
       * @param v
       * @return
       */
      Vec3 toAbsoluteCS( const Vec3 &v ) const;

      bool testPointTerraQuad( int x, int y );
      bool testPointBrush( const BSP::Brush *brush );
      bool testPointNode( int nodeIndex );
      bool testPointTerra();
      bool testPointWorld();
      bool testPointWorldOO();
      bool testPointWorldOSO();

      bool trimTerraQuad( int x, int y );
      void trimPointTerra();

      void trimPointVoid();
      void trimPointObj( Object *sObj );
      void trimPointBrush( const BSP::Brush *brush );
      void trimPointNode( int nodeIndex, float startRatio, float endRatio,
                          const Vec3 &startPos, const Vec3 &endPos );
      void trimPointWorld();

      bool testAABBBrush( const BSP::Brush *brush );
      bool testAABBNode( int nodeIndex );
      bool testAABBWorld();
      bool testAABBWorldOO();
      bool testAABBWorldOSO();

      void trimAABBVoid();
      void trimAABBObj( Object *sObj );
      void trimAABBBrush( const BSP::Brush *brush );
      void trimAABBWater( const BSP::Brush *brush );
      void trimAABBLadder( const BSP::Brush *brush );
      void trimAABBNode( int nodeIndex, float startRatio, float endRatio,
                         const Vec3 &startPos, const Vec3 &endPos );
      void trimAABBWorld();

      void getWorldOverlaps( Vector<Object*> *objects,
                             Vector<const Structure*> *structs );
      void getWorldIncludes( Vector<Object*> *objects );

    public:

      // for returning getInters cell indices
      int minX;
      int minY;
      int maxX;
      int maxY;

      Hit hit;

      // get pointer to the cell the point is in
      Cell *getCell( float x, float y );
      Cell *getCell( const Vec3 &p );

      // get indices of the cell the point is in
      void getInters( float x, float y, float epsilon = 0.0f );
      void getInters( const Vec3 &p, float epsilon = 0.0f );

      // get indices of min and max cells which the area intersects
      void getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                      float epsilon = 0.0f );

      // get indices of min and max cells which the AABB intersects
      void getInters( const AABB &bb, float epsilon = 0.0f );

      // get indices of min and max cells which the bounds intersects
      void getInters( const Bounds &bounds, float epsilon = 0.0f );

      bool test( const Vec3 &point, const Object *exclObj = null );
      // test for object collisions only (no structures or terrain)
      bool testOO( const Vec3 &point, const Object *exclObj = null );
      // test for object and structure collisions only (no terain)
      bool testOSO( const Vec3 &point, const Object *exclObj = null );

      bool test( const AABB &aabb, const Object *exclObj = null );
      bool testOO( const AABB &aabb, const Object *exclObj = null );
      bool testOSO( const AABB &aabb, const Object *exclObj = null );

      // fill given vectors with objects and structures overlapping with the AABB
      // if either vector is null the respecitve test is not performed
      void getOverlaps( const AABB &aabb, Vector<Object*> *objects,
                        Vector<const Structure*> *structs );

      // fill given vector with objects included in the AABB
      void getIncludes( const AABB &aabb, Vector<Object*> *objects );

      void translate( const Vec3 &point, const Vec3 &move, const Object *exclObj = null );
      void translate( const AABB &aabb, const Vec3 &move, const Object *exclObj = null );
      void translate( const DynObject *obj, const Vec3 &move );

  };

  extern Collider collider;

  inline Cell *Collider::getCell( float x, float y )
  {
    int ix = static_cast<int>( x + World::DIM ) / Cell::SIZEI;
    int iy = static_cast<int>( y + World::DIM ) / Cell::SIZEI;

    ix = bound( ix, 0, World::MAX - 1 );
    iy = bound( iy, 0, World::MAX - 1 );

    return &world.cells[ix][iy];
  }

  inline Cell *Collider::getCell( const Vec3 &p )
  {
    return getCell( p.x, p.y );
  }

  inline void Collider::getInters( float x, float y, float epsilon )
  {
    minX = max( static_cast<int>( x - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    minY = max( static_cast<int>( y - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    maxX = min( static_cast<int>( x + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    maxY = min( static_cast<int>( y + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void Collider::getInters( const Vec3 &p, float epsilon )
  {
    getInters( p.x, p.y, epsilon );
  }

  inline void Collider::getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                                float epsilon )
  {
    minX = max( static_cast<int>( minPosX - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    minY = max( static_cast<int>( minPosY - epsilon + World::DIM ) / Cell::SIZEI, 0 );
    maxX = min( static_cast<int>( maxPosX + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
    maxY = min( static_cast<int>( maxPosY + epsilon + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
  }

  inline void Collider::getInters( const AABB &bb, float epsilon )
  {
    getInters( bb.p.x - bb.dim.x, bb.p.y - bb.dim.y, bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
               epsilon );
  }

  inline void Collider::getInters( const Bounds &bounds, float epsilon )
  {
    getInters( bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
  }

  inline bool Collider::test( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    getInters( point, AABB::MAX_DIM );

    return testPointWorld();
  }

  inline bool Collider::testOO( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    getInters( point, AABB::MAX_DIM );

    return testPointWorldOO();
  }

  inline bool Collider::testOSO( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    getInters( point, AABB::MAX_DIM );

    return testPointWorldOSO();
  }

  inline bool Collider::test( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( EPSILON );
    getInters( trace, AABB::MAX_DIM );

    return testAABBWorld();
  }

  inline bool Collider::testOO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( EPSILON );
    getInters( trace, AABB::MAX_DIM );

    return testAABBWorldOO();
  }

  inline bool Collider::testOSO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( EPSILON );
    getInters( trace, AABB::MAX_DIM );

    return testAABBWorldOSO();
  }

  inline void Collider::getOverlaps( const AABB &aabb_,
                                     Vector<Object*> *objects,
                                     Vector<const Structure*> *structs )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( EPSILON );
    getInters( trace, AABB::MAX_DIM );

    return getWorldOverlaps( objects, structs );
  }

  inline void Collider::getIncludes( const AABB &aabb_, Vector<Object*> *objects )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( EPSILON );
    getInters( trace, AABB::MAX_DIM );

    return getWorldIncludes( objects );
  }

  inline void Collider::translate( const Vec3 &point_, const Vec3 &move_, const Object *exclObj_ )
  {
    point = point_;
    move = move_;
    exclObj = exclObj_;

    trace.fromPointMove( point, move, EPSILON );
    getInters( trace, AABB::MAX_DIM );

    trimPointWorld();
  }

  inline void Collider::translate( const AABB &aabb_, const Vec3 &move_, const Object *exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace = aabb.toBounds( move, EPSILON );
    getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

  inline void Collider::translate( const DynObject *obj_, const Vec3 &move_ )
  {
    obj  = obj_;
    aabb = *obj;
    move = move_;
    exclObj = obj;

    trace = aabb.toBounds( move, EPSILON );
    getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

}
