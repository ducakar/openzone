/*
 *  Point4.hpp
 *
 *  3D point inside 4D space optimised for SIMD
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Point4.hpp"

namespace oz
{

  const Point4 Point4::ORIGIN = Point4( (float4) { 0.0f, 0.0f, 0.0f, 1.0f } );

}
