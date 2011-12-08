/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Collider.hh
 */

#pragma once

#include "matrix/Orbis.hh"

namespace oz
{
namespace matrix
{

struct Material
{
  static const int VOID_BIT    = 0x00000001;
  static const int TERRAIN_BIT = 0x00000002;
  static const int WATER_BIT   = 0x00000004;
  static const int STRUCT_BIT  = 0x00000008;
  static const int SLICK_BIT   = 0x00000010;
  static const int LADDER_BIT  = 0x00000020;
  static const int OBJECT_BIT  = 0x00000040;
};

struct Hit
{
  Vec3  normal;

  Object* obj;
  Struct* str;
  Struct::Entity* entity;

  float ratio;

  int   material;
  int   medium;
  float waterDepth;
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
    Vec3   localDim;

    const Dynamic* obj;
    const Object* exclObj;
    const Struct* str;
    const Struct::Entity* entity;
    const BSP::Model* model;
    const BSP* bsp;

    int   flags;
    float margin;

    SBitset<BSP::MAX_BRUSHES> visitedBrushes;

    /**
     * Return true if brush was already visited and mark it visited.
     * @param index
     * @return
     */
    bool visitBrush( int index );

    bool overlapsAABBObj( const Object* sObj ) const;
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
    // terrain collision is penetration-safe
    void trimAABBTerraQuad( int x, int y );
    void trimAABBTerra();
    void trimAABBOrbis();

    void getOrbisOverlaps( Vector<Object*>* objects, Vector<Struct*>* structs );
    void getOrbisIncludes( Vector<Object*>* objects ) const;
    void touchOrbisOverlaps() const;
    void getEntityOverlaps( Vector<Object*>* objects );

  public:

    /**
     * Collision bitmask for Object::flags
     */
    int mask;

    /**
     * Collision feedback data
     */
    Hit hit;

    Collider();

    bool overlaps( const Point3& point, const Object* exclObj = null );
    // test for object collisions only (no structures or terrain)
    bool overlapsOO( const Point3& point, const Object* exclObj = null );
    // test for object and structure collisions only (no terain)
    bool overlapsOSO( const Point3& point, const Object* exclObj = null );

    bool overlaps( const AABB& aabb, const Object* exclObj = null );
    bool overlapsOO( const AABB& aabb, const Object* exclObj = null );
    bool overlapsOSO( const AABB& aabb, const Object* exclObj = null );

    bool overlaps( const Object* obj, const Object* exclObj = null );
    bool overlapsOO( const Object* obj, const Object* exclObj = null );
    bool overlapsOSO( const Object* obj, const Object* exclObj = null );

    bool overlapsOO( const Struct::Entity* entity, float margin = 0.0f );

    // fill given vectors with objects and structures overlapping with the AABB
    // if either vector is null the respecitve test is not performed
    void getOverlaps( const AABB& aabb, Vector<Object*>* objects, Vector<Struct*>* structs,
                      float eps = 0.0f );
    // fill given vector with objects included in the AABB
    void getIncludes( const AABB& aabb, Vector<Object*>* objects, float eps = 0.0f );
    // un-disable all dynamic objects that overlap (does not respect mask)
    void touchOverlaps( const AABB& aabb, float eps = 0.0f );

    void getOverlaps( const Struct::Entity* entity, Vector<Object*>* objects,
                      float margin = 0.0f );

    void translate( const Point3& point, const Vec3& move, const Object* exclObj = null );
    void translate( const AABB& aabb, const Vec3& move, const Object* exclObj = null );
    void translate( const Dynamic* obj, const Vec3& move );

};

extern Collider collider;

}
}
