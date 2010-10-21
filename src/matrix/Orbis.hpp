/*
 *  Orbis.hpp
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world).
 *  The world should not be manipulated directly; use Synapse instead.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Sky.hpp"
#include "matrix/Terra.hpp"
#include "matrix/BSP.hpp"

#include "matrix/Structure.hpp"
#include "matrix/Dynamic.hpp"
#include "matrix/Particle.hpp"

#include "matrix/Translator.hpp"
#include "matrix/Lua.hpp"

namespace oz
{

  class Synapse;
  class Physics;

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float INV_SIZE;
    static const float RADIUS;

    Object*           firstObject;
    Particle*         firstPart;
    SVector<short, 6> structs;
  };

//  struct MarkerCell
//  {
//    SVector<short, 6> markers;
//  };

  class Orbis : public Bounds
  {
    friend class Synapse;
    friend class Physics;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      Cell               cells[Orbis::MAX][Orbis::MAX];
//      Cell               markerCells[Orbis::MAX][Orbis::MAX];
      Sky                sky;
      Terra              terra;
      Vector<BSP*>       bsps;
      Vector<Structure*> structs;
      Vector<Object*>    objects;
      Vector<Particle*>  parts;

    private:

      /*
       * Index reusing: when an entity is removed, there may still be references to it (from other
       * models or from render or audio subsystems); that's why every cycle all references must
       * be checked if the slot they're pointing at (all references should be indices of a slot
       * in Orbis::structures/objects/particles vectors). If the target slot is null, the referenced
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
      Vector<int>        partFreedIndices[2];

      // indices of slots that can be reused
      Vector<int>        strAvailableIndices;
      Vector<int>        objAvailableIndices;
      Vector<int>        partAvailableIndices;

    public:

      void requestBSP( int iBsp );

    private:

      bool position( Structure* str );
      void unposition( Structure* str );

      void position( Object* obj );
      void unposition( Object* obj );
      void reposition( Object* obj );

      void position( Particle* part );
      void unposition( Particle* part );
      void reposition( Particle* part );

      int  addStruct( const char* name, const Vec3& p, Structure::Rotation rot );
      int  addObject( const char* name, const Vec3& p );
      int  addPart( const Vec3& p, const Vec3& velocity, const Vec3& colour,
                    float restitution, float mass, float lifeTime );

      void remove( Structure* str );
      void remove( Object* obj );
      void remove( Particle* part );

    public:

      // get pointer to the cell the point is in
      Cell* getCell( float x, float y );
      Cell* getCell( const Vec3& p );

      // get indices of the cell the point is in
      Span getInters( float x, float y, float epsilon = 0.0f ) const;
      Span getInters( const Vec3& p, float epsilon = 0.0f ) const;

      // get indices of min and max cells which the area intersects
      Span getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                      float epsilon = 0.0f ) const;

      // get indices of min and max cells which the AABB intersects
      Span getInters( const AABB& bb, float epsilon = 0.0f ) const;

      // get indices of min and max cells which the bounds intersects
      Span getInters( const Bounds& bounds, float epsilon = 0.0f ) const;

      explicit Orbis();

      void init();
      void free();

      void load();
      void unload();

      void update();

      bool read( InputStream* istream );
      bool write( OutputStream* ostream );

  };

  extern Orbis orbis;

  inline Cell* Orbis::getCell( float x, float y )
  {
    int ix = int( ( x + Orbis::DIM ) * Cell::INV_SIZE );
    int iy = int( ( y + Orbis::DIM ) * Cell::INV_SIZE );

    ix = bound( ix, 0, Orbis::MAX - 1 );
    iy = bound( iy, 0, Orbis::MAX - 1 );

    return &cells[ix][iy];
  }

  inline Cell* Orbis::getCell( const Vec3& p )
  {
    return getCell( p.x, p.y );
  }

  inline Span Orbis::getInters( float x, float y, float epsilon ) const
  {
    return Span( max( int( ( x - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
                 max( int( ( y - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
                 min( int( ( x + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 ),
                 min( int( ( y + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 ) );
  }

  inline Span Orbis::getInters( const Vec3& p, float epsilon ) const
  {
    return getInters( p.x, p.y, epsilon );
  }

  inline Span Orbis::getInters( float minPosX, float minPosY,
                                float maxPosX, float maxPosY, float epsilon ) const
  {
    return Span( max( int( ( minPosX - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
                 max( int( ( minPosY - epsilon + Orbis::DIM ) * Cell::INV_SIZE ), 0 ),
                 min( int( ( maxPosX + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 ),
                 min( int( ( maxPosY + epsilon + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 ) );
  }

  inline Span Orbis::getInters( const AABB& bb, float epsilon ) const
  {
    return getInters( bb.p.x - bb.dim.x, bb.p.y - bb.dim.y,
                      bb.p.x + bb.dim.x, bb.p.y + bb.dim.y,
                      epsilon );
  }

  inline Span Orbis::getInters( const Bounds& bounds, float epsilon ) const
  {
    return getInters( bounds.mins.x, bounds.mins.y, bounds.maxs.x, bounds.maxs.y, epsilon );
  }

  inline bool Orbis::position( Structure* str )
  {
    str->setRotation( *bsps[str->iBsp], str->rot );

    Span span = getInters( *str, EPSILON );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        if( cells[x][y].structs.length() == cells[x][y].structs.capacity() ) {
          return false;
        }
      }
    }

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        assert( !cells[x][y].structs.contains( short( str->index ) ) );

        cells[x][y].structs.add( short( str->index ) );
      }
    }
    return true;
  }

  inline void Orbis::unposition( Structure* str )
  {
    Span span = getInters( *str, EPSILON );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        assert( cells[x][y].structs.contains( short( str->index ) ) );

        cells[x][y].structs.excludeUO( short( str->index ) );
      }
    }
  }

  inline void Orbis::position( Object* obj )
  {
    assert( obj->cell == null );

    Cell* cell = getCell( obj->p );

    obj->cell = cell;
    obj->next[0] = cell->firstObject;
    obj->prev[0] = null;

    if( cell->firstObject == null ) {
      cell->firstObject = obj;
    }
    else {
      cell->firstObject->prev[0] = obj;
      cell->firstObject = obj;
    }
  }

  inline void Orbis::unposition( Object* obj )
  {
    assert( obj->cell != null );

    Cell* cell = obj->cell;

    obj->cell = null;

    if( obj->prev[0] == null ) {
      cell->firstObject = obj->next[0];
    }
    else {
      obj->prev[0]->next[0] = obj->next[0];
    }
    if( obj->next[0] != null ) {
      obj->next[0]->prev[0] = obj->prev[0];
    }
  }

  inline void Orbis::reposition( Object* obj )
  {
    assert( obj->cell != null );

    Cell* oldCell = obj->cell;
    Cell* newCell = getCell( obj->p );

    if( newCell != oldCell ) {
      if( obj->prev[0] == null ) {
        oldCell->firstObject = obj->next[0];
      }
      else {
        obj->prev[0]->next[0] = obj->next[0];
      }
      if( obj->next[0] != null ) {
        obj->next[0]->prev[0] = obj->prev[0];
      }

      obj->cell = newCell;
      obj->next[0] = newCell->firstObject;
      obj->prev[0] = null;

      if( newCell->firstObject == null ) {
        newCell->firstObject = obj;
      }
      else {
        newCell->firstObject->prev[0] = obj;
        newCell->firstObject = obj;
      }
    }
  }

  inline void Orbis::position( Particle* part )
  {
    assert( part->cell == null );

    Cell* cell = getCell( part->p );

    part->cell = cell;
    part->next[0] = cell->firstPart;
    part->prev[0] = null;

    if( cell->firstPart == null ) {
      cell->firstPart = part;
    }
    else {
      cell->firstPart->prev[0] = part;
      cell->firstPart = part;
    }
  }

  inline void Orbis::unposition( Particle* part )
  {
    assert( part->cell != null );

    Cell* cell = part->cell;

    part->cell = null;

    if( part->prev[0] == null ) {
      cell->firstPart = part->next[0];
    }
    else {
      part->prev[0]->next[0] = part->next[0];
    }
    if( part->next[0] != null ) {
      part->next[0]->prev[0] = part->prev[0];
    }
  }

  inline void Orbis::reposition( Particle* part )
  {
    assert( part->cell != null );

    Cell* oldCell = part->cell;
    Cell* newCell = getCell( part->p );

    if( newCell != oldCell ) {
      if( part->prev[0] == null ) {
        oldCell->firstPart = part->next[0];
      }
      else {
        part->prev[0]->next[0] = part->next[0];
      }
      if( part->next[0] != null ) {
        part->next[0]->prev[0] = part->prev[0];
      }

      part->cell = newCell;
      part->next[0] = newCell->firstPart;
      part->prev[0] = null;

      if( newCell->firstPart == null ) {
        newCell->firstPart = part;
      }
      else {
        newCell->firstPart->prev[0] = part;
        newCell->firstPart = part;
      }
    }
  }

  inline void Orbis::requestBSP( int iBsp ) {
    if( bsps[iBsp] == null ) {
      bsps[iBsp] = new BSP( translator.bsps[iBsp].name );
    }
  }

  inline int Orbis::addStruct( const char* name, const Vec3& p, Structure::Rotation rot )
  {
    int index;

    if( strAvailableIndices.isEmpty() ) {
      index = structs.length();
      structs.pushLast( translator.createStruct( index, name, p, rot ) );
    }
    else {
      index = strAvailableIndices.popLast();
      structs[index] = translator.createStruct( index, name, p, rot );
    }
    return index;
  }

  // has to be reentrant, can be called again from translator.createObject
  inline int Orbis::addObject( const char* name, const Vec3& p )
  {
    int index;

    if( objAvailableIndices.isEmpty() ) {
      index = objects.length();
      // reserve slot so reentrant calls cannot occupy it again
      objects.pushLast( null );
    }
    else {
      index = objAvailableIndices.popLast();
    }
    // objects vector may relocate during createObject call, we must use this workaround
    Object* obj = translator.createObject( index, name, p );
    objects[index] = obj;

    if( objects[index]->flags & Object::LUA_BIT ) {
      lua.registerObject( index );
    }
    return index;
  }

  inline int Orbis::addPart( const Vec3& p, const Vec3& velocity, const Vec3& colour,
                             float restitution, float mass, float lifeTime )
  {
    int index;

    if( partAvailableIndices.isEmpty() ) {
      index = parts.length();
      parts.pushLast( new Particle( index, p, velocity, colour, restitution, mass, lifeTime ) );
    }
    else {
      index = partAvailableIndices.popLast();
      parts[index] = new Particle( index, p, velocity, colour, restitution, mass, lifeTime );
    }
    return index;
  }

  inline void Orbis::remove( Structure* str )
  {
    assert( str->index >= 0 );

    strFreedIndices[freeing].pushLast( str->index );
    structs[str->index] = null;
    str->index = -1;
  }

  inline void Orbis::remove( Object* obj )
  {
    assert( obj->index >= 0 );
    assert( obj->cell == null );

    if( obj->flags & Object::LUA_BIT ) {
      lua.unregisterObject( obj->index );
    }
    objFreedIndices[freeing].pushLast( obj->index );
    objects[obj->index] = null;
    obj->index = -1;
  }

  inline void Orbis::remove( Particle* part )
  {
    assert( part->index >= 0 );
    assert( part->cell == null );

    partFreedIndices[freeing].pushLast( part->index );
    parts[part->index] = null;
    part->index = -1;
  }

}
