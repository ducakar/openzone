/*
 *  BuildMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/BuildMenu.hpp"

#include "matrix/Library.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Synapse.hpp"

#include "nirvana/Nirvana.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  BuildButton::BuildButton( const char* className, Callback* callback, int width, int height ) :
      Button( gettext( ( *library.classes.find( className ) )->title ), callback, width, height ),
      className( className )
  {}

  void BuildMenu::createObject( Button* button_ )
  {
    const BuildButton* button = static_cast<const BuildButton*>( button_ );
    const ObjectClass* const* clazz = library.classes.find( button->className );

    if( clazz == null ) {
      return;
    }

    Point3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;
    AABB bb = AABB( p, ( *clazz )->dim );

    if( !collider.overlaps( bb ) ) {
      synapse.addObject( button->className, p );
    }
  }

  BuildMenu::BuildMenu() : Frame( -8, -8, 240, 250, gettext( "Create" ) )
  {
    add( new BuildButton( "smallCrate", createObject, 110, 15 ), 5, -40 );
    add( new BuildButton( "bigCrate", createObject, 110, 15 ), 5, -60 );
    add( new BuildButton( "metalBarrel", createObject, 110, 15 ), 5, -80 );
    add( new BuildButton( "firstAid", createObject, 110, 15 ), 5, -100 );
    add( new BuildButton( "cvicek", createObject, 110, 15 ), 5, -120 );
    add( new BuildButton( "bomb", createObject, 110, 15 ), 5, -140 );
    add( new BuildButton( "droid_weapon.blaster", createObject, 110, 15 ), 5, -160 );
    add( new BuildButton( "droid_weapon.hyperblaster", createObject, 110, 15 ), 5, -180 );
    add( new BuildButton( "droid_weapon.chaingun", createObject, 110, 15 ), 5, -200 );
    add( new BuildButton( "droid_weapon.grenadeLauncher", createObject, 110, 15 ), 5, -220 );

    add( new BuildButton( "goblin", createObject, 110, 15 ), -115, -40 );
    add( new BuildButton( "knight", createObject, 110, 15 ), -115, -60 );
    add( new BuildButton( "lord", createObject, 110, 15 ), -115, -80 );
    add( new BuildButton( "beast", createObject, 110, 15 ), -115, -100 );
    add( new BuildButton( "droid", createObject, 110, 15 ), -115, -120 );
    add( new BuildButton( "droid.OOM-9", createObject, 110, 15 ), -115, -140 );
    add( new BuildButton( "raptor", createObject, 110, 15 ), -115, -160 );
    add( new BuildButton( "tank", createObject, 110, 15 ), -115, -180 );
  }

}
}
}
