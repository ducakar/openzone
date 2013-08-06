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
#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/Shader.hh>
#include <client/Camera.hh>
#include <client/SMM.hh>
#include <client/Context.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{
namespace ui
{

const Mat44 BuildMenu::OVERLAY_GREEN  = Mat44( 0.0f, 1.0f, 0.0f, 0.0f,
                                               0.0f, 1.0f, 0.0f, 0.0f,
                                               0.0f, 1.0f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 0.0f, 0.5f );

const Mat44 BuildMenu::OVERLAY_YELLOW = Mat44( 0.8f, 0.8f, 0.0f, 0.0f,
                                               0.8f, 0.8f, 0.0f, 0.0f,
                                               0.8f, 0.8f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 0.0f, 0.5f );

const Mat44 BuildMenu::OVERLAY_RED    = Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                                               1.0f, 0.0f, 0.0f, 0.0f,
                                               1.0f, 0.0f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 0.0f, 0.5f );

void BuildMenu::overlayCallback( Area* area, const Vec3& ray )
{
  BuildMenu*         buildMenu = static_cast<BuildMenu*>( area );
  const oz::BSP*     bsp       = buildMenu->overlayBSP;
  const ObjectClass* clazz     = buildMenu->overlayClass;
  Heading            heading   = buildMenu->overlayHeading;
  bool               overlaps  = false;

  collider.translate( camera.p, ray );

  Point position = camera.p + collider.hit.ratio * ray;
  position.z += bsp != nullptr ? bsp->groundOffset : clazz->dim.z;
  position.z += 2.0f * EPSILON;

  tf.model = Mat44::translation( position - Point::ORIGIN );
  tf.model.rotateZ( float( heading ) * Math::TAU / 4.0f );

  if( buildMenu->overlayBSP != nullptr ) {
    Bounds bounds   = rotate( *bsp, heading ) + ( position - Point::ORIGIN );
    BSP*   bspModel = context.requestBSP( bsp );

    List<Struct*> strs;
    List<Object*> objs;

    collider.getOverlaps( bounds.toAABB(), &strs, &objs );
    overlaps  = !strs.isEmpty() || !objs.isEmpty();
    tf.colour = overlaps ? OVERLAY_RED : OVERLAY_GREEN;

    if( !overlaps ) {
      // Check if ground is plain enough.
      float corners[][2] = {
        { bounds.mins.x, bounds.mins.y },
        { bounds.maxs.x, bounds.mins.y },
        { bounds.mins.x, bounds.maxs.y },
        { bounds.maxs.x, bounds.maxs.y }
      };

      for( int i = 0; i < 4; ++i ) {
        if( orbis.terra.height( corners[i][0], corners[i][1] ) < bounds.mins.z ) {
          overlaps  = true;
          tf.colour = OVERLAY_YELLOW;
          break;
        }
      }
    }

    bspModel->schedule( nullptr, Mesh::OVERLAY_QUEUE );
  }
  else {
    Vec3 dim   = clazz->dim + Vec3( 2.0f*EPSILON, 2.0f*EPSILON, 2.0f*EPSILON );
    AABB bb    = AABB( position, rotate( dim, heading ) );
    SMM* model = context.requestModel( clazz->imagoModel );

    overlaps  = collider.overlaps( bb );
    tf.colour = overlaps ? OVERLAY_RED : OVERLAY_GREEN;

    model->schedule( -1, Mesh::OVERLAY_QUEUE );
    context.releaseModel( clazz->imagoModel );
  }

  tf.colour = Mat44::ID;

  if( input.leftPressed ) {
    buildMenu->overlayHeading = Heading( ( heading + 1 ) % 4 );
  }
  else if( input.middlePressed && !overlaps ) {
    if( bsp != nullptr ) {
      synapse.add( bsp, position, heading, false );
    }
    else {
      synapse.add( clazz, position, heading, false );
    }

    buildMenu->overlayBSP     = nullptr;
    buildMenu->overlayClass   = nullptr;
    buildMenu->overlayHeading = NORTH;

    if( ui.strategicArea ) {
      ui.strategicArea->clearOverlay();
    }
  }
}

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

void BuildMenu::startPlacement( ModelField* sender, bool isClicked )
{
  if( sender->id < 0 ) {
    return;
  }

  BuildMenu* buildMenu = static_cast<BuildMenu*>( sender->parent );

  buildMenu->isOverModel  = true;
  buildMenu->wasOverModel = true;

  switch( buildMenu->mode ) {
    case BUILDINGS: {
      const oz::BSP* bsp = techTree.allowedBuildings[sender->id];

      buildMenu->title.setText( "%s", bsp->title.cstr() );

      if( isClicked && ui.strategicArea != nullptr ) {
        buildMenu->overlayBSP     = bsp;
        buildMenu->overlayClass   = nullptr;
        buildMenu->overlayHeading = NORTH;

        ui.strategicArea->setOverlay( overlayCallback, buildMenu );
      }
      break;
    }
    case UNITS: {
      const ObjectClass* clazz = techTree.allowedUnits[sender->id];

      buildMenu->title.setText( "%s", clazz->title.cstr() );

      if( isClicked && ui.strategicArea != nullptr ) {
        buildMenu->overlayBSP     = nullptr;
        buildMenu->overlayClass   = clazz;
        buildMenu->overlayHeading = NORTH;

        ui.strategicArea->setOverlay( overlayCallback, buildMenu );
      }
      break;
    }
    case ITEMS: {
      const ObjectClass* clazz = techTree.allowedItems[sender->id];

      buildMenu->title.setText( "%s", clazz->title.cstr() );

      if( isClicked && ui.strategicArea != nullptr ) {
        buildMenu->overlayBSP     = nullptr;
        buildMenu->overlayClass   = clazz;
        buildMenu->overlayHeading = NORTH;

        ui.strategicArea->setOverlay( overlayCallback, buildMenu );
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
          models[i]->setModel( techTree.allowedBuildings[index] );
          models[i]->show( true );
          models[i]->id = index;
        }
        else {
          models[i]->setModel( nullptr );
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
  mode( BUILDINGS ), overlayBSP( nullptr ), overlayClass( nullptr ), overlayHeading( NORTH ),
  nScrollRows( 0 ), scroll( 0 ), isOverModel( false ), wasOverModel( false )
{
  scrollUpTex.load( "@ui/icon/scrollUp.dds" );
  scrollDownTex.load( "@ui/icon/scrollDown.dds" );

  add( new Button( OZ_GETTEXT( "B" ), selectBuildings, SLOT_SIZE, 18 ),   4, -HEADER_SIZE - 2 );
  add( new Button( OZ_GETTEXT( "U" ), selectUnits,     SLOT_SIZE, 18 ),  82, -HEADER_SIZE - 2 );
  add( new Button( OZ_GETTEXT( "I" ), selectItems,     SLOT_SIZE, 18 ), 160, -HEADER_SIZE - 2 );

  for( int i = 0; i < 4; ++i ) {
    models[i*3 + 0] = new ModelField( startPlacement, SLOT_SIZE, SLOT_SIZE );
    models[i*3 + 1] = new ModelField( startPlacement, SLOT_SIZE, SLOT_SIZE );
    models[i*3 + 2] = new ModelField( startPlacement, SLOT_SIZE, SLOT_SIZE );

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
