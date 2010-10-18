/*
 *  ozPrebuildData.cpp
 *
 *  Prebuild data
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Matrix.hpp"

#include <SDL_main.h>

using namespace oz;

int main( int, char** )
{
  matrix.init();

  for( int i = 0; i < translator.terras.length(); ++i ) {
    const String& name = translator.terras[i].name;

    try {
      orbis.terra.load( name, 0 );
      orbis.terra.save( "terra/" + name + ".ozTerra" );
    }
    catch( const Exception& e ) {
    }
  }

  for( int i = 0; i < translator.bsps.length(); ++i ) {
    const String& name = translator.bsps[i].name;

    BSP* bsp = new BSP( name, 0 );
    bsp->save( "maps/" + name + ".ozBSP" );
    bsp->freeQBSP();
    delete bsp;
  }

  matrix.free();

  return 0;
}
