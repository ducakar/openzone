/*
 *  SMM.hpp
 *
 *  Single mesh model
 *  Common model format that all simple models are compiled to.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class SMM
  {
    private:

      String name;
      Mesh   mesh;

    public:

      bool isLoaded;

      static void prebuild( const char* name );

      explicit SMM( const char* name );
      ~SMM();

      void load();
      void draw() const;

  };

}
}
