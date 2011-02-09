/*
 *  Quat.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */


#include "Quat.hpp"

namespace oz
{

#ifdef OZ_SIMD

  const Quat Quat::ZERO = Quat( 0.0f, 0.0f, 0.0f, 0.0f );
  const Quat Quat::ID   = Quat( 0.0f, 0.0f, 0.0f, 1.0f );

#else

  const Quat Quat::ZERO = Quat( 0.0f, 0.0f, 0.0f, 0.0f );
  const Quat Quat::ID   = Quat( 0.0f, 0.0f, 0.0f, 1.0f );

#endif

}
