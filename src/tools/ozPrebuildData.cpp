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

#include "client/Terra.hpp"
#include "client/OBJ.hpp"

#include <SDL_main.h>

using namespace oz;

int main( int, char** )
{
  matrix.init();

  foreach( terra, translator.terras.citer() ) {
    try {
      orbis.terra.prebuild( terra->name );
      client::terra.prebuild();
    }
    catch( const Exception& ) {
    }
  }

  foreach( bsp, translator.bsps.citer() ) {
    BSP::prebuild( bsp->name );
  }

  foreach( model, translator.models.citer() ) {
    try {
      client::OBJ::prebuild( model->name );
    }
    catch( const Exception& ) {
    }
  }

  matrix.free();

  return 0;
}
