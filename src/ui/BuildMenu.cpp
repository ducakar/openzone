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

    Vec3 p  = camera.p + camera.at * 2.0f;
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

    Vec3 p  = camera.p + camera.at * 2.0f;
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

    Vec3 p  = camera.p + camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      synapse.put( translator.createObject( "MetalBarrel", p ) );
    }
  }

  static void createGoblin( Button* )
  {
    if( !translator.classes.contains( "Goblin" ) ) {
      return;
    }

    Vec3 p  = camera.p + camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      Bot *bot = (Bot*) translator.createObject( "Goblin", p );

      synapse.put( bot );
      nirvana.add( new M_Walker( bot ) );
    }
  }

  static void createKnight( Button* )
  {
    if( !translator.classes.contains( "Knight" ) ) {
      return;
    }

    Vec3 p  = camera.p + camera.at * 2.0f;
    AABB bb = AABB( p, translator.classes.cachedValue()->dim );

    if( collider.test( bb ) ) {
      Bot *bot = (Bot*) translator.createObject( "Knight", p );

      synapse.put( bot );
      nirvana.add( new M_Walker( bot ) );
    }
  }

  static void destroy( Button* )
  {
    collider.translate( camera.p, camera.at * 2.0f, camera.bot );

    if( collider.hit.obj != null ) {
      synapse.remove( collider.hit.obj );
    }
  }

  BuildMenu::BuildMenu() : Frame( 100, 200 )
  {
    setFont( TITLE );
    setFontColor( 0xff, 0xff, 0xff );

    add( new Button( "SmallCrate", createSmallCrate, 90, 15 ), 5, -35 );
    add( new Button( "BigCrate", createBigCrate, 90, 15 ), 5, -55 );
    add( new Button( "MetalBarrel", createMetalBarrel, 90, 15 ), 5, -75 );
    add( new Button( "Goblin", createGoblin, 90, 15 ), 5, -95 );
    add( new Button( "Knight", createKnight, 90, 15 ), 5, -115 );

    add( new Button( "DESTROY", destroy, 90, 15 ), 5, 5 );
  }

  void BuildMenu::draw()
  {
    Frame::draw();

    printCentered( 50, -10, "Create" );
  }

}
}
}
