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
 * @file client/PartGen.cc
 */

#include <client/PartGen.hh>

#include <common/Timer.hh>
#include <matrix/Physics.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{

struct PartGen::Part
{
  Vec3  p;
  Vec3  velocity;
  float life;
};

void PartGen::draw() const
{
  tf.model = camera.rotMat;

  for( const Part& part : parts ) {
    tf.push();
    tf.model.translate( part.p );
    tf.model = tf.model ^ camera.rotTMat;

    shape.quad( 1.0f, 1.0f );

    tf.pop();
  }
}

void PartGen::update()
{
  for( Part& part : parts ) {
    if( part.life <= 0.0f ) {
      Vec3 localVel = clazz->velocity + Vec3( clazz->velocitySpread * Math::normalRand(),
                                              clazz->velocitySpread * Math::normalRand(),
                                              clazz->velocitySpread * Math::normalRand() );

      part.p        = transf.w.vec3();
      part.velocity = transf * localVel;
    }

    part.p          += part.velocity * timer.frameTime;
    part.velocity.z += physics.gravity;
    part.velocity   *= clazz->drag;
    part.life       -= timer.frameTime;
  }
}

}
}
