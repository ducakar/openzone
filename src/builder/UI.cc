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
 * @file builder/UI.cc
 */

#include <builder/UI.hh>

namespace oz
{
namespace builder
{

const char* const UI::ICON_NAMES[] = {
  "crosshair",
  "use",
  "device",
  "equip",
  "unequip",
  "mount",
  "take",
  "browse",
  "lift",
  "grab",
  "locked",
  "unlocked",
  "scrollUp",
  "scrollDown",
  "marker",
  "arrow"
};

void UI::buildIcons()
{
  File dir( "@ui/icon" );

  if( dir.type() != File::DIRECTORY ) {
    return;
  }

  Set<String> builtIcons;

  Log::println( "Building UI icons {" );
  Log::indent();

  File::mkdir( "ui" );
  File::mkdir( "ui/icon" );

  DArray<File> images = dir.ls();

  foreach( image, images.iter() ) {
    String name = image->baseName();

    if( image->type() != File::REGULAR || builtIcons.contains( name ) ) {
      continue;
    }

    image->map();

    if( !Builder::isImage( *image ) ) {
      image->unmap();
      continue;
    }

    Log::print( "%s ...", image->name().cstr() );

    if( !aContains( ICON_NAMES, name, aLength( ICON_NAMES ) ) ) {
      OZ_ERROR( "Unnecessary icon: %s", image->path().cstr() );
    }

    File destFile( "ui/icon/" + name + ".dds" );

    OutputStream os( 0 );
    if( image->hasExtension( "dds" ) ) {
      if( !image->read( &os ) ) {
        OZ_ERROR( "Error reading image '%s'", image->path().cstr() );
      }
      else {
        Log::printEnd( " OK" );
      }
    }
    else {
      if( !Builder::buildDDS( *image, 0, &os ) ) {
        OZ_ERROR( "Error converting '%s' to DDS", image->name().cstr() );
      }
      else {
        Log::printEnd( " %s ... OK", destFile.name().cstr() );
      }
    }

    if( !destFile.write( os.begin(), os.tell() ) ) {
      OZ_ERROR( "Failed to write '%s' file", destFile.path().cstr() );
    }

    builtIcons.add( name );
  }

  for( int i = 0; i < aLength( ICON_NAMES ); ++i ) {
    if( !builtIcons.contains( ICON_NAMES[i]) ) {
      OZ_ERROR( "Mission icon: %s", ICON_NAMES[i] );
    }
  }

  hard_assert( builtIcons.length() == aLength( ICON_NAMES ) );

  Log::unindent();
  Log::println( "}" );
}

}
}
