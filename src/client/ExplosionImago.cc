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
 * @file client/ExplosionImago.cc
 */

#include "stable.hh"

#include "client/ExplosionImago.hh"

#include "client/Context.hh"
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

  modelId = obj->clazz->imagoModel;

  imago->smm = context.requestSMM( modelId );
  imago->startMicros = uint( timer.micros );

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

  float time   = float( uint( timer.micros ) - startMicros ) * 1.0e-6f;
  float radius = 4.0f * time * obj->dim.z;
  float alpha  = 1.0f - 2.0f * time;

  if( shader.mode == Shader::SCENE ) {
    tf.model = Mat44::translation( obj->p - Point::ORIGIN );
    tf.model.scale( Vec3( radius, radius, radius ) );
  }

  glDisable( GL_CULL_FACE );

  shader.colourTransform.w.w = alpha*alpha;

  smm->draw( Mesh::SOLID_BIT );

  shader.colourTransform.w.w = 1.0f;

  glEnable( GL_CULL_FACE );
}

}
}
