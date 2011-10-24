/*
 *  Pair.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Pair.hpp
 */

#include "common.hpp"

namespace oz
{

/**
 * %Pair container.
 *
 * It holds two elements, not necessarily of the same type.
 */
template <typename TypeX, typename TypeY = TypeX>
class Pair
{
  public:

    TypeX x; ///< First element.
    TypeY y; ///< Second element.

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    Pair()
    {}

    /**
     * Create a pair with given members.
     */
    OZ_ALWAYS_INLINE
    explicit Pair( const TypeX& x_, const TypeY& y_ ) : x( x_ ), y( y_ )
    {}

    /**
     * Per-member equality operator.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Pair& p ) const
    {
      return x == p.x && y == p.y;
    }

    /**
     * Per-member inequality operator.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Pair& p ) const
    {
      return x != p.x || y != p.y;
    }

};

}
