/*
 *  OBJ.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifdef OZ_BUILD_TOOLS

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  struct Face;

  class OBJ
  {
    private:

      static const int LINE_BUFFER_SIZE = 1024;

      String name;

      static char* skipSpaces( char* pos );
      static char* readWord( char* pos );

      static bool readVertexData( char* pos );
      static bool readFace( char* pos, int part );
      static bool loadMaterials( const String& path );

      static void loadOBJ( const char* name );
      static void freeOBJ();
      static void save( const char* fileName );

    public:

      static void prebuild( const char* name );

  };

}
}

#endif
