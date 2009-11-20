/*
 *  BuildMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

  static void createSmallCrate( Button* )
  {
    if( !translator.classes.contains( "SmallCrate" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "SmallCrate", p ) );
    }
  }

  static void createBigCrate( Button* )
  {
    if( !translator.classes.contains( "BigCrate" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "BigCrate", p ) );
    }
  }

  static void createMetalBarrel( Button* )
  {
    if( !translator.classes.contains( "MetalBarrel" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "MetalBarrel", p ) );
    }
  }

  static void createFirstAid( Button* )
  {
    if( !translator.classes.contains( "FirstAid" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "FirstAid", p ) );
    }
  }

  static void createGoblin( Button* )
  {
    if( !translator.classes.contains( "Goblin" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      Bot *bot = static_cast<Bot*>( translator.createObject( "Goblin", p ) );

      synapse.put( bot );
    }
  }

  static void createKnight( Button* )
  {
    if( !translator.classes.contains( "Knight" ) ) {
      return;
    }

    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      Bot *bot = static_cast<Bot*>( translator.createObject( "Knight", p ) );

      synapse.put( bot );
    }
  }

  static void createCenterR0( Button* )
  {
    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R0 );
  }

  static void createCenterR90( Button* )
  {
    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R90 );
  }

  static void createCenterR180( Button* )
  {
    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R180 );
  }

  static void createCenterR270( Button* )
  {
    Vec3 p  = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    p += camera.at * 2.0f;

    synapse.addStruct( "house", p, Structure::R270 );
  }

  static void destroy( Button* )
  {
    Vec3 p = camera.bot == null ? camera.p : camera.bot->p + camera.bot->camPos;
    collider.translate( p, camera.at * 2.0f, camera.bot );

    if( collider.hit.obj != null ) {
      synapse.remove( collider.hit.obj );
    }
  }

  BuildMenu::BuildMenu() : Frame( 100, 250 )
  {
    setFont( TITLE );
    setFontColor( 0xff, 0xff, 0xff );

    add( new Button( "SmallCrate", createSmallCrate, 90, 15 ), 5, -35 );
    add( new Button( "BigCrate", createBigCrate, 90, 15 ), 5, -55 );
    add( new Button( "MetalBarrel", createMetalBarrel, 90, 15 ), 5, -75 );
    add( new Button( "FirstAid", createFirstAid, 90, 15 ), 5, -95 );
    add( new Button( "Goblin", createGoblin, 90, 15 ), 5, -115 );
    add( new Button( "Knight", createKnight, 90, 15 ), 5, -135 );

    add( new Button( "House R0",   createCenterR0,   90, 15 ), 5, -155 );
    add( new Button( "House R90",  createCenterR90,  90, 15 ), 5, -175 );
    add( new Button( "House R180", createCenterR180, 90, 15 ), 5, -195 );
    add( new Button( "House R270", createCenterR270, 90, 15 ), 5, -215 );

    add( new Button( "DESTROY", destroy, 90, 15 ), 5, 5 );
  }

  void BuildMenu::onDraw()
  {
    Frame::onDraw();

    printCentered( 50, -10, "Create" );
  }

}
}
}
