/*
 *  Vec3.hpp
 *
 *  3D vector
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Vec3.hpp"

namespace oz
{

#ifdef OZ_SIMD

  const Vec3 Vec3::ZERO = Vec3( 0.0f, 0.0f, 0.0f );

#else

  const Vec3 Vec3::ZERO = Vec3( 0.0f, 0.0f, 0.0f );

#endif

}
