/*
 *  Quat.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */


#include "Quat.hpp"

namespace oz
{

  const Quat Quat::ZERO = Quat( (float4) { 0.0f, 0.0f, 0.0f, 0.0f } );
  const Quat Quat::ID   = Quat( (float4) { 0.0f, 0.0f, 0.0f, 1.0f } );

}
