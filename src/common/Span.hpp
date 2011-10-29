/*
 *  Span.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file common/Span.hpp
 */

#include "oz/oz.hpp"

namespace oz
{

/**
 * Span.
 *
 * 2D integer span.
 */
struct Span
{
  public:

    int minX; ///< Minimum X.
    int minY; ///< Minimum Y.
    int maxX; ///< Maximum X.
    int maxY; ///< Maximum Y.

    /**
     * Uninitialised instance.
     */
    Span() = default;

    /**
     * Construct with the given values.
     */
    OZ_ALWAYS_INLINE
    explicit Span( int minX_, int minY_, int maxX_, int maxY_ ) :
        minX( minX_ ), minY( minY_ ), maxX( maxX_ ), maxY( maxY_ )
    {}

};

}
