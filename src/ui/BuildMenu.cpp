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

#include "ui/Button.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  static void createObject( Button* button )
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

  static void destroy( Button* )
  {
    Point3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    collider.translate( p, camera.at * 2.0f, camera.botObj );

    if( collider.hit.obj != null ) {
      Object* obj = const_cast<Object*>( collider.hit.obj );
      obj->life = 0.0f;
    }
  }

  BuildMenu::BuildMenu() : Frame( -205, -255, 220, 250 )
  {
    setFont( Font::TITLE );

    add( new Button( "small crate", createObject, 100, 15 ), 5, -35 );
    add( new Button( "big crate", createObject, 100, 15 ), 5, -55 );
    add( new Button( "metal barrel", createObject, 100, 15 ), 5, -75 );
    add( new Button( "first aid", createObject, 100, 15 ), 5, -95 );
    add( new Button( "cvicek", createObject, 100, 15 ), 5, -115 );
    add( new Button( "bomb", createObject, 100, 15 ), 5, -135 );
    add( new Button( "droid rifle", createObject, 100, 15 ), 5, -155 );
    add( new Button( "goblin axe", createObject, 100, 15 ), 5, -185 );

    add( new Button( "goblin", createObject, 100, 15 ), 115, -35 );
    add( new Button( "knight", createObject, 100, 15 ), 115, -55 );
    add( new Button( "lord", createObject, 100, 15 ), 115, -75 );
    add( new Button( "beast", createObject, 100, 15 ), 115, -95 );
    add( new Button( "droid", createObject, 100, 15 ), 115, -115 );
    add( new Button( "droid commander", createObject, 100, 15 ), 115, -135 );
    add( new Button( "raptor", createObject, 100, 15 ), 115, -155 );
    add( new Button( "tank", createObject, 100, 15 ), 115, -185 );

    add( new Button( "DESTROY", destroy, 100, 15 ), 115, 5 );
  }

  bool BuildMenu::onMouseEvent()
  {
    if( mouse.doShow ) {
      return Frame::onMouseEvent();
    }
    return false;
  }

  void BuildMenu::onDraw()
  {
    if( mouse.doShow ) {
      Frame::onDraw();
      printCentred( 50, -10, "Create" );
    }
  }

}
}
}
