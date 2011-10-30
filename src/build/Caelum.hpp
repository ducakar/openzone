/*
 *  Caelum.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "build/common.hpp"

namespace oz
{
namespace build
{

class Caelum
{
  private:

    static const float STAR_DIM;

  public:

    static void build( const char* name );

};

}
}
