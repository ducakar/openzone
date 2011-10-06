/*
 *  GalileoModule.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Module.hpp"
#include "client/Lua.hpp"

#include "client/modules/GalileoFrame.hpp"

namespace oz
{
namespace client
{

  class GalileoModule : public Module
  {
#ifndef OZ_TOOLS
    private:

      ui::GalileoFrame* galileoFrame;

    public:

      Vector<Point3> markers;

      GalileoModule();

      virtual void read( InputStream* istream );
      virtual void write( OutputStream* ostream ) const;

      virtual void load();
      virtual void unload();

      virtual void init();

    private:

      OZ_LUA_API( ozGalileoAddMarker );

#else
    public:

      virtual void prebuild();

#endif
  };

  extern GalileoModule galileoModule;

}
}
