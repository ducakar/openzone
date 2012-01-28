/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/ui/InventoryMenu.cc
 */

#include "stable.hh"

#include "client/ui/InventoryMenu.hh"

#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Shape.hh"
#include "client/Colours.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

const float InventoryMenu::SLOT_OBJ_DIM = float( SLOT_SIZE - 2*PADDING_SIZE ) / 2.0f;

void InventoryMenu::onVisibilityChange()
{
  cachedContainerIndex  = -1;
  cachedTaggedItemIndex = -1;

  tagged = -1;
  scroll = 0;
}

bool InventoryMenu::onMouseEvent()
{
  isMouseOver = true;
  tagged = -1;

  Bot* bot = camera.botObj;
  const Object* container = null;

  if( bot != null && bot->parent != -1 ) {
    container = orbis.objects[bot->parent];
  }
  else if( camera.objectObj != null && ( camera.objectObj->flags & Object::BROWSABLE_BIT ) ) {
    container = camera.objectObj;
  }

  if( bot == null || ( master != null && container == null ) ) {
    scroll = 0;
    return false;
  }

  Frame::onMouseEvent();

  int minY = y + FOOTER_SIZE;
  int maxY = y + FOOTER_SIZE + ROWS * SLOT_SIZE;

  // mouse-over selects
  if( minY <= mouse.y && mouse.y < maxY ) {
    int i = ( mouse.x - x ) / SLOT_SIZE + COLS * ( ROWS - 1 - ( mouse.y - minY ) / SLOT_SIZE );

    if( 0 <= i && i < COLS * ROWS ) {
      tagged = scroll * COLS + i;

      const Dynamic* item;

      if( mouse.leftClick ) {
        if( master == null ) {
          if( uint( tagged ) < uint( bot->items.length() ) ) {
            item = static_cast<const Dynamic*>( orbis.objects[ bot->items[tagged] ] );

            if( item != null ) {
              if( container != null ) {
                bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
                bot->actions |= Bot::ACTION_INV_GIVE;
                bot->instrument = item->index;
                bot->container = container->index;
              }
              else if( bot->cargo == -1 ) {
                bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
                bot->actions |= Bot::ACTION_INV_DROP;
                bot->instrument = item->index;
                bot->container = -1;
              }
            }
          }
        }
        else {
          hard_assert( container != null );

          if( uint( tagged ) < uint( container->items.length() ) ) {
            item = static_cast<const Dynamic*>( orbis.objects[ container->items[tagged] ] );

            if( item != null ) {
              bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
              bot->actions |= Bot::ACTION_INV_TAKE;
              bot->instrument = item->index;
              bot->container = container->index;
            }
          }
        }
      }
      else if( mouse.rightClick ) {
        if( master == null ) {
          if( uint( tagged ) < uint( bot->items.length() ) ) {
            item = static_cast<const Dynamic*>( orbis.objects[ bot->items[tagged] ] );

            if( item != null ) {
              bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
              bot->actions |= Bot::ACTION_USE;
              bot->instrument = item->index;
              bot->container = -1;
            }
          }
        }
        else {
          if( uint( tagged ) < uint( container->items.length() ) ) {
            item = static_cast<const Dynamic*>( orbis.objects[ container->items[tagged] ] );

            if( item != null ) {
              bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
              bot->actions |= Bot::ACTION_USE;
              bot->instrument = item->index;
              bot->container = -1;
            }
          }
        }
      }
      else if( mouse.middleClick ) {
        if( uint( tagged ) < uint( bot->items.length() ) ) {
          item = static_cast<const Dynamic*>( orbis.objects[ bot->items[tagged] ] );

          if( item != null ) {
            ui::mouse.doShow = false;

            bot->actions &= ~( Bot::INSTRUMENT_ACTIONS );
            bot->actions |= Bot::ACTION_INV_GRAB;
            bot->instrument = item->index;
            bot->container = -1;
            bot->cargo = -1;
          }
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

void InventoryMenu::onUpdate()
{
  if( !isMouseOver ) {
    tagged = -1;
  }
  isMouseOver = false;
}

void InventoryMenu::onDraw()
{
  const Object* container = null;

  if( master == null ) {
    container = camera.botObj;
  }
  else if( camera.botObj != null && camera.botObj->parent != -1 ) {
    container = orbis.objects[camera.botObj->parent];
  }
  else if( camera.objectObj != null && ( camera.objectObj->flags & Object::BROWSABLE_BIT ) ) {
    container = camera.objectObj;
  }

  if( container == null ) {
    scroll = 0;
    return;
  }

  const ObjectClass* containerClazz = container->clazz;

  if( container->index != cachedContainerIndex ) {
    cachedContainerIndex = container->index;

    if( container->flags & Object::BOT_BIT ) {
      const Bot* bot = static_cast<const Bot*>( container );

      title.setText( "%s (%s)", bot->name.cstr(), containerClazz->title.cstr() );
    }
    else {
      title.setText( "%s", containerClazz->title.cstr() );
    }
  }

  Frame::onDraw();

  for( int i = 0; i < ROWS; ++i ) {
    for( int j = 0; j < COLS; ++j ) {
      if( ( scroll + i ) * COLS + j < containerClazz->nItems ) {
        if( i * COLS + j == tagged ) {
          glUniform4f( param.oz_Colour, 0.6f, 0.6f, 0.6f, 0.6f );
        }
        else {
          glUniform4f( param.oz_Colour, 0.3f, 0.3f, 0.3f, 0.6f );
        }

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
  tf.camera.scale( Vec3( 1.0f, 1.0f, 0.01f ) );

  const Vector<int>& items = container->items;

  int minIndex = scroll * COLS;
  int maxIndex = min( minIndex + COLS * ROWS, items.length() );
  const Dynamic* taggedItem = null;

  for( int i = minIndex; i < maxIndex; ++i ) {
    const Dynamic* item = static_cast<const Dynamic*>( orbis.objects[ items[i] ] );

    if( item == null ) {
      continue;
    }
    if( i == tagged ) {
      taggedItem = item;
    }

    hard_assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

    float size = item->dim.fastL();
    float scale = SLOT_OBJ_DIM / size;

    Mat44 originalCamera = tf.camera;

    tf.model = Mat44::ID;
    tf.camera.rotateX( Math::rad( -45.0f ) );
    tf.camera.rotateZ( Math::rad( +70.0f ) );
    tf.camera.scale( Vec3( scale, scale, scale ) );
    tf.applyCamera();

    context.drawImago( item, null, Mesh::SOLID_BIT | Mesh::ALPHA_BIT );

    tf.camera = originalCamera;
    tf.camera.translate( Vec3( float( SLOT_SIZE ), 0.0f, 0.0f ) );
    if( ( i + 1 ) % COLS == 0 ) {
      tf.camera.translate( Vec3( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f ) );
    }
  }

  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  shader.use( shader.plain );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, 0 );

  shape.bindVertexArray();

  tf.camera = Mat44::ID;
  tf.applyCamera();

  if( taggedItem == null ) {
    cachedTaggedItemIndex = -1;
  }
  else {
    const ObjectClass* taggedClazz = taggedItem->clazz;

    float life = taggedItem->life / taggedClazz->life;
    int lifeWidth = int( life * 46.0f );

    hard_assert( 0.0f <= life && life <= 1.0f );

    glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
    fill( -51, -15, lifeWidth, 10 );

    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
    rect( -52, -16, 48, 12 );

    if( taggedItem->flags & Object::USE_FUNC_BIT ) {
      uint texId = useTexId;

      if( taggedItem->flags & Object::WEAPON_BIT ) {
        const WeaponClass* clazz = static_cast<const WeaponClass*>( taggedClazz );

        if( !clazz->allowedUsers.contains( camera.botObj->clazz ) ) {
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

    if( tagged != cachedTaggedItemIndex ) {
      cachedTaggedItemIndex = tagged;

      if( !taggedClazz->description.isEmpty() ) {
        itemDesc.setText( "%s - %s", taggedClazz->title.cstr(), taggedClazz->description.cstr() );
      }
      else {
        itemDesc.setText( "%s", taggedClazz->title.cstr() );
      }
    }

    itemDesc.draw( this, false );
  }
}

InventoryMenu::InventoryMenu( const InventoryMenu* master_ ) :
  Frame( 0, 8, COLS*SLOT_SIZE, ROWS*SLOT_SIZE + FOOTER_SIZE, " " ),
  master( master_ ),
  itemDesc( -ICON_SIZE - 12, FOOTER_SIZE / 2, ALIGN_RIGHT | ALIGN_VCENTRE, Font::SANS, " " ),
  cachedContainerIndex( -1 ),
  cachedTaggedItemIndex( -1 ),
  tagged( -1 ),
  scroll( 0 ),
  isMouseOver( false )
{
  flags |= UPDATE_BIT;

  x = ( Area::uiWidth - width ) / 2;

  if( master != null ) {
    y = 8 + height + 8;
  }

  log.verboseMode = true;

  scrollUpTexId   = context.loadTexture( "ui/icon/scrollUp.ozcTex" );
  scrollDownTexId = context.loadTexture( "ui/icon/scrollDown.ozcTex" );
  useTexId        = context.loadTexture( "ui/icon/use.ozcTex" );
  equipTexId      = context.loadTexture( "ui/icon/equip.ozcTex" );
  unequipTexId    = context.loadTexture( "ui/icon/unequip.ozcTex" );

  log.verboseMode = false;
}

InventoryMenu::~InventoryMenu()
{
  glDeleteTextures( 1, &scrollUpTexId );
  glDeleteTextures( 1, &scrollDownTexId );
  glDeleteTextures( 1, &useTexId );
  glDeleteTextures( 1, &equipTexId );
  glDeleteTextures( 1, &unequipTexId );
}

}
}
}
