/*
 *  Collider.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
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

    // only set for object trim
    bool   onWater;
    bool   inWater;
    bool   underWater;
    bool   onLadder;
    bool   onSlick;
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

      void trimPointVoid();
      void trimPointObj( Object *sObj );
      void trimPointBrush( const BSP::Brush *brush );
      void trimPointNode( int nodeIndex, float startRatio, float endRatio,
                          const Vec3 &startPos, const Vec3 &endPos );
      void trimPointTerra();
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

      Hit hit;

      bool test( const Vec3 &point, const Object *exclObj = null );
      // test for object collisions only (no structures or terrain)
      bool testOO( const Vec3 &point, const Object *exclObj = null );
      // test for object and structure collisions only (no terain)
      bool testOSO( const Vec3 &point, const Object *exclObj = null );

      bool test( const AABB &aabb, const Object *exclObj = null );
      bool testOO( const AABB &aabb, const Object *exclObj = null );
      bool testOSO( const AABB &aabb, const Object *exclObj = null );

      // returns vector of objects and vector of structures, overlapping with given AABB
      // if either vector is null, respective test isn't performed
      void getOverlaps( const AABB &aabb, Vector<Object*> *objects,
                        Vector<const Structure*> *structs );

      // returns vector of objects, overlapping with given AABB
      void getIncludes( const AABB &aabb, Vector<Object*> *objects );

      void translate( const Vec3 &point, const Vec3 &move, const Object *exclObj = null );
      void translate( const AABB &aabb, const Vec3 &move, const Object *exclObj = null );
      void translate( const DynObject *obj, const Vec3 &move );

  };

  extern Collider collider;

  inline bool Collider::test( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    world.getInters( point, AABB::MAX_DIM );

    return testPointWorld();
  }

  inline bool Collider::testOO( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    world.getInters( point, AABB::MAX_DIM );

    return testPointWorldOO();
  }

  inline bool Collider::testOSO( const Vec3 &point_, const Object *exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    world.getInters( point, AABB::MAX_DIM );

    return testPointWorldOSO();
  }

  inline bool Collider::test( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb.toBounds( EPSILON ), AABB::MAX_DIM );

    return testAABBWorld();
  }

  inline bool Collider::testOO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb.toBounds( EPSILON ), AABB::MAX_DIM );

    return testAABBWorldOO();
  }

  inline bool Collider::testOSO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb.toBounds( EPSILON ), AABB::MAX_DIM );

    return testAABBWorldOSO();
  }

  inline void Collider::getOverlaps( const AABB &aabb_,
                                     Vector<Object*> *objects,
                                     Vector<const Structure*> *structs )
  {
    aabb = aabb_;
    exclObj = null;

    world.getInters( aabb.toBounds( EPSILON ), AABB::MAX_DIM );

    return getWorldOverlaps( objects, structs );
  }

  inline void Collider::getIncludes( const AABB &aabb_, Vector<Object*> *objects )
  {
    aabb = aabb_;
    exclObj = null;

    world.getInters( aabb.toBounds( EPSILON ), AABB::MAX_DIM );

    return getWorldIncludes( objects );
  }

  inline void Collider::translate( const Vec3 &point_, const Vec3 &move_, const Object *exclObj_ )
  {
    point = point_;
    move = move_;
    exclObj = exclObj_;

    trace.fromPointMove( point, move, EPSILON );
    world.getInters( trace, AABB::MAX_DIM );

    trimPointWorld();
  }

  inline void Collider::translate( const AABB &aabb_, const Vec3 &move_, const Object *exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace = aabb.toBounds( move, EPSILON );
    world.getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

  inline void Collider::translate( const DynObject *obj_, const Vec3 &move_ )
  {
    obj  = obj_;
    aabb = *obj;
    move = move_;
    exclObj = obj;

    trace = aabb.toBounds( move, EPSILON );
    world.getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

}
