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
 * @file build/modules/GalileoModule.cc
 */

#include "stable.hh"

#include "build/modules/GalileoModule.hh"

#include "client/OpenGL.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
{

void GalileoModule::build()
{
  Config terraConfig;

  PhysFile terraDir( "terra" );
  DArray<PhysFile> terrae = terraDir.ls();

  foreach( file, terrae.iter() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    File::mkdir( "terra" );

    String name = file->baseName();

    log.println( "Terrain texture '%s' for minimap {", name.cstr() );
    log.indent();

    if( !terraConfig.load( *file ) ) {
      throw Exception( "Failed to read terra configuration from '%s'", file->path().cstr() );
    }

    PhysFile srcTextureFile( String::str( "terra/%s", terraConfig.get( "mapTexture", "" ) ) );
    File destTextureFile( "terra/" + file->baseName() + ".ozcTex" );

    bool useS3TC = context.useS3TC;
    context.useS3TC = false;

    BufferStream os;

    uint id = context.loadRawTexture( srcTextureFile.path(), true, GL_LINEAR, GL_LINEAR );
    context.writeTexture( id, &os );
    glDeleteTextures( 1, &id );

    log.print( "Writing to '%s' ...", destTextureFile.path().cstr() );

    if( !destTextureFile.write( &os ) ) {
      throw Exception( "Terra texture '%s' write failed", destTextureFile.path().cstr() );
    }

    log.printEnd( " OK" );

    context.useS3TC = useS3TC;

    terraConfig.clear();

    log.unindent();
    log.println( "}" );
  }
}

}
}
