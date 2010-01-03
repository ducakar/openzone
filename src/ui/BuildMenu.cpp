/*
 *  BuildMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "BuildMenu.h"

#include "matrix/Collider.h"
#include "matrix/Synapse.h"
#include "nirvana/Nirvana.h"
#include "client/Camera.h"
#include "Button.h"

namespace oz
{
namespace client
{
namespace ui
{

  static void createObject( Button* button )
  {
    if( !translator.classes.contains( button->label ) ) {
      return;
    }

    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.addObject( button->label, p );
    }
  }

  static void createCenterR0( Button* )
  {
    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R0 );
  }

  static void createCenterR90( Button* )
  {
    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R90 );
  }

  static void createCenterR180( Button* )
  {
    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R180 );
  }

  static void createCenterR270( Button* )
  {
    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R270 );
  }

  static void destroy( Button* )
  {
    Vec3 p = camera.bot == -1 ? camera.p : camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ );
    collider.translate( p, camera.at * 2.0f, camera.botObj );

    if( collider.hit.obj != null ) {
      Object *obj = const_cast<Object*>( collider.hit.obj );
      obj->life = 0.0f;
    }
  }

  BuildMenu::BuildMenu() : Frame( -105, -255, 200, 250 )
  {
    setFont( TITLE );
    setFontColor( 0xff, 0xff, 0xff );

    add( new Button( "SmallCrate", createObject, 90, 15 ), 5, -35 );
    add( new Button( "BigCrate", createObject, 90, 15 ), 5, -55 );
    add( new Button( "MetalBarrel", createObject, 90, 15 ), 5, -75 );
    add( new Button( "FirstAid", createObject, 90, 15 ), 5, -95 );
    add( new Button( "Wine", createObject, 90, 15 ), 5, -115 );
    add( new Button( "Bomb", createObject, 90, 15 ), 5, -135 );
    add( new Button( "Rifle", createObject, 90, 15 ), 5, -155 );

    add( new Button( "Goblin", createObject, 90, 15 ), 105, -35 );
    add( new Button( "Knight", createObject, 90, 15 ), 105, -55 );
    add( new Button( "Lord", createObject, 90, 15 ), 105, -75 );
    add( new Button( "Beast", createObject, 90, 15 ), 105, -95 );
    add( new Button( "Droid", createObject, 90, 15 ), 105, -115 );
    add( new Button( "DroidCommander", createObject, 90, 15 ), 105, -135 );
    add( new Button( "Raptor", createObject, 90, 15 ), 105, -155 );

    add( new Button( "House R0",   createCenterR0,   90, 15 ), 5, -195 );
    add( new Button( "House R90",  createCenterR90,  90, 15 ), 5, -215 );
    add( new Button( "House R180", createCenterR180, 90, 15 ), 105, -195 );
    add( new Button( "House R270", createCenterR270, 90, 15 ), 105, -215 );

    add( new Button( "DESTROY", destroy, 90, 15 ), 105, 5 );
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
      printCentered( 50, -10, "Create" );
    }
  }

}
}
}
