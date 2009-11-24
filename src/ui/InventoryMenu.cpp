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

    taggedIndex = -1;
  }

  void InventoryMenu::onMouseEvent()
  {
    Frame::onMouseEvent();

    if( camera.botIndex == -1 ) {
      return;
    }

    taggedIndex = -1;

    int minY = y + 40;
    int maxY = y + 40 + 64;

    if( minY <= mouse.y && mouse.y < maxY ) {
      int i = ( mouse.x - x ) / 64;

      if( 0 <= i && i < 8 ) {
        taggedIndex = i;

        if( mouse.rightClick ) {
          camera.bot->taggedItem = taggedIndex;
          camera.bot->actions |= Bot::ACTION_INV_USE;
        }
        else if( mouse.middleClick ) {
          camera.bot->taggedItem = taggedIndex;
          camera.bot->actions |= Bot::ACTION_INV_GRAB;
        }
      }
    }
  }

  void InventoryMenu::onDraw()
  {
    if( camera.bot == null ) {
      return;
    }

    Frame::onDraw();

    printCentered( 256, -10, "Inventory" );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable( GL_TEXTURE_2D );
    glPushMatrix();
    glTranslatef( x + 32.0f, y + 32.0f + 40.0f, 0.0f );
    glScalef( 100.0f, 100.0f, 100.0f );

    const Vector<int> &items = camera.bot->items;
    taggedRotation = taggedIndex == -1 ? 20.0f : taggedRotation + 2.0f;

    for( int i = 0; i < items.length(); i++ ) {
      DynObject *item = static_cast<DynObject*>( world.objects[items[i]] );

      assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      if( !render.models.contains( item->index ) ) {
        render.models.add( item->index, context.createModel( item ) );
      }

      glPushMatrix();
      glRotatef( -70.0f, 1.0f, 0.0f, 0.0f );
      glRotatef( i == taggedIndex ? taggedRotation : 20.0f, 0.0f, 0.0f, 1.0f );

      // draw model
      render.models.cachedValue()->draw();
      render.models.cachedValue()->isUpdated = true;

      glPopMatrix();
      glTranslatef( 0.64f, 0.0f, 0.0f );
    }

    glPopMatrix();
    glDisable( GL_TEXTURE_2D );

    printCentered( 256, 10, "items.length(): %d", camera.bot->items.length() );
  }

}
}
}
