/*
 *  Vec3.hpp
 *
 *  3D vector inside 4D space, optimised for SIMD
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Vec4.hpp"

namespace oz
{

  const Vec4 Vec4::ZERO = Vec4( (float4) { 0.0f, 0.0f, 0.0f, 0.0f } );

}
