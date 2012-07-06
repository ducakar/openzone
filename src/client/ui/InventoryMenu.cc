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

#include "client/Shape.hh"
#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Input.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

void InventoryMenu::handleComponent( int height, const Object* container, int* tagged, int* scroll )
{
  Bot* bot = camera.botObj;

  int minY = y + height;
  int maxY = y + height + ROWS * SLOT_SIZE;

  // scroll
  if( ( container == owner && mouse.y < y + SINGLE_HEIGHT ) ||
    ( container == other && mouse.y >= y + SINGLE_HEIGHT ) )
  {
    if( input.wheelDown ) {
      int nScrollRows = max( 0, container->clazz->nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;
      *scroll = clamp( *scroll + 1,  0, nScrollRows );
    }
    if( input.wheelUp ) {
      int nScrollRows = max( 0, container->clazz->nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;
      *scroll = clamp( *scroll - 1,  0, nScrollRows );
    }
  }

  // mouse-over selects
  if( minY <= mouse.y && mouse.y < maxY ) {
    int i = ( mouse.x - x ) / SLOT_SIZE + COLS * ( ROWS - 1 - ( mouse.y - minY ) / SLOT_SIZE );

    if( 0 <= i && i < COLS * ROWS ) {
      *tagged = *scroll * COLS + i;

      const Dynamic* item;

      if( input.leftClick ) {
        if( uint( *tagged ) < uint( container->items.length() ) ) {
          item = static_cast<const Dynamic*>( orbis.objects[ container->items[*tagged] ] );

          if( item != null ) {
            if( container == other ) {
              bot->invTake( item, other );
            }
            else if( other != null ) {
              bot->invGive( item, other );
            }
            else if( bot->cargo < 0 ) {
              bot->invDrop( item );
            }
          }
        }
      }
      else if( input.rightClick ) {
        if( uint( *tagged ) < uint( container->items.length() ) ) {
          item = static_cast<const Dynamic*>( orbis.objects[ container->items[*tagged] ] );

          if( item != null ) {
            bot->invUse( item, container );
          }
        }
      }
      else if( input.middleClick ) {
        if( uint( *tagged ) < uint( bot->items.length() ) ) {
          item = static_cast<const Dynamic*>( orbis.objects[ bot->items[*tagged] ] );

          if( item != null && container == owner && bot->cargo < 0 ) {
            ui::mouse.doShow = false;

            bot->invGrab( item );
          }
        }
      }
    }
  }
}

void InventoryMenu::drawComponent( int height, const Object* container, int tagged, int scroll )
{
  const ObjectClass* containerClazz = container->clazz;

  for( int i = 0; i < ROWS; ++i ) {
    for( int j = 0; j < COLS; ++j ) {
      int index = ( scroll + i ) * COLS + j;

      if( index < containerClazz->nItems ) {
        if( index == tagged ) {
          shape.colour( 0.6f, 0.6f, 0.6f, 0.6f );
        }
        else {
          shape.colour( 0.3f, 0.3f, 0.3f, 0.6f );
        }

        shape.fill( x + j * SLOT_SIZE + PADDING_SIZE,
                    y + height + ( ROWS - i - 1 ) * SLOT_SIZE + PADDING_SIZE,
                    SLOT_SIZE - 2*PADDING_SIZE,
                    SLOT_SIZE - 2*PADDING_SIZE );
      }
      else {
        goto slotsRendered;
      }
    }
  }
slotsRendered:

  int nScrollRows = max( 0, containerClazz->nItems - ( ROWS - 1 ) * COLS - 1 ) / COLS;

  if( scroll != 0 ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollUpTexId );
    shape.fill( x + 16, y + height + ROWS * SLOT_SIZE, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
  if( scroll != nScrollRows ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollDownTexId );
    shape.fill( x + 16, y + height - 16, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  tf.camera = Mat44::translation( Vec3( float( x + SLOT_SIZE / 2 ),
                                        float( y + height + SLOT_SIZE / 2 ),
                                        0.0f ) );
  tf.camera.scale( Vec3( 1.0f, 1.0f, 0.001f ) );
  tf.applyCamera();

  tf.model = Mat44::ID;

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

    float size = item->dim.fastN();
    float scale = SLOT_OBJ_DIM / size;

    tf.push();

    tf.model.scale( Vec3( scale, scale, scale ) );
    tf.model.rotateX( Math::rad( -45.0f ) );
    tf.model.rotateZ( Math::rad( +70.0f ) );

    context.drawImago( item, null, Mesh::SOLID_BIT | Mesh::ALPHA_BIT );

    tf.pop();

    tf.model.translate( Vec3( float( SLOT_SIZE ), 0.0f, 0.0f ) );
    if( ( i + 1 ) % COLS == 0 ) {
      tf.model.translate( Vec3( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f ) );
    }
  }

  glEnable( GL_DEPTH_TEST );
  glDisable( GL_BLEND );

  Mesh::drawScheduled();

  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  shape.bind();
  shader.program( shader.plain );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, 0 );

  tf.camera = Mat44::ID;
  tf.applyCamera();

  if( taggedItem == null ) {
    return;
  }

  const ObjectClass* taggedClazz = taggedItem->clazz;

  float life = taggedItem->life / taggedClazz->life;
  int lifeWidth = int( life * 46.0f );

  hard_assert( 0.0f <= life && life <= 1.0f );

  shape.colour( 1.0f - life, life, 0.0f, 0.6f );
  shape.fill( x + width - 51, y + height + SLOT_SIZE + 5, lifeWidth, 10 );

  shape.colour( 1.0f, 1.0f, 1.0f, 0.6f );
  shape.rect( x + width - 52, y + height + SLOT_SIZE + 4, 48, 12 );

  if( taggedItem->flags & Object::USE_FUNC_BIT ) {
    uint texId = useTexId;

    if( taggedItem->flags & Object::WEAPON_BIT ) {
      const WeaponClass* clazz = static_cast<const WeaponClass*>( taggedClazz );

      if( !camera.botObj->clazz->name.beginsWith( clazz->userBase ) ) {
        goto noIcon;
      }

      texId = taggedItem->index == camera.botObj->weapon ? unequipTexId : equipTexId;
    }

    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, texId );
    shape.fill( x + width - ICON_SIZE - 4, y + height - FOOTER_SIZE + 4, ICON_SIZE, ICON_SIZE );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
noIcon:

  if( tagged != cachedTaggedItemIndex ) {
    cachedTaggedItemIndex = tagged;

    itemDesc.set( -ICON_SIZE - 8, height - FOOTER_SIZE / 2, ALIGN_RIGHT | ALIGN_VCENTRE,
                  Font::SANS, "%s", taggedClazz->title.cstr() );
  }

  itemDesc.draw( this, false );
}

void InventoryMenu::onVisibilityChange( bool )
{
  cachedContainerIndex  = -1;
  cachedTaggedItemIndex = -1;

  taggedOwner = -1;
  taggedOther = -1;
  scrollOwner = 0;
  scrollOther = 0;
}

void InventoryMenu::onUpdate()
{
  if( camera.state != Camera::UNIT || !mouse.doShow ) {
    if( !( flags & HIDDEN_BIT ) ) {
      show( false );
    }
    return;
  }

  owner = camera.botObj;
  other = null;

  if( camera.botObj != null && camera.botObj->parent >= 0 ) {
    other = orbis.objects[camera.botObj->parent];
  }
  else if( camera.objectObj != null && ( camera.objectObj->flags & Object::BROWSABLE_BIT ) ) {
    other = camera.objectObj;
  }

  height = HEADER_SIZE + ( other == null ? SINGLE_HEIGHT : 2 * SINGLE_HEIGHT );

  if( owner == null ) {
    if( !( flags & HIDDEN_BIT ) ) {
      show( false );
    }
  }
  else if( flags & HIDDEN_BIT ) {
    show( true );
  }
  else {
    if( other == null ) {
      taggedOther = -1;
      scrollOther = 0;
    }
    if( !isMouseOver ) {
      taggedOwner = -1;
      taggedOther = -1;
    }
    isMouseOver = false;
  }
}

bool InventoryMenu::onMouseEvent()
{
  Frame::onMouseEvent();

  if( input.keys[SDLK_LALT] || input.keys[SDLK_RALT] ) {
    return true;
  }

  taggedOwner = -1;
  taggedOther = -1;
  isMouseOver = true;

  handleComponent( FOOTER_SIZE, owner, &taggedOwner, &scrollOwner );

  if( other != null ) {
    handleComponent( FOOTER_SIZE + SINGLE_HEIGHT, other, &taggedOther, &scrollOther );
  }

  return true;
}

void InventoryMenu::onDraw()
{
  const Object*      container      = other == null ? owner : other;
  const ObjectClass* containerClazz = container->clazz;

  if( container->index != cachedContainerIndex ) {
    cachedContainerIndex = container->index;

    if( container->flags & Object::BOT_BIT ) {
      const Bot* bot = static_cast<const Bot*>( container );

      title.set( "%s (%s)", bot->name.cstr(), containerClazz->title.cstr() );
    }
    else {
      title.set( "%s", containerClazz->title.cstr() );
    }
  }

  Frame::onDraw();

  drawComponent( FOOTER_SIZE, owner, taggedOwner, scrollOwner );

  if( other != null ) {
    drawComponent( FOOTER_SIZE + SINGLE_HEIGHT, other, taggedOther, scrollOther );
  }

  if( taggedOwner < 0 && taggedOther < 0 ) {
    cachedTaggedItemIndex = -1;
  }
}

InventoryMenu::InventoryMenu() :
  Frame( COLS*SLOT_SIZE, ROWS*SLOT_SIZE + FOOTER_SIZE, " " ),
  owner( null ), other( null ),
  itemDesc( -ICON_SIZE - 12, FOOTER_SIZE / 2, ALIGN_RIGHT | ALIGN_VCENTRE, Font::SANS, " " ),
  cachedContainerIndex( -1 ),
  cachedTaggedItemIndex( -1 ),
  taggedOwner( -1 ), taggedOther( -1 ),
  scrollOwner( 0 ), scrollOther( 0 ),
  isMouseOver( false )
{
  flags = UPDATE_BIT | HIDDEN_BIT | IGNORE_BIT;

  Log::verboseMode = true;

  scrollUpTexId   = context.loadTextureLayer( "ui/icon/scrollUp.ozIcon" );
  scrollDownTexId = context.loadTextureLayer( "ui/icon/scrollDown.ozIcon" );
  useTexId        = context.loadTextureLayer( "ui/icon/use.ozIcon" );
  equipTexId      = context.loadTextureLayer( "ui/icon/equip.ozIcon" );
  unequipTexId    = context.loadTextureLayer( "ui/icon/unequip.ozIcon" );

  Log::verboseMode = false;
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
