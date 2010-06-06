/*
 *  assert.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include <cassert>

#define soft_assert( cond ) \
  ( ( cond ) ? static_cast<void>( 0 ) : oz::_softAssert( #cond, __FILE__, __LINE__ ) )

namespace oz
{

  void _softAssert( const char* message, const char* file, int line );

}
