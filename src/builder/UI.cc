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
 * @file builder/UI.cc
 */

#include "stable.hh"

#include "builder/UI.hh"

#include "client/ui/Mouse.hh"

#include "builder/Context.hh"

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

void UI::buildCursors()
{
  if( !PFile( "ui/cur" ).stat() ) {
    return;
  }

  Log::println( "Building mouse cursors {" );
  Log::indent();

  bool useS3TC = context.useS3TC;
  context.useS3TC = false;

  File::mkdir( "ui" );
  File::mkdir( "ui/cur" );

  for( int i = 0; i < ui::Mouse::CURSORS_MAX; ++i ) {
    PFile inFile( String::str( "ui/cur/%s.in", ui::Mouse::NAMES[i] ) );
    File destFile( String::str( "ui/cur/%s.ozCur", ui::Mouse::NAMES[i] ) );

    String realPath = inFile.realDir() + "/" + inFile.path();

    FILE* fs = fopen( realPath, "r" );
    if( fs == null ) {
      throw Exception( "Failed to open cursor description '%s'", realPath.cstr() );
    }

    int size, hotspotX, hotspotY;
    char imgPath[32];

    int nMatches = fscanf( fs, "%3d %3d %3d %31s", &size, &hotspotX, &hotspotY, imgPath );
    if( nMatches != 4 ) {
      fclose( fs );
      throw Exception( "Invalid xcursor line" );
    }

    fclose( fs );

    Context::Texture tex = context.loadTexture( String::str( "ui/cur/%s", imgPath ), false,
                                                GL_NEAREST, GL_NEAREST );

    BufferStream os;

    os.writeInt( size );
    os.writeInt( hotspotX );
    os.writeInt( hotspotY );
    tex.write( &os );

    destFile.write( os.begin(), os.length() );
  }

  context.useS3TC = useS3TC;

  Log::unindent();
  Log::println( "}" );
}

void UI::buildIcons()
{
  if( !PFile( "ui/icon" ).stat() ) {
    return;
  }

  Log::println( "Building UI icons {" );
  Log::indent();

  bool useS3TC = context.useS3TC;
  context.useS3TC = false;

  File::mkdir( "ui" );
  File::mkdir( "ui/icon" );

  foreach( name, citer( ICON_NAMES ) ) {
    String srcPath  = String::str( "ui/icon/%s.png", *name );
    String destPath = String::str( "ui/icon/%s.ozIcon", *name );

    Context::Texture tex = context.loadTexture( srcPath, false, GL_NEAREST, GL_NEAREST );
    hard_assert( !tex.isEmpty() );

    BufferStream os;

    Log::println( "Compiling '%s'", destPath.cstr() );
    tex.write( &os );

    if( !File( destPath ).write( os.begin(), os.length() ) ) {
      throw Exception( "Texture writing failed" );
    }
  }

  context.useS3TC = useS3TC;

  Log::unindent();
  Log::println( "}" );
}

void UI::copyScheme()
{
  PFile srcFile( "ui/scheme.json" );
  File outFile( "ui/scheme.json" );

  Buffer buffer = srcFile.read();
  if( buffer.isEmpty() ) {
    return;
  }

  Log::print( "Copying UI colour scheme '%s' ...", srcFile.path().cstr() );

  outFile.write( buffer.begin(), buffer.length() );

  Log::printEnd( " OK" );
}

}
}
