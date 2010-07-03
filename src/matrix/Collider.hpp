/*
 *  Collider.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/World.hpp"

namespace oz
{

  struct Hit
  {
    Vec3  normal;
    float ratio;

    const Object*     obj;
    const Structure*  str;
    const BSP::Model* model;

    int   material;

    float waterDepth;
    bool  inWater;
    bool  onLadder;
  };

  class Collider
  {
    private:

      static const Vec3  bbNormals[];
      static const Mat44 structRotations[];
      static const Mat44 structInvRotations[];

      Span              span;
      Bounds            trace;
      Vec3              move;

      Vec3              point;
      AABB              aabb;

      Vec3              startPos;
      Vec3              endPos;

      const Dynamic*    obj;
      const Object*     exclObj;
      const Structure*  str;
      const BSP::Model* model;
      const BSP*        bsp;

      Bitset            visitedBrushes;

      /**
       * Return if brush was already visited and mark it visited.
       * @param index
       * @return
       */
      bool visitBrush( int index );

      /**
       * Rotate vector from absolute coordinate system to structure coordinate system. Do not
       * translate (because of normals).
       * @param v
       * @return
       */
      Vec3 toStructCS( const Vec3& v ) const;

      /**
       * Rotate vector from structure coordinate system to absolute coordinate system. Do not
       * translate (because of normals).
       * @param v
       * @return
       */
      Vec3 toAbsoluteCS( const Vec3& v ) const;

      bool testPointBrush( const BSP::Brush* brush ) const;
      bool testPointNode( int nodeIndex );
      bool testPointModels() const;
      bool testPointWorld();
      bool testPointWorldOO();
      bool testPointWorldOSO();

      bool trimTerraQuad( int x, int y );
      void trimPointTerra();

      void trimPointVoid();
      void trimPointObj( const Object* sObj );
      void trimPointBrush( const BSP::Brush* brush );
      void trimPointNode( int nodeIndex );
      void trimPointModels();
      void trimPointWorld();

      bool testAABBBrush( const BSP::Brush* brush ) const;
      bool testAABBNode( int nodeIndex );
      bool testAABBModels() const;
      bool testAABBWorld();
      bool testAABBWorldOO();
      bool testAABBWorldOSO();

      void trimAABBVoid();
      void trimAABBObj( const Object* sObj );
      void trimAABBBrush( const BSP::Brush* brush );
      void trimAABBWater( const BSP::Brush* brush );
      void trimAABBLadder( const BSP::Brush* brush );
      void trimAABBNode( int nodeIndex );
      void trimAABBModels();
      void trimAABBWorld();

      bool testModelAABB( const Object* sObj );
      bool testModelWorldOO();

      void getWorldOverlaps( Vector<Object*>* objects, Vector<Structure*>* structs );
      void getWorldIncludes( Vector<Object*>* objects ) const;
      void touchWorldOverlaps() const;

    public:

      /**
       * Collision feedback data
       */
      Hit  hit;
      /**
       * Collision bitmask for Object::flags
       */
      int  mask;

      Collider();

      bool test( const Vec3& point, const Object* exclObj = null );
      // test for object collisions only (no structures or terrain)
      bool testOO( const Vec3& point, const Object* exclObj = null );
      // test for object and structure collisions only (no terain)
      bool testOSO( const Vec3& point, const Object* exclObj = null );

      bool test( const AABB& aabb, const Object* exclObj = null );
      bool testOO( const AABB& aabb, const Object* exclObj = null );
      bool testOSO( const AABB& aabb, const Object* exclObj = null );

      bool testOO( const BSP* bsp, const BSP::Model* model );

      // fill given vectors with objects and structures overlapping with the AABB
      // if either vector is null the respecitve test is not performed
      void getOverlaps( const AABB& aabb, Vector<Object*>* objects, Vector<Structure*>* structs,
                        float eps = 0.0f );
      void touchOverlaps( const AABB& aabb, float eps = 0.0f );

      // fill given vector with objects included in the AABB
      void getIncludes( const AABB& aabb, Vector<Object*>* objects, float eps = 0.0f );

      void translate( const Vec3& point, const Vec3& move, const Object* exclObj = null );
      void translate( const AABB& aabb, const Vec3& move, const Object* exclObj = null );
      void translate( const Dynamic* obj, const Vec3& move );

  };

  extern Collider collider;

  inline bool Collider::test( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return testPointWorld();
  }

  inline bool Collider::testOO( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return testPointWorldOO();
  }

  inline bool Collider::testOSO( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return testPointWorldOSO();
  }

  inline bool Collider::test( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return testAABBWorld();
  }

  inline bool Collider::testOO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return testAABBWorldOO();
  }

  inline bool Collider::testOSO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return testAABBWorldOSO();
  }

  inline bool Collider::testOO( const BSP* bsp_, const BSP::Model* model_ )
  {
     bsp_ = bsp;
     model = model_;

     trace = model->toBounds( 2.0f * EPSILON );
     span = world.getInters( trace, AABB::MAX_DIM );

     return testModelWorldOO();
  }

  inline void Collider::getOverlaps( const AABB& aabb_, Vector<Object*>* objects,
                                     Vector<Structure*>* structs, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    getWorldOverlaps( objects, structs );
  }

  inline void Collider::getIncludes( const AABB& aabb_, Vector<Object*>* objects, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    getWorldIncludes( objects );
  }

  inline void Collider::touchOverlaps( const AABB& aabb_, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    touchWorldOverlaps();
  }

  inline void Collider::translate( const Vec3& point_, const Vec3& move_, const Object* exclObj_ )
  {
    point = point_;
    move = move_;
    exclObj = exclObj_;

    trace.fromPointMove( point, move, 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    trimPointWorld();
  }

  inline void Collider::translate( const AABB& aabb_, const Vec3& move_, const Object* exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

  inline void Collider::translate( const Dynamic* obj_, const Vec3& move_ )
  {
    assert( obj_->cell != null );

    obj  = obj_;
    aabb = *obj;
    move = move_;
    exclObj = obj;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    trimAABBWorld();
  }

}
