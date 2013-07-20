/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/Inventory.cc
 */

#include <client/ui/Inventory.hh>

#include <common/Timer.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/Input.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

void Inventory::ownerItemCallback( ModelField* sender )
{
  Inventory*     inventory = static_cast<Inventory*>( sender->parent );
  const Object*  container = inventory->owner;
  const Dynamic* item      = nullptr;
  Bot*           bot       = camera.botObj;
  int            id        = inventory->scrollOwner * COLS + sender->id;

  hard_assert( bot != nullptr );

  if( uint( id ) < uint( container->items.length() ) ) {
    item = static_cast<const Dynamic*>( orbis.obj( container->items[id] ) );
  }
  if( item == nullptr ) {
    return;
  }

  hard_assert( inventory->taggedItemIndex == -1 );

  if( input.leftClick ) {
    if( inventory->other != nullptr ) {
      bot->invGive( item, inventory->other );
      return;
    }
    else if( bot->cargo < 0 ) {
      bot->invDrop( item );
      return;
    }
  }
  else if( input.rightClick ) {
    bot->invUse( item, container );
  }
  else if( input.middleClick ) {
    if( bot->cargo < 0 && inventory->other == nullptr ) {
      ui::mouse.doShow = false;

      bot->invGrab( item );
      return;
    }
  }

  inventory->taggedItemIndex = item->index;
}

void Inventory::otherItemCallback( ModelField* sender )
{
  Inventory*     inventory = static_cast<Inventory*>( sender->parent );
  const Object*  container = inventory->other;
  const Dynamic* item      = nullptr;
  Bot*           bot       = camera.botObj;
  int            id        = inventory->scrollOther * COLS + sender->id;

  hard_assert( bot != nullptr );

  if( uint( id ) < uint( container->items.length() ) ) {
    item = static_cast<const Dynamic*>( orbis.obj( container->items[id] ) );
  }
  if( item == nullptr ) {
    return;
  }

  hard_assert( inventory->taggedItemIndex == -1 );

  if( input.leftClick ) {
    bot->invTake( item, container );
  }
  else if( input.rightClick ) {
    bot->invUse( item, container );
  }
  else {
    inventory->taggedItemIndex = item->index;
  }
}

void Inventory::updateReferences()
{
  owner = camera.botObj;
  other = nullptr;

  if( camera.botObj != nullptr && camera.botObj->parent >= 0 ) {
    other = orbis.obj( camera.botObj->parent );
  }
  else if( camera.objectObj != nullptr && ( camera.objectObj->flags & Object::BROWSABLE_BIT ) ) {
    other = camera.objectObj;
  }
}

void Inventory::handleScroll( const Object* container, int* scroll )
{
  // scroll
  if( ( container == owner && mouse.y < y + SINGLE_HEIGHT ) ||
      ( container == other && mouse.y >= y + SINGLE_HEIGHT ) )
  {
    if( input.wheelDown ) {
      int nScrollRows = max( 0, container->clazz->nItems - 1 ) / COLS;
      *scroll = clamp( *scroll + 1,  0, nScrollRows );
    }
    if( input.wheelUp ) {
      int nScrollRows = max( 0, container->clazz->nItems - 1 ) / COLS;
      *scroll = clamp( *scroll - 1,  0, nScrollRows );
    }
  }
}

void Inventory::drawComponent( int height, const Object* container, const Dynamic* taggedItem,
                               int scroll )
{
  const ObjectClass* containerClazz = container->clazz;

  int nScrollRows = max( 0, containerClazz->nItems - 1 ) / COLS;

  if( scroll != 0 ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollUpTex.id() );
    shape.fill( x + 16, y + height + SLOT_SIZE, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }
  if( scroll != nScrollRows ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollDownTex.id() );
    shape.fill( x + 16, y + height - 16, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  if( taggedItem == nullptr || taggedItem->parent != container->index ) {
    return;
  }

  const ObjectClass* taggedClazz = taggedItem->clazz;

  float life   = taggedItem->life / taggedClazz->life;
  float status = taggedItem->status();

  lifeBar.draw( this, x + width - 52, y + height + SLOT_SIZE + 8, 50, 12, life );
  if( status >= 0.0f ) {
    statusBar.draw( this, x + width - 52, y + height + SLOT_SIZE + 1, 50, 8, status );
  }

  if( taggedItem->flags & Object::USE_FUNC_BIT ) {
    uint texId = useTex.id();

    if( taggedItem->flags & Object::WEAPON_BIT ) {
      if( !camera.botObj->canEquip( static_cast<const Weapon*>( taggedItem ) ) ) {
        goto noIcon;
      }

      texId = taggedItem->index == camera.botObj->weapon ? unequipTex.id() : equipTex.id();
    }

    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, texId );
    shape.fill( x + width - ICON_SIZE - 4, y + height - FOOTER_SIZE + 4, ICON_SIZE, ICON_SIZE );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }
  noIcon:

  if( taggedItemIndex != cachedTaggedItemIndex ) {
    cachedTaggedItemIndex = taggedItem->index;

    itemDesc.setText( "%s", taggedClazz->title.cstr() );
  }

  itemDesc.setPosition( -ICON_SIZE - 8, height - FOOTER_SIZE / 2 );
  itemDesc.draw( this );
}

