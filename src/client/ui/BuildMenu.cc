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
 * @file client/ui/BuildMenu.cc
 */

#include "stable.hh"

#include "client/ui/BuildMenu.hh"

#include "matrix/Library.hh"
#include "matrix/Synapse.hh"

#include "nirvana/Nirvana.hh"

#include "client/Camera.hh"

namespace oz
{
namespace client
{
namespace ui
{

BuildButton::BuildButton( const char* className_, Callback* callback, int width, int height ) :
  Button( lingua.get( library.objClass( className_ )->title ), callback, width, height ),
  className( className_ )
{}

BuildButton::~BuildButton()
{}

void BuildMenu::createObject( Button* button_ )
{
  const BuildButton* button = static_cast<const BuildButton*>( button_ );
  const ObjectClass* clazz = library.objClass( button->className );

  Point p  = camera.p + ( 2.0f + clazz->dim.fastN() ) * camera.at;
  AABB  bb = AABB( p, clazz->dim );

  if( !collider.overlaps( bb ) ) {
    synapse.addObject( button->className, p, NORTH, false );
  }
}

BuildMenu::BuildMenu() :
  Frame( 240, 250, OZ_GETTEXT( "Create" ) )
{
  add( new BuildButton( "smallCrate", createObject, 110, 18 ), 5, -40 );
  add( new BuildButton( "bigCrate", createObject, 110, 18 ), 5, -60 );
  add( new BuildButton( "metalBarrel", createObject, 110, 18 ), 5, -80 );
  add( new BuildButton( "firstAid", createObject, 110, 18 ), 5, -100 );
  add( new BuildButton( "cvicek", createObject, 110, 18 ), 5, -120 );
  add( new BuildButton( "bomb", createObject, 110, 18 ), 5, -140 );
  add( new BuildButton( "droid_weapon.blaster", createObject, 110, 18 ), 5, -160 );
  add( new BuildButton( "droid_weapon.hyperblaster", createObject, 110, 18 ), 5, -180 );
  add( new BuildButton( "droid_weapon.chaingun", createObject, 110, 18 ), 5, -200 );
  add( new BuildButton( "droid_weapon.grenadeLauncher", createObject, 110, 18 ), 5, -220 );

  add( new BuildButton( "goblin", createObject, 110, 18 ), -5, -40 );
  add( new BuildButton( "knight", createObject, 110, 18 ), -5, -60 );
  add( new BuildButton( "bauul", createObject, 110, 18 ), -5, -80 );
  add( new BuildButton( "beast", createObject, 110, 18 ), -5, -100 );
  add( new BuildButton( "droid", createObject, 110, 18 ), -5, -120 );
  add( new BuildButton( "droid.OOM-9", createObject, 110, 18 ), -5, -140 );
  add( new BuildButton( "raptor", createObject, 110, 18 ), -5, -160 );
  add( new BuildButton( "hoverTank", createObject, 110, 18 ), -5, -180 );
}

BuildMenu::~BuildMenu()
{}

}
}
}
