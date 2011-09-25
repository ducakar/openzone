/*
 *  InventoryMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/InventoryMenu.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"
#include "client/Colours.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  const float InventoryMenu::SLOT_DIMF = float( SLOT_SIZE ) / 2.0f;

  bool InventoryMenu::onMouseEvent()
  {
    if( camera.state != Camera::BOT || camera.bot == -1 ||
        ( master != null && camera.botObj->instrument == -1 ) )
    {
      return false;
    }

    Object* container = null;
    if( camera.botObj->instrument != -1 ) {
      container = orbis.objects[camera.botObj->instrument];

      if( container == null ) {
        return false;
      }
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

        if( mouse.leftClick ) {
          if( container != null ) {
            if( master == null ) {
              if( container->items.length() < container->clazz->nItems &&
                  tagged < bot->items.length() )
              {
                Dynamic* obj = static_cast<Dynamic*>( orbis.objects[ bot->items[tagged] ] );

                hard_assert( ( obj->flags & Object::DYNAMIC_BIT ) && obj->parent == bot->index );

                if( obj != null ) {
                  container->items.add( obj->index );
                  bot->items.remove( tagged );
                  obj->parent = container->index;

                  if( bot->weaponItem == obj->index ) {
                    bot->weaponItem = -1;
                  }
                }
              }
            }
            else {
              if( bot->items.length() < bot->clazz->nItems &&
                  tagged < container->items.length() )
              {
                Dynamic* obj = static_cast<Dynamic*>( orbis.objects[ container->items[tagged] ] );

                hard_assert( ( obj->flags & Object::DYNAMIC_BIT ) && obj->parent == container->index );

                if( obj != null ) {
                  bot->items.add( container->items[tagged] );
                  container->items.remove( tagged );
                  obj->parent = bot->index;
                }
              }
            }
          }
        }
        else if( master == null ) {
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
    if( camera.state != Camera::BOT || camera.bot == -1 ||
        ( master != null && camera.botObj->instrument == -1 ) )
    {
      tagged = -1;
      row = 0;
      return;
    }

    const Object*      obj;
    const ObjectClass* objClazz;

    if( master == null ) {
      obj = camera.botObj;
      objClazz = obj->clazz;
    }
    else {
      obj = orbis.objects[camera.botObj->instrument];

      if( obj == null ) {
        return;
      }

      objClazz = obj->clazz;
    }

    String sTitle = ( obj->flags & Object::BOT_BIT ) ?
        static_cast<const Bot*>( obj )->name + " (" + objClazz->title + ")" :
        objClazz->title;
    title.set( width / 2, -textHeight - 8, ALIGN_HCENTRE, Font::TITLE, "%s", sTitle.cstr() );
    Frame::onDraw();

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    tf.camera = Mat44::ID;
    tf.camera.translate( Vec3( float( x + SLOT_SIZE / 2 ), float( y + SLOT_SIZE / 2 + FOOTER_SIZE ), 0.0f ) );
    tf.camera.translate( Vec3( float( COLS * SLOT_SIZE ), float( ROWS * SLOT_SIZE ), 0.0f ) );
    tf.camera.scale( Vec3( 1.0f, 1.0f, 0.001f ) );

    const Vector<int>& items = obj->items;

    int minIndex = row * COLS;
    int maxIndex = min( minIndex + COLS * ROWS, items.length() );
    Dynamic* taggedItem = null;

    for( int i = minIndex; i < maxIndex; ++i ) {
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[ items[i] ] );

      hard_assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      float size = item->dim.fastL();
      float scale = SLOT_DIMF / size;

      if( i % COLS == 0 ) {
        tf.camera.translate( Vec3( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f ) );
      }

      Mat44 originalCamera = tf.camera;

      tf.model = Mat44::ID;
      tf.camera.rotateX( Math::rad( -60.0f ) );
      tf.camera.rotateZ( Math::rad( +70.0f ) );
      tf.camera.scale( Vec3( scale, scale, scale ) );
      tf.applyCamera();

      if( i == tagged ) {
        shader.colour = Colours::TAG;
        taggedItem = item;
      }

      context.drawModel( item, null );

      if( i == tagged ) {
        shader.colour = Colours::WHITE;
      }

      tf.camera = originalCamera;
      tf.camera.translate( Vec3( float( SLOT_SIZE ), 0.0f, 0.0f ) );
    }

    glBindTexture( GL_TEXTURE_2D, 0 );

    shape.bindVertexArray();

    shader.use( shader.ui );

    tf.camera = Mat44::ID;
    tf.applyCamera();

    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    tagged = -1;

    if( taggedItem != null ) {
      const ObjectClass* taggedClazz = taggedItem->clazz;

      float life = taggedItem->life / taggedClazz->life;
      int lifeWidth = int( life * 46.0f );

      hard_assert( 0.0f <= life && life <= 1.0f );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
      fill( -51, -15, lifeWidth, 10 );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -52, -16, 48, 12 );

      if( master == null && ( taggedItem->flags & Object::USE_FUNC_BIT ) ) {
        uint texId = useTexId;
        if( taggedItem->flags & Object::WEAPON_BIT ) {
          texId = taggedItem->index == camera.botObj->weaponItem ? unequipTexId : equipTexId;
        }

        glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
        glBindTexture( GL_TEXTURE_2D, texId );
        shape.fill( x + width - ICON_SIZE - 4, y + 4, ICON_SIZE, ICON_SIZE );
        glBindTexture( GL_TEXTURE_2D, 0 );
      }

      if( !taggedClazz->description.isEmpty() ) {
        itemDesc.setText( "%s - %s", taggedClazz->title.cstr(), taggedClazz->description.cstr() );
      }
      else {
        itemDesc.setText( "%s", taggedClazz->title.cstr() );
      }

      itemDesc.draw( this );
    }
  }

  InventoryMenu::InventoryMenu( const InventoryMenu* master_ ) :
      Frame( 0, 8, COLS*SLOT_SIZE, ROWS*SLOT_SIZE + FOOTER_SIZE, "" ),
      master( master_ ),
      itemDesc( -ICON_SIZE - 12, FOOTER_SIZE / 2, ALIGN_RIGHT | ALIGN_VCENTRE, Font::SANS, "" ),
      tagged( -1 ),
      row( 0 )
  {
    x = ( camera.width - width ) / 2;

    if( master != null ) {
      y = 8 + height + 8;
    }

    if( master == null ) {
      useTexId = context.loadTexture( "ui/icon/use.ozcTex" );
      equipTexId = context.loadTexture( "ui/icon/equip.ozcTex" );
      unequipTexId = context.loadTexture( "ui/icon/unequip.ozcTex" );
    }
  }

  InventoryMenu::~InventoryMenu()
  {
    if( master == null ) {
      glDeleteTextures( 1, &useTexId );
      glDeleteTextures( 1, &equipTexId );
      glDeleteTextures( 1, &unequipTexId );
    }
  }

}
}
}
