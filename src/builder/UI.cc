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

#include <stable.hh>
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

  HashMap<String, bool> icons;
  foreach( name, citer( ICON_NAMES ) ) {
    icons.add( *name, false );
  }

  Log::println( "Building UI icons {" );
  Log::indent();

  File::mkdir( "ui" );
  File::mkdir( "ui/icon" );

  DArray<File> images = dir.ls();

  foreach( image, images.citer() ) {
    String name = image->baseName();

    bool* isBuilt = icons.find( name );
    if( image->type() != File::REGULAR || isBuilt == nullptr || *isBuilt ) {
      continue;
    }

    File destFile( "ui/icon/" + name + ".dds" );

    OutputStream os( 0 );
    if( image->hasExtension( "dds" ) ) {
      if( !image->read( &os ) ) {
        OZ_ERROR( "Error reading image '%s'", image->path().cstr() );
      }
      else {
        Log::println( "'%s' copied", image->path().cstr() );
      }
    }
    else {
      if( !Builder::buildDDS( *image, 0, &os ) ) {
        continue;
      }
      else {
        Log::println( "'%s' converted to DDS", image->path().cstr() );
      }
    }

    if( !destFile.write( os.begin(), os.tell() ) ) {
      OZ_ERROR( "Failed to write '%s' file", destFile.path().cstr() );
    }

    *isBuilt = true;
  }

  foreach( i, icons.citer() ) {
    if( !i->value ) {
      OZ_ERROR( "Mission icon: %s", i->key.cstr() );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

}
}
