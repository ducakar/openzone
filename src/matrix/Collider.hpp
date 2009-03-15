/*
 *  Collider.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Hit.hpp"
#include "World.hpp"

namespace oz
{

  class Collider
  {
    protected:

      static const Vec3 bbNormals[];

      Vec3      point;
      AABB      aabb;
      Object    *obj;
      Vec3      move;

      Bounds    trace;

      Vec3      leafStartPos;
      Vec3      leafEndPos;
      Vec3      globalStartPos;
      Vec3      globalEndPos;
      float     leafStartRatio;
      float     leafEndRatio;

      const BSP    *bsp;
      const Object *exclObj;

      bool testPointTerraQuad( int x, int y );
      bool testPointSimplex( const BSP::Simplex *simplex );
      bool testPointNode( int nodeIndex );
      bool testPointTerra();
      bool testPointWorld();
      bool testPointWorldOO();
      bool testPointWorldOSO();

      bool trimTerraQuad( int x, int y );

      void trimPointVoid();
      void trimPointObj( Object *sObj );
      void trimPointSimplex( const BSP::Simplex *simplex );
      void trimPointNode( int nodeIndex, float startRatio, float endRatio,
                          const Vec3 &startPos, const Vec3 &endPos );
      void trimPointTerra();
      void trimPointWorld();

      bool testAABBSimplex( const BSP::Simplex *simplex );
      bool testAABBNode( int nodeIndex );
      bool testAABBWorld();
      bool testAABBWorldOO();
      bool testAABBWorldOSO();

      void trimAABBVoid();
      void trimAABBObj( Object *sObj );
      void trimAABBSimplex( const BSP::Simplex *simplex );
      void trimAABBNode( int nodeIndex, float startRatio, float endRatio,
                         const Vec3 &startPos, const Vec3 &endPos );
      void trimAABBWorld();

      void getWorldOverlaps( Vector<Object*> *objects, Vector<Structure*> *structs );
      void getWorldIncludes( Vector<Object*> *objects );

    public:

      Hit hit;

      bool test( const Vec3 &point );
      // test for object collisions only (no structures or terrain)
      bool testOO( const Vec3 &point );
      // test for object and structure collisions only (no terain)
      bool testOSO( const Vec3 &point );

      bool test( const AABB &aabb, const Object *exclObj = null );
      bool testOO( const AABB &aabb, const Object *exclObj = null );
      bool testOSO( const AABB &aabb, const Object *exclObj = null );

      // returns vector of objects and vector of structures, overlapping with given AABB
      // if either vector is null, respective test isn't performed
      void getOverlaps( const AABB &aabb, Vector<Object*> *objects, Vector<Structure*> *structs );

      void translate( const Vec3 &point, const Vec3 &move );
      void translate( const AABB &aabb, const Vec3 &move, const Object *exclObj );
      void translate( Object *obj, const Vec3 &move );
  };

  extern Collider collider;

  inline bool Collider::test( const Vec3 &point_ )
  {
    point = point_;

    world.getInters( point, AABB::MAX_DIMXY );

    return testPointWorld();
  }

  inline bool Collider::testOO( const Vec3 &point_ )
  {
    point = point_;

    world.getInters( point, AABB::MAX_DIMXY );

    return testPointWorldOO();
  }

  inline bool Collider::testOSO( const Vec3 &point_ )
  {
    point = point_;

    world.getInters( point, AABB::MAX_DIMXY );

    return testPointWorldOSO();
  }

  inline bool Collider::test( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb_.toBounds( EPSILON ), AABB::MAX_DIMXY );

    return testAABBWorld();
  }

  inline bool Collider::testOO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb_.toBounds( EPSILON ), AABB::MAX_DIMXY );

    return testAABBWorldOO();
  }

  inline bool Collider::testOSO( const AABB &aabb_, const Object *exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    world.getInters( aabb_.toBounds( EPSILON ), AABB::MAX_DIMXY );

    return testAABBWorldOSO();
  }

  inline void Collider::getOverlaps( const AABB &aabb_,
                                     Vector<Object*> *objects,
                                     Vector<Structure*> *structs )
  {
    aabb = aabb_;
    exclObj = null;

    world.getInters( aabb_.toBounds( EPSILON ), AABB::MAX_DIMXY );

    return getWorldOverlaps( objects, structs );
  }

  inline void Collider::translate( const Vec3 &point_, const Vec3 &move_ )
  {
    point = point_;
    move = move_;

    trace.fromPointMove( point, move, EPSILON );
    world.getInters( trace, AABB::MAX_DIMXY );

    trimPointWorld();
  }

  inline void Collider::translate( const AABB &aabb_, const Vec3 &move_, const Object *exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace = aabb.toBounds( move, EPSILON );
    world.getInters( trace, AABB::MAX_DIMXY );

    trimAABBWorld();
  }

  inline void Collider::translate( Object *obj_, const Vec3 &move_ )
  {
    obj  = obj_;
    aabb = *obj;
    move = move_;
    exclObj = obj;

    trace = aabb.toBounds( move, EPSILON );
    world.getInters( trace, AABB::MAX_DIMXY );

    trimAABBWorld();
  }

}
