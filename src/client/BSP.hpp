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
      int            shaderId;

#ifdef OZ_BUILD_TOOLS
      // prebuild
      static void loadQBSP( const char* path );
      static void freeQBSP();
      static void optimise();
      static void save( const char* path );
#endif

      void playSound( const Struct::Entity* entity, int sample ) const;

    public:

      bool isLoaded;

#ifdef OZ_BUILD_TOOLS
      // create ozcBSP from a Quake 3 QBSP (matrix BSP must be loaded)
      static void prebuild( const char* name );
#endif

      explicit BSP( int id );
      ~BSP();

      void load();
      void draw( const Struct* str, int mask ) const;
      void play( const Struct* str ) const;

  };

}
}
