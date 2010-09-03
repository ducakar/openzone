/*
 *  Collider.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Orbis.hpp"

namespace oz
{

  struct Hit
  {
    Vec3  normal;
    float ratio;

    const Object*      obj;
    const Structure*   str;
    const BSP::Entity* entity;

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

      SBitset<BSP::MAX_BRUSHES> visitedBrushes;

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

      bool overlapsPointBrush( const BSP::Brush* brush ) const;
      bool overlapsPointNode( int nodeIndex );
      bool overlapsPointModels() const;
      bool overlapsPointOrbis();
      bool overlapsPointOrbisOO();
      bool overlapsPointOrbisOSO();

      bool trimTerraQuad( int x, int y );
      void trimPointTerra();

      void trimPointVoid();
      void trimPointObj( const Object* sObj );
      void trimPointBrush( const BSP::Brush* brush );
      void trimPointNode( int nodeIndex );
      void trimPointModels();
      void trimPointOrbis();

      bool overlapsAABBBrush( const BSP::Brush* brush ) const;
      bool overlapsAABBNode( int nodeIndex );
      bool overlapsAABBModels() const;
      bool overlapsAABBOrbis();
      bool overlapsAABBOrbisOO();
      bool overlapsAABBOrbisOSO();

      void trimAABBVoid();
      void trimAABBObj( const Object* sObj );
      void trimAABBBrush( const BSP::Brush* brush );
      void trimAABBWater( const BSP::Brush* brush );
      void trimAABBLadder( const BSP::Brush* brush );
      void trimAABBNode( int nodeIndex );
      void trimAABBModels();
      void trimAABBOrbis();

      void getOrbisOverlaps( Vector<Object*>* objects, Vector<Structure*>* structs );
      void getOrbisIncludes( Vector<Object*>* objects ) const;
      void touchOrbisOverlaps() const;
      void getModelOverlaps( Vector<Object*>* objects );

    public:

      /**
       * Collision feedback data
       */
      Hit  hit;
      /**
       * Collision bitmask for Object::flags
       */
      int  mask;

      explicit Collider();

      bool overlaps( const Vec3& point, const Object* exclObj = null );
      // test for object collisions only (no structures or terrain)
      bool overlapsOO( const Vec3& point, const Object* exclObj = null );
      // test for object and structure collisions only (no terain)
      bool overlapsOSO( const Vec3& point, const Object* exclObj = null );

      bool overlaps( const AABB& aabb, const Object* exclObj = null );
      bool overlapsOO( const AABB& aabb, const Object* exclObj = null );
      bool overlapsOSO( const AABB& aabb, const Object* exclObj = null );

      // fill given vectors with objects and structures overlapping with the AABB
      // if either vector is null the respecitve test is not performed
      void getOverlaps( const AABB& aabb, Vector<Object*>* objects, Vector<Structure*>* structs,
                        float eps = 0.0f );
      // fill given vector with objects included in the AABB
      void getIncludes( const AABB& aabb, Vector<Object*>* objects, float eps = 0.0f );
      void touchOverlaps( const AABB& aabb, float eps = 0.0f );
      void getOverlaps( const BSP* bsp, const BSP::Model* model, Vector<Object*>* objects,
                        float eps = 0.0f );

      void translate( const Vec3& point, const Vec3& move, const Object* exclObj = null );
      void translate( const AABB& aabb, const Vec3& move, const Object* exclObj = null );
      void translate( const Dynamic* obj, const Vec3& move );

  };

  extern Collider collider;

  inline bool Collider::overlaps( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return overlapsPointOrbis();
  }

  inline bool Collider::overlapsOO( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return overlapsPointOrbisOO();
  }

  inline bool Collider::overlapsOSO( const Vec3& point_, const Object* exclObj_ )
  {
    point = point_;
    exclObj = exclObj_;

    span = world.getInters( point, AABB::MAX_DIM );

    return overlapsPointOrbisOSO();
  }

  inline bool Collider::overlaps( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbis();
  }

  inline bool Collider::overlapsOO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOO();
  }

  inline bool Collider::overlapsOSO( const AABB& aabb_, const Object* exclObj_ )
  {
    aabb = aabb_;
    exclObj = exclObj_;

    trace = aabb.toBounds( 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    return overlapsAABBOrbisOSO();
  }

  inline void Collider::getOverlaps( const AABB& aabb_, Vector<Object*>* objects,
                                     Vector<Structure*>* structs, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    getOrbisOverlaps( objects, structs );
  }

  inline void Collider::getIncludes( const AABB& aabb_, Vector<Object*>* objects, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    getOrbisIncludes( objects );
  }

  inline void Collider::touchOverlaps( const AABB& aabb_, float eps )
  {
    aabb = aabb_;
    exclObj = null;

    trace = aabb.toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    touchOrbisOverlaps();
  }

  inline void Collider::getOverlaps( const BSP* bsp_, const BSP::Model* model_,
                                     Vector<Object*>* objects, float eps )
  {
    bsp = bsp_;
    model = model_;

    trace = model->toBounds( eps );
    span = world.getInters( trace, AABB::MAX_DIM );

    getModelOverlaps( objects );
  }

  inline void Collider::translate( const Vec3& point_, const Vec3& move_, const Object* exclObj_ )
  {
    point = point_;
    move = move_;
    exclObj = exclObj_;

    trace.fromPointMove( point, move, 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    trimPointOrbis();
  }

  inline void Collider::translate( const AABB& aabb_, const Vec3& move_, const Object* exclObj_ )
  {
    obj  = null;
    aabb = aabb_;
    move = move_;
    exclObj = exclObj_;

    trace.fromAABBMove( aabb, move, 2.0f * EPSILON );
    span = world.getInters( trace, AABB::MAX_DIM );

    trimAABBOrbis();
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

    trimAABBOrbis();
  }

}
