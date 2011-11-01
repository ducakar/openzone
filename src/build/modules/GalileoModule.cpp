/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file build/modules/GalileoModule.cpp
 *
 * Copyright (C) 2002-2011  Davorin Učakar.
 * This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "build/modules/GalileoModule.hpp"

#include "matrix/Library.hpp"

#include "client/OpenGL.hpp"

#include "build/Context.hpp"

namespace oz
{
namespace build
{

void GalileoModule::build()
{
  Config terraConfig;
  Buffer buffer( 2 * 1024 * 1024 );

  for( int i = 0; i < library.terras.length(); ++i ) {
    const String& name = library.terras[i].name;

    terraConfig.load( "terra/" + name + ".rc" );

    // disable warnings
    terraConfig.get( "step", 0.0f );
    terraConfig.get( "bias", 0.0f );
    terraConfig.get( "waterTexture", "" );
    terraConfig.get( "detailTexture", "" );

    String srcTextureFile = String( "terra/" ) + terraConfig.get( "mapTexture", "" );
    String destTextureFile = "ui/galileo/" + name + ".ozcTex";

    OutputStream ostream = buffer.outputStream();

    uint id = Context::loadRawTexture( srcTextureFile, true, GL_LINEAR, GL_LINEAR );
    Context::writeTexture( id, &ostream );

    terraConfig.clear();

    buffer.write( destTextureFile, ostream.length() );
  }
}

}
}
