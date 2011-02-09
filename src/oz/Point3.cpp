/*
 *  Point3.hpp
 *
 *  3D point
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Point3.hpp"

namespace oz
{

#ifdef OZ_SIMD

  const Point3 Point3::ORIGIN = Point3( 0.0f, 0.0f, 0.0f );

#else

  const Point3 Point3::ORIGIN = Point3( 0.0f, 0.0f, 0.0f );

#endif

}
