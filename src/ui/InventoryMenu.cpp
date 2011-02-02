/*
 *  InventoryMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/InventoryMenu.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  const float InventoryMenu::SLOT_DIMF = float( InventoryMenu::SLOT_SIZE ) / 2.0f;

  InventoryMenu::InventoryMenu() :
      Frame( 5, 5, SLOT_SIZE * COLS, SLOT_SIZE * ROWS + HEADER_SIZE + FOOTER_SIZE )
  {
    useTexId = context.loadTexture( "ui/use.png", false, GL_LINEAR, GL_LINEAR );
    tagged = -1;
    row = 0;
  }

  InventoryMenu::~InventoryMenu()
  {
    context.freeTexture( useTexId );
  }

  bool InventoryMenu::onMouseEvent()
  {
    if( camera.state != Camera::BOT || camera.bot == -1 || !mouse.doShow ) {
      return false;
    }

    Frame::onMouseEvent();

    Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

    tagged = -1;

    int minY = y + FOOTER_SIZE;
    int maxY = y + FOOTER_SIZE + ROWS * SLOT_SIZE;

    // mouse-over selects
    if( minY <= mouse.y && mouse.y < maxY ) {
      int i = ( mouse.x - x ) / SLOT_SIZE + COLS * ( ROWS - 1 - ( mouse.y - minY ) / SLOT_SIZE );

      if( 0 <= i && i < COLS * ROWS ) {
        tagged = row * COLS + i;

        if( mouse.rightClick ) {
          bot->taggedItem = tagged;
          bot->actions |= Bot::ACTION_INV_USE;
        }
        else if( mouse.middleClick ) {
          bot->taggedItem = tagged;
          bot->actions |= Bot::ACTION_INV_GRAB;

          mouse.doShow = false;
        }
      }
    }

    // scroll
    if( mouse.wheelDown ) {
      ++row;
    }
    if( mouse.wheelUp ) {
      --row;
    }

    // works fine if there's no items (then nRows == 1)
    int nRows = ( bot->items.length() - 1 ) / COLS + 1;
    row = ( row + nRows ) % nRows;

    return true;
  }

  void InventoryMenu::onDraw()
  {
    if( camera.state != Camera::BOT || camera.bot == -1 || !mouse.doShow ) {
      tagged = -1;
      row = 0;
      return;
    }

    Frame::onDraw();

    setFont( TITLE );
    printCentred( SLOT_SIZE * COLS / 2, -HEADER_SIZE / 2, "Inventory" );
    setFont( SANS );

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable( GL_TEXTURE_2D );
    glPushMatrix();
    glTranslatef( float( x + SLOT_SIZE / 2 ), float( y + SLOT_SIZE / 2 + FOOTER_SIZE ), 0 );
    glTranslatef( float( COLS * SLOT_SIZE ), float( ROWS * SLOT_SIZE ), 0.0f );

    const Vector<int>& items = camera.botObj->items;

    int minIndex = row * COLS;
    int maxIndex = min( minIndex + COLS * ROWS, items.length() );
    Dynamic* taggedItem = null;

    for( int i = minIndex; i < maxIndex; ++i ) {
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[items[i]] );

      assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      float size = !item->dim;
      float scale = SLOT_DIMF / size;

      if( i % COLS == 0 ) {
        glTranslatef( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f );
      }

      glPushMatrix();
      glRotatef( -70.0f, 1.0f, 0.0f, 0.0f );
      glRotatef(  20.0f, 0.0f, 0.0f, 1.0f );
      glScalef( scale, scale, scale );

      if( i == tagged ) {
        glColor4fv( Colours::TAG );
        taggedItem = item;
      }

      context.drawModel( item, null );

      if( i == tagged ) {
        glColor4fv( Colours::WHITE );
      }

      glPopMatrix();
      glTranslatef( float( SLOT_SIZE ), 0.0f, 0.0f );
    }

    glPopMatrix();
    glDisable( GL_TEXTURE_2D );

    tagged = -1;

    if( taggedItem != null ) {
      float life = taggedItem->life / taggedItem->clazz->life;
      int lifeWidth = int( life * 46.0f );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( -51, -15, lifeWidth, 10 );

      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -52, -16, 48, 12 );

      glColor4fv( Colours::WHITE );
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

      printBaseline( 4, FOOTER_SIZE / 2, "%s", taggedItem->clazz->description.cstr() );
    }
  }

}
}
}
