/*
 *  InventoryMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "InventoryMenu.h"

#include "client/Camera.h"
#include "client/Context.h"
#include "client/Shape.h"
#include "client/Render.h"

namespace oz
{
namespace client
{
namespace ui
{

  InventoryMenu::InventoryMenu() : Frame( 512, 64 + 20 + 40 )
  {
    setFont( TITLE );
    setFontColor( 0xff, 0xff, 0xff );
  }

  void InventoryMenu::onMouseEvent()
  {
    Frame::onMouseEvent();
  }

  void InventoryMenu::onDraw()
  {
    if( camera.bot == null ) {
      return;
    }

    Frame::onDraw();

    printCentered( 256, -10, "Inventory" );

    glPushMatrix();
    glTranslatef( 100.0f, 100.0f, 0.0f );

    glEnable( GL_TEXTURE_2D );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glScalef( 100.0f, 100.0f, 100.0f );

    foreach( i, camera.bot->items.iterator() ) {
      DynObject *item = static_cast<DynObject*>( world.objects[*i] );

      assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      if( !render.models.contains( item->index ) ) {
        render.models.add( item->index, context.createModel( item ) );
      }
      // draw model
      render.models.cachedValue()->draw();
      render.models.cachedValue()->isUpdated = true;
    }

    glPopMatrix();
    glDisable( GL_TEXTURE_2D );

    printCentered( 256, 10, "items.length(): %d", camera.bot->items.length() );
  }

}
}
}
