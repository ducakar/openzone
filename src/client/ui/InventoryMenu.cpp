/*
 *  InventoryMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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

const float InventoryMenu::SLOT_OBJ_DIMF = float( SLOT_SIZE - 2*PADDING_SIZE ) / 2.0f;

void InventoryMenu::onVisibilityChange()
{
  scroll = 0;
}

bool InventoryMenu::onMouseEvent()
{
  if( camera.state != Camera::BOT || camera.botObj == null ||
      ( master != null && camera.botObj->instrument == -1 ) )
  {
    return false;
  }

  Object* container = null;
  if( camera.botObj->instrument != -1 ) {
    container = orbis.objects[camera.botObj->instrument];

    if( container == null || !( container->flags & Object::BROWSABLE_BIT ) ) {
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
      tagged = scroll * COLS + i;

      if( mouse.leftClick ) {
        if( container == null ) {
          bot->taggedItem = tagged;
          bot->actions |= Bot::ACTION_INV_DROP;
        }
        else {
          bot->taggedItem = tagged;

          if( master == null ) {
            bot->actions |= Bot::ACTION_INV_GIVE;
          }
          else {
            bot->actions |= Bot::ACTION_INV_TAKE;
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
    int nItems = master == null ? bot->clazz->nItems : container->clazz->nItems;
    int nScrollRows = max( 0, nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;

    scroll = clamp( scroll + 1,  0, nScrollRows );
  }
  if( mouse.wheelUp ) {
    int nItems = master == null ? bot->clazz->nItems : container->clazz->nItems;
    int nScrollRows = max( 0, nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;

    scroll = clamp( scroll - 1,  0, nScrollRows );
  }
  return true;
}

void InventoryMenu::onDraw()
{
  if( camera.state != Camera::BOT || camera.botObj == null ||
      ( master != null && camera.botObj->instrument == -1 ) )
  {
    tagged = -1;
    scroll = 0;
    return;
  }

  const Object*      container;
  const ObjectClass* containerClazz;

  if( master == null ) {
    container = camera.botObj;
    containerClazz = container->clazz;
  }
  else {
    container = orbis.objects[camera.botObj->instrument];

    if( container == null || !( container->flags & Object::BROWSABLE_BIT ) ) {
      return;
    }

    containerClazz = container->clazz;
  }

  String sTitle = ( container->flags & Object::BOT_BIT ) ?
      static_cast<const Bot*>( container )->name + " (" + containerClazz->title + ")" :
      containerClazz->title;
  title.set( width / 2, -textHeight - 8, ALIGN_HCENTRE, Font::LARGE, "%s", sTitle.cstr() );
  Frame::onDraw();

  glUniform4f( param.oz_Colour, 0.3f, 0.3f, 0.3f, 0.6f );

  for( int i = 0; i < ROWS; ++i ) {
    for( int j = 0; j < COLS; ++j ) {
      if( ( scroll + i ) * COLS + j < containerClazz->nItems ) {
        fill( j * SLOT_SIZE + PADDING_SIZE,
              FOOTER_SIZE + ( ROWS - i - 1 ) * SLOT_SIZE + PADDING_SIZE,
              SLOT_SIZE - 2*PADDING_SIZE,
              SLOT_SIZE - 2*PADDING_SIZE );
      }
      else {
        goto slotsRendered;
      }
    }
  }
  slotsRendered:;

  int nScrollRows = max( 0, containerClazz->nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;

  if( scroll != 0 ) {
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollUpTexId );
    fill( 16, FOOTER_SIZE + ROWS * SLOT_SIZE, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
  if( scroll != nScrollRows ) {
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollDownTexId );
    fill( 16, FOOTER_SIZE - 16, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  glEnable( GL_DEPTH_TEST );
  glDisable( GL_BLEND );

  tf.camera = Mat44::ID;
  tf.camera.translate( Vec3( float( x + SLOT_SIZE / 2 ), float( y + SLOT_SIZE / 2 + FOOTER_SIZE ), 0.0f ) );
  tf.camera.translate( Vec3( float( COLS * SLOT_SIZE ), float( ROWS * SLOT_SIZE ), 0.0f ) );
  tf.camera.scale( Vec3( 1.0f, 1.0f, 0.001f ) );

  const Vector<int>& items = container->items;

  int minIndex = scroll * COLS;
  int maxIndex = min( minIndex + COLS * ROWS, items.length() );
  const Dynamic* taggedItem = null;

  for( int i = minIndex; i < maxIndex; ++i ) {
    const Dynamic* item = static_cast<const Dynamic*>( orbis.objects[ items[i] ] );

    hard_assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

    float size = item->dim.fastL();
    float scale = SLOT_OBJ_DIMF / size;

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

    context.drawImago( item, null, Mesh::SOLID_BIT | Mesh::ALPHA_BIT );

    if( i == tagged ) {
      shader.colour = Colours::WHITE;
    }

    tf.camera = originalCamera;
    tf.camera.translate( Vec3( float( SLOT_SIZE ), 0.0f, 0.0f ) );
  }

  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  shader.use( shader.plain );

  glBindTexture( GL_TEXTURE_2D, 0 );
  shape.bindVertexArray();

  tf.camera = Mat44::ID;
  tf.applyCamera();

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
        const WeaponClass* clazz = static_cast<const WeaponClass*>( taggedClazz );

        if( !clazz->allowedUsers.contains( containerClazz ) ) {
          goto noIcon;
        }

        texId = taggedItem->index == camera.botObj->weapon ? unequipTexId : equipTexId;
      }

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
      glBindTexture( GL_TEXTURE_2D, texId );
      shape.fill( x + width - ICON_SIZE - 4, y + 4, ICON_SIZE, ICON_SIZE );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }
    noIcon:;

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
    scroll( 0 )
{
  x = ( camera.width - width ) / 2;

  if( master != null ) {
    y = 8 + height + 8;
  }

  scrollUpTexId = context.loadTexture( "ui/icon/scrollUp.ozcTex" );
  scrollDownTexId = context.loadTexture( "ui/icon/scrollDown.ozcTex" );

  if( master == null ) {
    useTexId = context.loadTexture( "ui/icon/use.ozcTex" );
    equipTexId = context.loadTexture( "ui/icon/equip.ozcTex" );
    unequipTexId = context.loadTexture( "ui/icon/unequip.ozcTex" );
  }
}

InventoryMenu::~InventoryMenu()
{
  glDeleteTextures( 1, &scrollUpTexId );
  glDeleteTextures( 1, &scrollDownTexId );

  if( master == null ) {
    glDeleteTextures( 1, &useTexId );
    glDeleteTextures( 1, &equipTexId );
    glDeleteTextures( 1, &unequipTexId );
  }
}

}
}
}
