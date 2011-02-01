/*
 *  Orbis.hpp
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world).
 *  The world should not be manipulated directly; use Synapse instead.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/BSP.hpp"
#include "matrix/Terra.hpp"
#include "matrix/Sky.hpp"

#include "matrix/Struct.hpp"
#include "matrix/Dynamic.hpp"
#include "matrix/Particle.hpp"

namespace oz
{

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float INV_SIZE;
    static const float RADIUS;

    List<Object>      objects;
    List<Particle>    particles;
    SVector<short, 6> structs;
  };

  class Orbis : public Bounds
  {
    friend class Synapse;
    friend class Physics;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      Cell              cells[Orbis::MAX][Orbis::MAX];

      Vector<BSP*>      bsps;
      Vector<Struct*>   structs;
      Vector<Object*>   objects;
      Vector<Particle*> parts;

      Terra             terra;
      Sky               sky;

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

    private:

      bool position( Struct* str );
      void unposition( Struct* str );

      void position( Object* obj );
      void unposition( Object* obj );
      void reposition( Object* obj );

      void position( Particle* part );
      void unposition( Particle* part );
      void reposition( Particle* part );

      int  addStruct( const char* name, const Point3& p, Struct::Rotation rot );
      int  addObject( const char* name, const Point3& p );
      int  addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                    float restitution, float mass, float lifeTime );

      void remove( Struct* str );
      void remove( Object* obj );
      void remove( Particle* part );

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

  inline Cell* Orbis::getCell( const Point3& p )
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

  inline Span Orbis::getInters( const Point3& p, float epsilon ) const
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

}
