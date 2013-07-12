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

/*
 * skeletalMesh.vert
 *
 * Mesh shader with support for skeletal animation.
 */

#include "header.glsl"

attribute vec3  inPosition;
attribute vec2  inTexCoord;
attribute vec3  inNormal;
attribute vec2  inBones;
attribute float inBlend;

varying vec2 exTexCoord;
varying vec3 exNormal;
varying vec3 exLook;

void main()
{
  int bone0 = int( inBones.x );
  int bone1 = int( inBones.y );

  vec4 localPos    = vec4( inPosition, 1.0 );
  vec4 localNormal = vec4( inNormal, 0.0 );

  localPos = 0.5 * ( oz_BoneTransforms[ bone0 ] * localPos +
                     oz_BoneTransforms[ bone1 ] * localPos );
  localNormal = 0.5 * ( oz_BoneTransforms[ bone0 ] * localNormal +
                        oz_BoneTransforms[ bone1 ] * localNormal );

  gl_Position = oz_ProjModelTransform * localPos;
  exTexCoord  = inTexCoord;
  exNormal    = ( oz_ModelTransform * localNormal ).xyz;
  exLook      = ( oz_ModelTransform * localPos ).xyz - oz_CameraPosition;
}
