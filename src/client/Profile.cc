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
 * @file client/Profile.cc
 */

#include <stable.hh>
#include <client/Profile.hh>

#include <common/Lingua.hh>
#include <matrix/Liber.hh>
#include <matrix/BotClass.hh>
#include <matrix/WeaponClass.hh>

#include <cwchar>
#include <cwctype>

namespace oz
{
namespace client
{

void Profile::init()
{
  File profileFile( File::NATIVE, config["dir.config"].asString() + "/profile.json" );

  JSON profileConfig;
  bool configExists = profileConfig.load( profileFile );

  if( profileConfig.isNull() ) {
    profileConfig = JSON( JSON::OBJECT );
  }

  name = profileConfig["name"].get( "" );

  if( name.isEmpty() ) {
    const char* userName = getenv( "USER" );

    if( userName == nullptr || String::isEmpty( userName ) ) {
      name = OZ_GETTEXT( "Player" );
    }
    else {
      mbstate_t mbState;
      mSet( &mbState, 0, sizeof( mbState ) );

      const char* userNamePtr = userName;
      wchar_t wcUserName[64];
      mbsrtowcs( wcUserName, &userNamePtr, 64, &mbState );

      wcUserName[0] = wchar_t( towupper( wint_t( wcUserName[0] ) ) );

      mSet( &mbState, 0, sizeof( mbState ) );

      const wchar_t* wcUserNamePtr = wcUserName;
      char mbUserName[64];
      wcsrtombs( mbUserName, &wcUserNamePtr, 64, &mbState );

      name = mbUserName;
      configExists = false;
    }
  }

  // HACK default profile
  if( !configExists ) {
    profileConfig.add( "class", "beast" );
    profileConfig.add( "weaponItem", 0 );

    JSON& itemsConfig = profileConfig.add( "items", JSON::ARRAY );

    itemsConfig.add( "beast$plasmagun" );
    itemsConfig.add( "nvGoggles" );
    itemsConfig.add( "binoculars" );
    itemsConfig.add( "galileo" );
    itemsConfig.add( "musicPlayer" );
    itemsConfig.add( "cvicek" );
  }

  const char*        sClazz   = profileConfig["class"].asString();
  const ObjectClass* objClazz = liber.objClass( sClazz );

  clazz = static_cast<const BotClass*>( objClazz );

  const JSON& itemsConfig = profileConfig["items"];
  int nItems = itemsConfig.length();

  if( nItems > clazz->nItems ) {
    OZ_ERROR( "Too many items for player class '%s' in profile", clazz->name.cstr() );
  }

  items.clear();
  items.deallocate();

  for( int i = 0; i < nItems; ++i ) {
    const char* sItem = itemsConfig[i].asString();

    const ObjectClass* itemClazz = liber.objClass( sItem );
    if( ( itemClazz->flags & ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) ) !=
        ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) )
    {
      OZ_ERROR( "Invalid item '%s' in profile", sItem );
    }

    items.add( static_cast<const DynamicClass*>( itemClazz ) );
  }

  weaponItem = profileConfig["weaponItem"].get( -1 );

  if( weaponItem >= 0 ) {
    if( uint( weaponItem ) >= uint( items.length() ) ) {
      OZ_ERROR( "Invalid weaponItem #%d in profile", weaponItem );
    }

    const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( items[weaponItem] );

    if( !clazz->name.beginsWith( weaponClazz->userBase ) ) {
      OZ_ERROR( "Invalid weapon class '%s' for player class '%s' in profile",
                weaponClazz->name.cstr(), clazz->name.cstr() );
    }
  }

  if( !configExists ) {
    profileConfig.save( profileFile );
  }

  profileConfig.clear( true );
}

void Profile::destroy()
{
  items.clear();
  items.deallocate();
}

Profile profile;

}
}
