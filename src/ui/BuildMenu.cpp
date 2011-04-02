/*
 *  BuildMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/BuildMenu.hpp"

#include "matrix/Translator.hpp"
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

  void BuildMenu::createObject( Button* button )
  {
    const ObjectClass* const* clazz = translator.classes.find( button->label );
    if( clazz == null ) {
      return;
    }

    Point3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;
    AABB bb = AABB( p, ( *clazz )->dim );

    if( !collider.overlaps( bb ) ) {
      synapse.addObject( button->label, p );
    }
  }

  BuildMenu::BuildMenu() : Frame( -8, -8, 240, 250, gettext( "Create" ) )
  {
    setFont( Font::SANS );

    add( new Button( "smallCrate", createObject, 110, 15 ), 5, -40 );
    add( new Button( "bigCrate", createObject, 110, 15 ), 5, -60 );
    add( new Button( "metalBarrel", createObject, 110, 15 ), 5, -80 );
    add( new Button( "firstAid", createObject, 110, 15 ), 5, -100 );
    add( new Button( "cvicek", createObject, 110, 15 ), 5, -120 );
    add( new Button( "bomb", createObject, 110, 15 ), 5, -140 );
    add( new Button( "droidRifle", createObject, 110, 15 ), 5, -160 );
    add( new Button( "goblinAxe", createObject, 110, 15 ), 5, -180 );

    add( new Button( "goblin", createObject, 110, 15 ), -115, -40 );
    add( new Button( "knight", createObject, 110, 15 ), -115, -60 );
    add( new Button( "lord", createObject, 110, 15 ), -115, -80 );
    add( new Button( "beast", createObject, 110, 15 ), -115, -100 );
    add( new Button( "droid", createObject, 110, 15 ), -115, -120 );
    add( new Button( "droidCommander", createObject, 110, 15 ), -115, -140 );
    add( new Button( "raptor", createObject, 110, 15 ), -115, -160 );
    add( new Button( "tank", createObject, 110, 15 ), -115, -180 );
  }

}
}
}