void Inventory::onVisibilityChange( bool )
{
  cachedContainerIndex  = -1;
  cachedTaggedItemIndex = -1;

  scrollOwner = 0;
  scrollOther = 0;
}

void Inventory::onUpdate()
{
  updateReferences();

  taggedItemIndex = -1;

  height = HEADER_SIZE + ( other == nullptr ? SINGLE_HEIGHT : 2 * SINGLE_HEIGHT );

  if( camera.state != Camera::UNIT || !mouse.doShow || owner == nullptr ||
      ( owner->state & Bot::DEAD_BIT ) )
  {
    if( !( flags & HIDDEN_BIT ) ) {
      show( false );
    }
  }
  else if( flags & HIDDEN_BIT ) {
    show( true );
  }
  else if( other == nullptr ) {
    scrollOther = 0;
  }
}

bool Inventory::onMouseEvent()
{
  Frame::onMouseEvent();

  if( input.keys[Input::KEY_UI_ALT] ) {
    return true;
  }

  handleScroll( owner, &scrollOwner );

  if( other != nullptr ) {
    handleScroll( other, &scrollOther );
  }
  return true;
}

void Inventory::onDraw()
{
  updateReferences();

  if( owner == nullptr ) {
    return;
  }

  const Object*      container      = other == nullptr ? owner : other;
  const ObjectClass* containerClazz = container->clazz;
  const Dynamic*     taggedItem     = static_cast<const Dynamic*>( orbis.obj( taggedItemIndex ) );

  taggedItemIndex = taggedItem == nullptr ? -1 : taggedItemIndex;

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

  for( int i = 0; i < COLS; ++i ) {
    int id = scrollOwner * COLS + i;

    if( id < owner->items.length() ) {
      const Object* item = orbis.obj( owner->items[id] );

      ownerModels[i]->show( true );
      ownerModels[i]->setModel( item == nullptr ? -1 : item->clazz->imagoModel );
    }
    else {
      ownerModels[i]->show( id < owner->items.capacity() );
      ownerModels[i]->setModel( -1 );
    }
  }

  for( int i = 0; i < COLS; ++i ) {
    int id = scrollOther * COLS + i;

    if( other != nullptr && id < other->items.length() ) {
      const Object* item = orbis.obj( other->items[id] );

      otherModels[i]->show( true );
      otherModels[i]->setModel( item == nullptr ? -1 : item->clazz->imagoModel );
    }
    else {
      otherModels[i]->show( other != nullptr && id < other->items.capacity() );
      otherModels[i]->setModel( -1 );
    }
  }

  Frame::onDraw();

  drawComponent( FOOTER_SIZE, owner, taggedItem, scrollOwner );

  if( other != nullptr ) {
    drawComponent( FOOTER_SIZE + SINGLE_HEIGHT, other, taggedItem, scrollOther );
  }
}

Inventory::Inventory() :
  Frame( COLS*SLOT_SIZE, SLOT_SIZE + FOOTER_SIZE, " " ),
  owner( nullptr ), other( nullptr ),
  lifeBar( &style.taggedLife ), statusBar( &style.taggedStatus ),
  itemDesc( -ICON_SIZE - 12, FOOTER_SIZE / 2, ALIGN_RIGHT | ALIGN_VCENTRE, Font::SANS, " " ),
  taggedItemIndex( -1 ), cachedContainerIndex( -1 ), cachedTaggedItemIndex( -1 ),
  scrollOwner( 0 ), scrollOther( 0 )
{
  flags = UPDATE_BIT | HIDDEN_BIT | IGNORE_BIT;

  for( int i = 0; i < COLS; ++i ) {
    ownerModels[i] = new ModelField( ownerItemCallback,
                                     SLOT_SIZE - 2*PADDING_SIZE, SLOT_SIZE - 2*PADDING_SIZE );
    ownerModels[i]->id = i;

    add( ownerModels[i], PADDING_SIZE + i * SLOT_SIZE, FOOTER_SIZE + PADDING_SIZE );
  }
  for( int i = 0; i < COLS; ++i ) {
    otherModels[i] = new ModelField( otherItemCallback,
                                     SLOT_SIZE - 2*PADDING_SIZE, SLOT_SIZE - 2*PADDING_SIZE );
    otherModels[i]->id = i;

    add( otherModels[i], PADDING_SIZE + i * SLOT_SIZE, FOOTER_SIZE + SINGLE_HEIGHT + PADDING_SIZE );
  }

  scrollUpTex.load( "@ui/icon/scrollUp.dds" );
  scrollDownTex.load( "@ui/icon/scrollDown.dds" );
  useTex.load( "@ui/icon/use.dds" );
  equipTex.load( "@ui/icon/equip.dds" );
  unequipTex.load( "@ui/icon/unequip.dds" );
}

}
}
}
