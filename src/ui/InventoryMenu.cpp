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
#include "client/Colors.h"
#include "client/Render.h"

namespace oz
{
namespace client
{
namespace ui
{

  InventoryMenu::InventoryMenu() :
      Frame( SLOT_SIZE * COLS, SLOT_SIZE * ROWS + HEADER_SIZE + FOOTER_SIZE )
  {
    setFont( TITLE );
    setFontColor( 0xff, 0xff, 0xff );

    useTexId = context.loadTexture( "ui/use.png", false, GL_LINEAR, GL_LINEAR );
    taggedIndex = -1;
    row = 0;
  }

  InventoryMenu::~InventoryMenu()
  {
    context.freeTexture( useTexId );
  }

  void InventoryMenu::onMouseEvent()
  {
    Frame::onMouseEvent();

    if( camera.botIndex == -1 ) {
      return;
    }

    taggedIndex = -1;

    int minY = y + FOOTER_SIZE;
    int maxY = y + FOOTER_SIZE + ROWS * SLOT_SIZE;

    // mouse-over selects
    if( minY <= mouse.y && mouse.y < maxY ) {
      int i = ( mouse.x - x ) / SLOT_SIZE + COLS * ( ROWS - 1 - ( mouse.y - minY ) / SLOT_SIZE );

      if( 0 <= i && i < COLS * ROWS ) {
        taggedIndex = row * COLS + i;

        if( mouse.rightClick ) {
          camera.bot->taggedItem = taggedIndex;
          camera.bot->actions |= Bot::ACTION_INV_USE;
        }
        else if( mouse.middleClick ) {
          camera.bot->taggedItem = taggedIndex;
          camera.bot->actions |= Bot::ACTION_INV_GRAB;

          mouse.doShow = false;
        }
      }
    }

    // scroll
    if( mouse.wheelDown ) {
      row++;
    }
    if( mouse.wheelUp ) {
      row--;
    }

    // works fine if there's no items (then nRows == 1)
    int nRows = ( camera.bot->items.length() - 1 ) / COLS + 1;
    row = ( row + nRows ) % nRows;
  }

  void InventoryMenu::onDraw()
  {
    if( !mouse.doShow || camera.bot == null ) {
      taggedIndex = -1;
      row = 0;
      return;
    }

    Frame::onDraw();

    printCentered( SLOT_SIZE * COLS / 2, -HEADER_SIZE / 2, "Inventory" );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable( GL_TEXTURE_2D );
    glPushMatrix();
    glTranslatef( x + SLOT_SIZE / 2, y + SLOT_SIZE / 2 + FOOTER_SIZE, 0.0f );
    glTranslatef( COLS * SLOT_SIZE, ROWS * SLOT_SIZE, 0.0f );

    const Vector<int> &items = camera.bot->items;

    int minIndex = row * COLS;
    int maxIndex = min( minIndex + COLS * ROWS, items.length() );
    DynObject *taggedItem = null;

    for( int i = minIndex; i < maxIndex; i++ ) {
      DynObject *item = static_cast<DynObject*>( world.objects[items[i]] );

      assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      if( !render.models.contains( item->index ) ) {
        render.models.add( item->index, context.createModel( item ) );
      }

      float size = !item->dim;
      float scale = SLOT_DIMF / size;

      if( i % COLS == 0 ) {
        glTranslatef( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f );
      }

      glPushMatrix();
      glRotatef( -70.0f, 1.0f, 0.0f, 0.0f );
      glRotatef(  20.0f, 0.0f, 0.0f, 1.0f );
      glScalef( scale, scale, scale );

      if( i == taggedIndex ) {
        glColor4fv( Colors::TAG );
        taggedItem = item;
      }

      // draw model
      render.models.cachedValue()->draw();
      render.models.cachedValue()->isUpdated = true;

      if( i == taggedIndex ) {
        glColor4fv( Colors::WHITE );
      }

      glPopMatrix();
      glTranslatef( SLOT_SIZE, 0.0f, 0.0f );
    }

    glPopMatrix();
    glDisable( GL_TEXTURE_2D );

    taggedIndex = -1;

    if( taggedItem != null ) {
      float life = taggedItem->life / taggedItem->type->life;
      int lifeWidth = life * 46.0f;

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( -51, -15, lifeWidth, 10 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -52, -16, 48, 12 );

      glColor4fv( Colors::WHITE );
      if( taggedItem->flags & Object::USE_FUNC_BIT ) {
        glEnable( GL_TEXTURE_2D );

        glBindTexture( GL_TEXTURE_2D, useTexId );
        glBegin( GL_QUADS );
          glTexCoord2i( 0, 1 );
          glVertex2i( x + width - ICON_SIZE - 4, y + 4 );
          glTexCoord2i( 1, 1 );
          glVertex2i( x + width - 4, y + 4 );
          glTexCoord2i( 1, 0 );
          glVertex2i( x + width - 4, y + ICON_SIZE + 4 );
          glTexCoord2i( 0, 0 );
          glVertex2i( x + width - ICON_SIZE - 4, y + ICON_SIZE + 4 );
        glEnd();

        glDisable( GL_TEXTURE_2D );
      }

      printBaseline( 4, FOOTER_SIZE / 2, "%s", taggedItem->type->description.cstr() );
    }
  }

}
}
}
