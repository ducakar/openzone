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
 * @file client/SMMImago.cc
 */

#include "stable.hh"

#include "client/SMMImago.hh"

#include "client/Context.hh"

namespace oz
{
namespace client
{

Pool<SMMImago, 4096> SMMImago::pool;

Imago* SMMImago::create( const Object* obj )
{
  SMMImago* imago = new SMMImago( obj );

  imago->smm = context.requestSMM( obj->clazz->imagoModel );

  return imago;
}

SMMImago::~SMMImago()
{
  context.releaseSMM( clazz->imagoModel );
}

void SMMImago::draw( const Imago* )
{
  flags |= UPDATED_BIT;

  if( !smm->isLoaded ) {
    return;
  }

  if( shader.mode == Shader::SCENE ) {
    tf.model = Mat44::translation( obj->p - Point::ORIGIN );
    tf.model.rotateZ( float( obj->flags & Object::HEADING_MASK ) * Math::TAU / 4.0f );

    tf.colour.w.w = 1.0f;
  }

  smm->schedule( -1 );
}

}
}
