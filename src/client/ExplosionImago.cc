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
 * @file client/ExplosionImago.cc
 */

#include "stable.hh"

#include "client/ExplosionImago.hh"

#include "client/Context.hh"
#include "client/Colours.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

int ExplosionImago::modelId;

Pool<ExplosionImago, 64> ExplosionImago::pool;

Imago* ExplosionImago::create( const Object* obj )
{
  ExplosionImago* imago = new ExplosionImago( obj );

  modelId = library.modelIndex( "explosion" );

  imago->smm = context.requestSMM( modelId );
  imago->startMillis = timer.millis;

  return imago;
}

ExplosionImago::~ExplosionImago()
{
  context.releaseSMM( modelId );
}

void ExplosionImago::draw( const Imago*, int mask )
{
  flags |= UPDATED_BIT;

  if( !smm->isLoaded || !( mask & Mesh::ALPHA_BIT ) ) {
    return;
  }

  float millis = float( timer.millis - startMillis );
  float radius = millis * obj->dim.z * 0.004f;
  float alpha  = 1.0f - 0.002f * millis;

  if( shader.mode == Shader::SCENE ) {
    tf.model = Mat44::translation( obj->p - Point3::ORIGIN );
    tf.model.scale( Vec3( radius, radius, radius ) );
  }

  glDisable( GL_CULL_FACE );

  shader.colour = Vec4( 1.0f, 1.0f, 1.0f, alpha*alpha );

  smm->draw( Mesh::SOLID_BIT );

  shader.colour = Colours::WHITE;

  glEnable( GL_CULL_FACE );
}

}
}
