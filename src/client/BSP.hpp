/*
 *  BSP.hpp
 *
 *  BSP level rendering class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Struct.hpp"
#include "matrix/BSP.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class BSP
  {
    private:

      static const int LIGHTMAP_DIM  = 128;
      static const int LIGHTMAP_BPP  = 3;
      static const int LIGHTMAP_SIZE = LIGHTMAP_DIM * LIGHTMAP_DIM * LIGHTMAP_BPP;

    private:

      const oz::BSP* bsp;
      DArray<Mesh>   meshes;
      int            flags;

      // prebuild
      static void loadQBSP( const char* fileName );
      static void freeQBSP();
      static void optimise();
      static void save( const char* file );

    public:

      bool isLoaded;

      // create ozcBSP from a Quake 3 QBSP (matrix BSP must be loaded)
      static void prebuild( const char* name );

      explicit BSP( int bspIndex );
      ~BSP();

      void load();
      void draw( const Struct* str, int mask ) const;

  };

}
}
