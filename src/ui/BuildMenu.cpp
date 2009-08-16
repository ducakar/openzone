/*
 *  BuildMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "BuildMenu.h"

#include "matrix/Collider.h"
#include "matrix/Synapse.h"
#include "client/Camera.h"
#include "Button.h"

namespace oz
{
namespace client
{
namespace ui
{

  static void createSmallCrate()
  {
    if( !translator.classes.contains( "SmallCrate" ) ) {
      return;
    }

    Vec3 p  = camera.p + camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "SmallCrate", p ) );
    }
  }

  BuildMenu::BuildMenu() : Frame( 200, 300 )
  {
    Button *smallCrateButton = new Button( "SmallCrate", 90, 15 );
    smallCrateButton->setCallback( createSmallCrate );

    add( smallCrateButton, 5, -20 );
  }

}
}
}
