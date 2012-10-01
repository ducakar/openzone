/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Vec3.cc
 */

#include "common.hh"

namespace oz
{

// Declare dummy class binary compatible with both SIMD and non-SIMD versions of Point class.
struct OZ_ALIGNED( 16 ) Vec3
{
  float x;
  float y;
  float z;
  float w;

  static const Vec3 ZERO;
  static const Vec3 ONE;
};

const Vec3 Vec3::ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
const Vec3 Vec3::ONE  = { 1.0f, 1.0f, 1.0f, 0.0f };

}
