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
 * @file client/Profile.cc
 */

#include "stable.hh"

#include "client/Profile.hh"

#include "matrix/Library.hh"
#include "matrix/BotClass.hh"
#include "matrix/WeaponClass.hh"

#include <cwctype>

namespace oz
{
namespace client
{

Profile profile;

void Profile::init()
{
  File profileFile( config["dir.config"].asString() + "/profile.json" );

  JSON profileConfig;
  bool configExists = profileConfig.load( &profileFile );

  if( profileConfig.isNull() ) {
    profileConfig.setObject();
  }

  name = profileConfig["name"].get( "" );

  if( name.isEmpty() ) {
    const char* userName = SDL_getenv( "USER" );

    if( userName == null || String::isEmpty( userName ) ) {
      name = OZ_GETTEXT( "Player" );
    }
    else {
      wchar_t wcUserName[64];
      mbstowcs( wcUserName, userName, 64 );

      wcUserName[0] = wchar_t( towupper( wint_t( wcUserName[0] ) ) );

      char mbUserName[64];
      wcstombs( mbUserName, wcUserName, 64 );

      name = mbUserName;
      configExists = false;
    }
  }

  // HACK default profile
  if( !configExists ) {
    profileConfig.add( "class", "beast" );
    profileConfig.add( "weaponItem", 0 );

    JSON& itemsConfig = profileConfig.addArray( "items" );

    itemsConfig.add( "beast_weapon.plasmagun" );
    itemsConfig.add( "nvGoggles" );
    itemsConfig.add( "binoculars" );
    itemsConfig.add( "galileo" );
    itemsConfig.add( "musicPlayer" );
    itemsConfig.add( "cvicek" );
  }

  const char*        sClazz   = profileConfig["class"].asString();
  const ObjectClass* objClazz = library.objClass( sClazz );

  clazz = static_cast<const BotClass*>( objClazz );

  const JSON& itemsConfig = profileConfig["items"];
  int nItems = itemsConfig.length();

  if( nItems > clazz->nItems ) {
    throw Exception( "Too many items for player class '%s' in profile", clazz->name.cstr() );
  }

  items.clear();
  items.dealloc();

  for( int i = 0; i < nItems; ++i ) {
    const char* sItem = itemsConfig[i].asString();

    const ObjectClass* itemClazz = library.objClass( sItem );
    if( ( itemClazz->flags & ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) ) !=
        ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) )
    {
      throw Exception( "Invalid item '%s' in profile", sItem );
    }

    items.add( static_cast<const DynamicClass*>( itemClazz ) );
  }

  weaponItem = profileConfig["weaponItem"].get( -1 );

  if( weaponItem >= 0 ) {
    if( uint( weaponItem ) >= uint( items.length() ) ) {
      throw Exception( "Invalid weaponItem #%d in profile", weaponItem );
    }

    const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( items[weaponItem] );

    if( !clazz->name.beginsWith( weaponClazz->userBase ) ) {
      throw Exception( "Invalid weapon class '%s' for player class '%s' in profile",
                       weaponClazz->name.cstr(), clazz->name.cstr() );
    }
  }

  if( !configExists ) {
    profileConfig.save( &profileFile );
  }
}

void Profile::free()
{
  items.clear();
  items.dealloc();
}

}
}
