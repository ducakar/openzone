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

#include <client/ui/Mouse.hh>
#include <builder/Context.hh>

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
  if( File( "@ui/icon" ).type() != File::DIRECTORY ) {
    return;
  }

  Log::println( "Building UI icons {" );
  Log::indent();

  bool useS3TC = context.useS3TC;
  context.useS3TC = false;

  File::mkdir( "ui" );
  File::mkdir( "ui/icon" );

  foreach( name, citer( ICON_NAMES ) ) {
    String srcPath  = String::str( "@ui/icon/%s.png", *name );
    String destPath = String::str( "ui/icon/%s.ozIcon", *name );

    Context::Texture tex = context.loadTexture( srcPath, false, GL_NEAREST, GL_NEAREST );
    hard_assert( !tex.isEmpty() );

    OutputStream os( 0 );

    Log::println( "Compiling '%s'", destPath.cstr() );
    tex.write( &os );

    if( !File( destPath ).write( os.begin(), os.tell() ) ) {
      OZ_ERROR( "Texture writing failed" );
    }
  }

  context.useS3TC = useS3TC;

  Log::unindent();
  Log::println( "}" );
}

void UI::copyScheme()
{
  File srcFile( "@ui/style.json" );
  File outFile( "ui/style.json" );

  if( srcFile.type() == File::REGULAR ) {
    Log::print( "Copying UI colour scheme '%s' ...", srcFile.path().cstr() );

    outFile.write( srcFile.read() );

    Log::printEnd( " OK" );
  }
}

}
}
