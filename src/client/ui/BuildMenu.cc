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
 * @file client/ui/BuildMenu.cc
 */

#include <client/ui/BuildMenu.hh>

#include <matrix/Synapse.hh>
#include <nirvana/TechTree.hh>
#include <client/Shape.hh>
#include <client/Shader.hh>
#include <client/Camera.hh>
#include <client/Input.hh>

namespace oz
{
namespace client
{
namespace ui
{

const int BuildMenu::SLOT_SIZE = 76;

void BuildMenu::selectBuildings( Button* sender )
{
  BuildMenu* buildMenu = static_cast<BuildMenu*>( sender->parent );

  buildMenu->mode = BUILDINGS;
  buildMenu->title.setText( "%s", OZ_GETTEXT( "Buildings" ) );
}

void BuildMenu::selectUnits( Button* sender )
{
  BuildMenu* buildMenu = static_cast<BuildMenu*>( sender->parent );

  buildMenu->mode = UNITS;
  buildMenu->title.setText( "%s", OZ_GETTEXT( "Units" ) );
}

void BuildMenu::selectItems( Button* sender )
{
  BuildMenu* buildMenu = static_cast<BuildMenu*>( sender->parent );

  buildMenu->mode = ITEMS;
  buildMenu->title.setText( "%s", OZ_GETTEXT( "Items" ) );
}

void BuildMenu::createSelection( ModelField* sender )
{
  if( sender->id < 0 ) {
    return;
  }

  BuildMenu* buildMenu = static_cast<BuildMenu*>( sender->parent );

  buildMenu->isOverModel  = true;
  buildMenu->wasOverModel = true;

  switch( buildMenu->mode ) {
    case BUILDINGS: {
      const BSP* bsp = techTree.allowedBuildings[sender->id];

      buildMenu->title.setText( "%s", bsp->title.cstr() );

      if( input.leftClick ) {
        Point p  = camera.p + ( 2.0f + bsp->dim().fastN() ) * camera.at;
        AABB  bb = AABB( p, bsp->dim() );

        if( !collider.overlaps( bb ) ) {
          synapse.add( bsp, p, NORTH, false );
        }
      }
      break;
    }
    case UNITS: {
      const ObjectClass* clazz = techTree.allowedUnits[sender->id];

      buildMenu->title.setText( "%s", clazz->title.cstr() );

      if( input.leftClick ) {
        Point p  = camera.p + ( 2.0f + clazz->dim.fastN() ) * camera.at;
        AABB  bb = AABB( p, clazz->dim );

        if( !collider.overlaps( bb ) ) {
          synapse.add( clazz, p, NORTH, false );
        }
      }
      break;
    }
    case ITEMS: {
      const ObjectClass* clazz = techTree.allowedItems[sender->id];

      buildMenu->title.setText( "%s", clazz->title.cstr() );

      if( input.leftClick ) {
        Point p  = camera.p + ( 2.0f + clazz->dim.fastN() ) * camera.at;
        AABB  bb = AABB( p, clazz->dim );

        if( !collider.overlaps( bb ) ) {
          synapse.add( clazz, p, NORTH, false );
        }
      }
      break;
    }
  }
}

bool BuildMenu::onMouseEvent()
{
  Frame::onMouseEvent();

  if( input.wheelDown ) {
    scroll = clamp( scroll + 1, 0, nScrollRows );
  }
  else if( input.wheelUp ) {
    scroll = clamp( scroll - 1, 0, nScrollRows );
  }
  return true;
}

void BuildMenu::onDraw()
{
  Frame::onDraw();

  if( scroll != 0 ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollUpTex.id() );
    shape.fill( x + 112, y + height - HEADER_SIZE - 40, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }
  if( scroll != nScrollRows ) {
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, scrollDownTex.id() );
    shape.fill( x + 112, y + 4, 16, 16 );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  switch( mode ) {
    case BUILDINGS: {
      for( int i = 0; i < 12; ++i ) {
        int index = scroll * 3 + i;

        if( index < techTree.allowedBuildings.length() ) {
          models[i]->setBSP( techTree.allowedBuildings[index] );
          models[i]->show( true );
          models[i]->id = index;
        }
        else {
          models[i]->setBSP( nullptr );
          models[i]->show( false );
          models[i]->id = -1;
        }
      }

      nScrollRows = max( 0, ( techTree.allowedBuildings.length() + 2 ) / 3 - 4 );
      break;
    }
    case UNITS: {
      for( int i = 0; i < 12; ++i ) {
        int index = scroll * 3 + i;

        if( index < techTree.allowedUnits.length() ) {
          models[i]->setModel( techTree.allowedUnits[index]->imagoModel );
          models[i]->show( true );
          models[i]->id = index;
        }
        else {
          models[i]->setModel( -1 );
          models[i]->show( false );
          models[i]->id = -1;
        }
      }

      nScrollRows = max( 0, ( techTree.allowedUnits.length() + 2 ) / 3 - 4 );
      break;
    }
    case ITEMS: {
      for( int i = 0; i < 12; ++i ) {
        int index = scroll * 3 + i;

        if( index < techTree.allowedItems.length() ) {
          models[i]->setModel( techTree.allowedItems[index]->imagoModel );
          models[i]->show( true );
          models[i]->id = index;
        }
        else {
          models[i]->setModel( -1 );
          models[i]->show( false );
          models[i]->id = -1;
        }
      }

      nScrollRows = max( 0, ( techTree.allowedItems.length() + 2 ) / 3 - 4 );
      break;
    }
  }

  if( !isOverModel && wasOverModel ) {
    wasOverModel = false;

    switch( mode ) {
      case BUILDINGS: {
        title.setText( "%s", OZ_GETTEXT( "Buildings" ) );
        break;
      }
      case UNITS: {
        title.setText( "%s", OZ_GETTEXT( "Units" ) );
        break;
      }
      case ITEMS: {
        title.setText( "%s", OZ_GETTEXT( "Items" ) );
        break;
      }
    }
  }

  scroll      = clamp( scroll, 0, nScrollRows );
  isOverModel = false;
}

BuildMenu::BuildMenu() :
  Frame( 240, 374, OZ_GETTEXT( "Buildings" ) ),
  mode( BUILDINGS ), nScrollRows( 0 ), scroll( 0 ), isOverModel( false ), wasOverModel( false )
{
  scrollUpTex.load( "@ui/icon/scrollUp.dds" );
  scrollDownTex.load( "@ui/icon/scrollDown.dds" );

  add( new Button( OZ_GETTEXT( "B" ), selectBuildings, SLOT_SIZE, 18 ),   4, -HEADER_SIZE - 2 );
  add( new Button( OZ_GETTEXT( "U" ), selectUnits,     SLOT_SIZE, 18 ),  82, -HEADER_SIZE - 2 );
  add( new Button( OZ_GETTEXT( "I" ), selectItems,     SLOT_SIZE, 18 ), 160, -HEADER_SIZE - 2 );

  for( int i = 0; i < 4; ++i ) {
    models[i*3 + 0] = new ModelField( createSelection, SLOT_SIZE, SLOT_SIZE );
    models[i*3 + 1] = new ModelField( createSelection, SLOT_SIZE, SLOT_SIZE );
    models[i*3 + 2] = new ModelField( createSelection, SLOT_SIZE, SLOT_SIZE );

    models[i*3 + 0]->id = i*3 + 0;
    models[i*3 + 1]->id = i*3 + 1;
    models[i*3 + 2]->id = i*3 + 2;

    models[i*3 + 0]->show( false );
    models[i*3 + 1]->show( false );
    models[i*3 + 2]->show( false );

    add( models[i*3 + 0],   4, 22 + ( 3 - i ) * 78 );
    add( models[i*3 + 1],  82, 22 + ( 3 - i ) * 78 );
    add( models[i*3 + 2], 160, 22 + ( 3 - i ) * 78 );
  }
}

BuildMenu::~BuildMenu()
{}

}
}
}
