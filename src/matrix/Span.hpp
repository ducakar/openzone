/*
 *  common.hpp
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  struct Span
  {
    int minX;
    int minY;
    int maxX;
    int maxY;

    Span() = default;

    OZ_ALWAYS_INLINE
    explicit Span( int minX_, int minY_, int maxX_, int maxY_ ) :
        minX( minX_ ), minY( minY_ ), maxX( maxX_ ), maxY( maxY_ )
    {}
  };

}
