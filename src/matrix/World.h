/*
 *  World.h
 *
 *  Matrix data structure for world (terrain, all structures and objects in the world)
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Sky.h"
#include "Terrain.h"
#include "BSP.h"

#include "Structure.h"
#include "DynObject.h"
#include "Particle.h"

#include "Translator.h"

namespace oz
{

  class Synapse;

  struct Cell
  {
    static const int   SIZEI = 16;
    static const float SIZE;
    static const float RADIUS;

    Vector<int>        structures;
    DList<Object, 0>   objects;
    DList<Particle, 0> particles;
  };

  class World : public Bounds
  {
    friend class Synapse;

    public:

      // # of cells on each (x, y) axis
      static const int   MAX = 128;
      static const float DIM;

      Sky                sky;
      Terrain            terra;
      Cell               cells[World::MAX][World::MAX];
      Vector<BSP*>       bsps;
      Vector<Structure*> structures;
      Vector<Object*>    objects;
      Vector<Particle*>  particles;

    private:

      Vector<int>        strAvailableIndices;
      Vector<int>        objAvailableIndices;
      Vector<int>        partAvailableIndices;

      Vector<int>        strPendingIndices;
      Vector<int>        objPendingIndices;
      Vector<int>        partPendingIndices;

      void position( Structure *str );
      void unposition( Structure *str );

      void position( Object *obj );
      void unposition( Object *obj );
      void reposition( Object *obj );

      void position( Particle *part );
      void unposition( Particle *part );
      void reposition( Particle *part );

      void put( Structure *str );
      void put( Object *obj );
      void put( Particle *part );

      void cut( Structure *str );
      void cut( Object *obj );
      void cut( Particle *part );

    public:

      void init();
      void free();

      void load();
      void unload();

      void update();

      bool read( InputStream *istream );
      bool write( OutputStream *ostream );

  };

  extern World world;

}
