/*
 *  Mat33.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */


#include "Mat33.hpp"

namespace oz
{

  const Mat33 Mat33::ZERO = Mat33( 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f );

  const Mat33 Mat33::ID   = Mat33( 1.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f );

}
