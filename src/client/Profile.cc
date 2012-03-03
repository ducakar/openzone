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

#ifndef __clang__
// GCC bug, issues a false warning.
# pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace oz
{
namespace client
{

Profile profile;

void Profile::init()
{
  const char* userName = SDL_getenv( "USER" );
  userName = userName == null ? "Player" : userName;

  char playerName[64];
  strncpy( playerName, userName, 64 );
  playerName[63] = '\0';

  if( 'a' <= playerName[0] && playerName[0] <= 'z' ) {
    playerName[0] += char( 'A' - 'a' );
  }

  const ObjectClass* objClazz = library.objClass( config.getSet( "profile.class", "beast" ) );

  name  = config.getSet( "profile.name", playerName );
  clazz = static_cast<const BotClass*>( objClazz );

  char buffer[] = "profile.item  ";
  for( int i = 0; i < ObjectClass::MAX_ITEMS; ++i ) {
    hard_assert( i < 100 );

    buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
    buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

    const char* itemName = config.get( buffer, "" );
    if( !String::isEmpty( itemName ) ) {
      items.add( library.objClass( itemName ) );
    }
  }

  if( items.length() > clazz->nItems ) {
    throw Exception( "Too many items for player profile" );
  }

  weaponItem = config.getSet( "profile.weaponItem", -1 );

  if( weaponItem != -1 ) {
    if( uint( weaponItem ) >= uint( items.length() ) ) {
      throw Exception( "Invalid profile.weaponItem %d", weaponItem );
    }

    const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( items[weaponItem] );

    if( !clazz->name.beginsWith( weaponClazz->userBase ) ) {
      throw Exception( "Invalid weapon class '%s' for player class '%s' in profile",
                       weaponClazz->name.cstr(), clazz->name.cstr() );
    }
  }
}

}
}
