/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/modules/GalileoModule.cc
 *
 * Copyright (C) 2002-2011  Davorin Učakar.
 * This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hh"

#include "build/modules/GalileoModule.hh"

#include "matrix/Library.hh"

#include "client/OpenGL.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
{

void GalileoModule::build()
{
  Config terraConfig;

  for( int i = 0; i < library.terrae.length(); ++i ) {
    const String& name = library.terrae[i].name;

    terraConfig.load( "terra/" + name + ".rc" );

    String srcTextureFile = String::str( "terra/%s", terraConfig.get( "mapTexture", "" ) );
    String destTextureFile = String::str( "ui/galileo/%s.ozcTex", name.cstr() );

    bool useS3TC = Context::useS3TC;
    Context::useS3TC = false;

    BufferStream os;

    uint id = Context::loadRawTexture( srcTextureFile, true, GL_LINEAR, GL_LINEAR );
    Context::writeTexture( id, &os );
    glDeleteTextures( 1, &id );
    File( destTextureFile ).write( &os );

    os.reset();

    id = Context::loadRawTexture( "ui/galileo/arrow.png", false, GL_LINEAR, GL_LINEAR );
    Context::writeTexture( id, &os );
    glDeleteTextures( 1, &id );
    File( "ui/galileo/arrow.ozcTex" ).write( &os );

    os.reset();

    id = Context::loadRawTexture( "ui/galileo/marker.png", false, GL_LINEAR, GL_LINEAR );
    Context::writeTexture( id, &os );
    glDeleteTextures( 1, &id );
    File( "ui/galileo/marker.ozcTex" ).write( &os );

    Context::useS3TC = useS3TC;

    terraConfig.clear();
  }
}

}
}
