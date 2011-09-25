/*
 *  FloraModule.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Module.hpp"
#include "matrix/Lua.hpp"

namespace oz
{

  class FloraModule : public Module
  {
    private:

      static const float DENSITY;
      static const float SPACING;

      // plants/m2
      float density;
      // dim * SPACING
      float spacing;
      int   number;

      void addTree( float x, float y );
      void addPlant( const char* type, float x, float y );

    public:

      void seed();

      virtual void init();

    private:

      static LuaAPI ozFloraGetDensity;
      static LuaAPI ozFloraSetDensity;
      static LuaAPI ozFloraGetSpacing;
      static LuaAPI ozFloraSetSpacing;
      static LuaAPI ozFloraGetNumber;
      static LuaAPI ozFloraSeed;

  };

  extern FloraModule floraModule;

}
