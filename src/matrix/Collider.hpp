/*
 *  Collider.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"

namespace oz
{

  struct Hit
  {
    Vec3  normal;

    const Object* obj;
    const Struct* str;
    const Struct::Entity* entity;

    float ratio;

    int   material;
    float waterDepth;
    bool  inWater;
    bool  onLadder;
  };

  class Collider
  {
    private:

      static const Vec3 normals[];

      Span   span;
      Bounds trace;
      Vec3   move;

      AABB   aabb;

      Point3 startPos;
      Point3 endPos;

      const Dynamic* obj;
      const Object* exclObj;
      const Struct* str;
      const Struct::Entity* entity;
      const BSP::Model* model;
      const BSP* bsp;

      SBitset<BSP::MAX_BRUSHES> visitedBrushes;

      /**
       * Return if brush was already visited and mark it visited.
       * @param index
       * @return
       */
      bool visitBrush( int index );

      bool overlapsAABBBrush( const BSP::Brush* brush ) const;
      bool overlapsAABBNode( int nodeIndex );
      bool overlapsAABBEntities();
      bool overlapsAABBOrbis();
      bool overlapsAABBOrbisOO();
      bool overlapsAABBOrbisOSO();

      bool overlapsEntityOrbisOO();

      void trimAABBVoid();
      void trimAABBObj( const Object* sObj );
      void trimAABBBrush( const BSP::Brush* brush );
      void trimAABBWater( const BSP::Brush* brush );
      void trimAABBLadder( const BSP::Brush* brush );
      void trimAABBNode( int nodeIndex );
      void trimAABBEntities();
      bool trimAABBTerraQuad( int x, int y );
      void trimAABBTerra();
      void trimAABBOrbis();

      void getOrbisOverlaps( Vector<Object*>* objects, Vector<Struct*>* structs );
      void getOrbisIncludes( Vector<Object*>* objects ) const;
      void touchOrbisOverlaps() const;
      void getEntityOverlaps( Vector<Object*>* objects, float margin );

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

      bool overlaps( const Point3& point, const Object* exclObj = null );
      // test for object collisions only (no structures or terrain)
      bool overlapsOO( const Point3& point, const Object* exclObj = null );
      // test for object and structure collisions only (no terain)
      bool overlapsOSO( const Point3& point, const Object* exclObj = null );

      bool overlaps( const AABB& aabb, const Object* exclObj = null );
      bool overlapsOO( const AABB& aabb, const Object* exclObj = null );
      bool overlapsOSO( const AABB& aabb, const Object* exclObj = null );

      bool overlapsOO( const Struct::Entity* entity );

      // fill given vectors with objects and structures overlapping with the AABB
      // if either vector is null the respecitve test is not performed
      void getOverlaps( const AABB& aabb, Vector<Object*>* objects, Vector<Struct*>* structs,
                        float eps = 0.0f );
      // fill given vector with objects included in the AABB
      void getIncludes( const AABB& aabb, Vector<Object*>* objects, float eps = 0.0f );
      void touchOverlaps( const AABB& aabb, float eps = 0.0f );

      void getOverlaps( const Struct::Entity* entity, Vector<Object*>* objects,
                        float margin = 0.0f );

      void translate( const Point3& point, const Vec3& move, const Object* exclObj = null );
      void translate( const AABB& aabb, const Vec3& move, const Object* exclObj = null );
      void translate( const Dynamic* obj, const Vec3& move );

  };

  extern Collider collider;

}
