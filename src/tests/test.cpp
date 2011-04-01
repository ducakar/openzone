/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <cstdio>

#include <libintl.h>

using namespace oz;

bool Alloc::isLocked = true;

int main( int, char** )
{
  bindtextdomain( OZ_APPLICATION_NAME, OZ_INSTALL_PREFIX );
  return 0;
}
