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
 * @file client/BSP.cc
 */

#include <stable.hh>
#include <client/BSP.hh>

#include <client/Context.hh>
#include <client/OpenGL.hh>
#include "SMM.hh"

namespace oz
{
namespace client
{

BSP::BSP( const matrix::BSP* bsp_ ) :
  bsp( bsp_ ), file( "bsp/" + bsp->name + ".ozcBSP" ), isPreloaded( false ), isLoaded( false )
{
  foreach( model, bsp->models.citer() ) {
    context.requestSMM( *model );
  }
}

BSP::~BSP()
{
  if( isLoaded ) {
    mesh.unload();
  }

  foreach( model, bsp->models.citer() ) {
    context.releaseSMM( *model );
  }
}

void BSP::preload()
{
  if( !file.map() ) {
    OZ_ERROR( "BSP file '%s' mmap failed", file.path().cstr() );
  }

  isPreloaded = true;
}

void BSP::load()
{
  InputStream istream = file.inputStream();

  waterFogColour = istream.readVec4();
  lavaFogColour = istream.readVec4();

  mesh.load( &istream, GL_STATIC_DRAW );

  hard_assert( !istream.isAvailable() );

  file.setPath( "" );

  isLoaded = true;
}

void BSP::draw( const Struct* str )
{
  tf.model = Mat44::translation( str->p - Point::ORIGIN );
  tf.model.rotateZ( float( str->heading ) * Math::TAU / 4.0f );

  for( int i = 0; i < str->entities.length(); ++i ) {
    const Entity& entity = str->entities[i];

    tf.push();
    tf.model.translate( entity.offset );

    mesh.schedule( i + 1 );

    if( entity.clazz->model != -1 ) {
      SMM* smm = context.smms[entity.clazz->model].object;

      if( smm != nullptr && smm->isLoaded ) {
        tf.model = tf.model * entity.clazz->modelTransf;

        smm->schedule( 0 );
      }
    }

    tf.pop();
  }

  mesh.schedule( 0 );
}

}
}
