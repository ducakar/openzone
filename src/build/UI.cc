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
 * @file build/Mouse.cc
 */

#include "stable.hh"

#include "build/UI.hh"

#include "client/ui/Mouse.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
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

    uint texId = context.loadLayer( String::str( "ui/cur/%s", imgPath ), false,
                                    GL_LINEAR, GL_LINEAR );

    BufferStream os;

    os.writeInt( size );
    os.writeInt( hotspotX );
    os.writeInt( hotspotY );
    context.writeLayer( texId, &os );

    glDeleteTextures( 1, &texId );

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

    uint id = context.loadLayer( srcPath, false, GL_LINEAR, GL_LINEAR );
    hard_assert( id != 0 );

    BufferStream os;

    Log::println( "Compiling '%s'", destPath.cstr() );
    context.writeLayer( id, &os );

    glDeleteTextures( 1, &id );

    if( !File( destPath ).write( os.begin(), os.length() ) ) {
      throw Exception( "Texture writing failed" );
    }
  }

  context.useS3TC = useS3TC;

  Log::unindent();
  Log::println( "}" );
}

}
}
