/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file matrix/Orbis.hh
 */

#pragma once

#include "matrix/Caelum.hh"
#include "matrix/Terra.hh"
#include "matrix/Struct.hh"
#include "matrix/Dynamic.hh"
#include "matrix/Frag.hh"

namespace oz
{
namespace matrix
{

struct Cell
{
  static const     int   SIZEI    = 16;
  static constexpr float SIZE     = float( SIZEI );
  static constexpr float INV_SIZE = 1.0f / SIZE;

  SVector<short, 6> structs;
  List<Object>      objects;
  List<Frag>        frags;
};

/**
 * Matrix data structure for world (terrain, all structures and objects in the world).
 * The world should not be manipulated directly; use Synapse instead.
 */
class Orbis : public Bounds
{
  friend class Synapse;
  friend class Physics;

  public:

    // # of cells on each (x, y) axis
    static const     int   CELLS = 4096 / Cell::SIZEI;
    static constexpr float DIM   = Cell::SIZE * CELLS / 2.0f;

    Caelum            caelum;
    Terra             terra;

    Vector<Struct*>   structs;
    Vector<Object*>   objects;
    Vector<Frag*>     frags;

    Cell              cells[Orbis::CELLS][Orbis::CELLS];

  private:

    /*
     * Index reusing: when an entity is removed, there may still be references to it (from other
     * models or from render or sound subsystems); that's why every cycle all references must
     * be checked if the slot they're pointing at (all references should be indices of a slot
     * in Orbis::structures/objects/fragments vectors). If the target slot is null, the referenced
     * entity doesn't exist any more, so reference must be cleared. To make sure all references
     * can be checked that way, a full world update must pass before a slot is reused. Otherwise
     * an entity may be removed and immediately after that another added into it's slot; when an
     * another entity would retrieve the target entity via the reference: 1) it wouldn't get the
     * expected entity but a new one; that may result in program crash if the new one is not of
     * the same type, 2) it wouldn't detect the old entity has been removed/destroyed/whatever;
     * that may pose a big problem to rendering and audio subsystems as those must clear
     * models/audio objects of removed world objects.
     */

    int freeing;
    int waiting;

    // [freeing]: vector for indices that are currently being freed
    // [waiting]: indices that have been freed previous cycle; those can be reused next time
    Vector<int>        strFreedIndices[2];
    Vector<int>        objFreedIndices[2];
    Vector<int>        fragFreedIndices[2];

    // indices of slots that can be reused
    Vector<int>        strAvailableIndices;
    Vector<int>        objAvailableIndices;
    Vector<int>        fragAvailableIndices;

  private:

    bool position( Struct* str );
    void unposition( Struct* str );

    void position( Object* obj );
    void unposition( Object* obj );
    void reposition( Object* obj );

    void position( Frag* frag );
    void unposition( Frag* frag );
    void reposition( Frag* frag );

    int  addStruct( const BSP* bsp, const Point3& p, Heading heading );
    int  addObject( const ObjectClass* clazz, const Point3& p, Heading heading );
    int  addFrag( const FragPool* pool, const Point3& p, const Vec3& velocity );

    void remove( Struct* str );
    void remove( Object* obj );
    void remove( Frag* frag );

  public:

    // get pointer to the cell the point is in
    Cell* getCell( float x, float y );
    Cell* getCell( const Point3& p );

    // get indices of the cell the point is in
    Span getInters( float x, float y, float epsilon = 0.0f ) const;
    Span getInters( const Point3& p, float epsilon = 0.0f ) const;

    // get indices of min and max cells which the area intersects
    Span getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                    float epsilon = 0.0f ) const;

    // get indices of min and max cells which the AABB intersects
    Span getInters( const AABB& bb, float epsilon = 0.0f ) const;

    // get indices of min and max cells which the bounds intersects
    Span getInters( const Bounds& bounds, float epsilon = 0.0f ) const;

    void update();

    void read( InputStream* istream );
    void write( BufferStream* ostream ) const;

    void load();
    void unload();

    void init();
    void free();

};

extern Orbis orbis;

OZ_ALWAYS_INLINE
inline Cell* Orbis::getCell( float x, float y )
{
  int ix = int( ( x + Orbis::DIM ) * Cell::INV_SIZE );
  int iy = int( ( y + Orbis::DIM ) * Cell::INV_SIZE );

  ix = clamp( ix, 0, Orbis::CELLS - 1 );
  iy = clamp( iy, 0, Orbis::CELLS - 1 );

  return &cells[ix][iy];
}

OZ_ALWAYS_INLINE
inline Cell* Orbis::getCell( const Point3& p )
{
  return getCell( p.x, p.y );
}

OZ_ALWAYS_INLINE
inline Span Orbis::getInters( float x, float y, float epsilon ) const
{
  return { max( int( ( x - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
           max( int( ( y - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
           min( int( ( x + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::CELLS - 1 ),
           min( int( ( y + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::CELLS - 1 ) };
}

OZ_ALWAYS_INLINE
inline Span Orbis::getInters( const Point3& p, float epsilon ) const
{
  return getInters( p.x, p.y, epsilon );
}

OZ_ALWAYS_INLINE
inline Span Orbis::getInters( float minPosX, float minPosY,
                              float maxPosX, float maxPosY, float epsilon ) const
{
  return { max( int( ( minPosX - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
           max( int( ( minPosY - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
           min( int( ( maxPosX + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::CELLS - 1 ),
           min( int( ( maxPosY + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::CELLS - 1 ) };
}

OZ_ALWAYS_INLINE
inline Span Orbis::getInters( const AABB& bb, float epsilon ) const
{
  return getInters( bb.p.x - bb.dim.x, bb.p.y - bb.dim.y,
                    bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
                    epsilon );
}

OZ_ALWAYS_INLINE
inline Span Orbis::getInters( const Bounds& bounds, float epsilon ) const
{
  return getInters( bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
}

}
}
