/*
 *  Pair.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/Pair.hpp
 */

#include "common.hpp"

namespace oz
{

/**
 * %Pair container.
 *
 * It holds two elements, not necessarily of the same type.
 *
 * @ingroup oz
 */
template <typename ValueX, typename ValueY = ValueX>
class Pair
{
  public:

    ValueX x; ///< First element.
    ValueY y; ///< Second element.

    /**
     * Create an uninitialised instance.
     */
    Pair() = default;

    /**
     * Create a pair with given members.
     */
    template <typename ValueX_, typename ValueY_>
    OZ_ALWAYS_INLINE
    explicit constexpr Pair( ValueX_&& x_, ValueY_&& y_ ) :
        x( static_cast<ValueX_&&>( x_ ) ), y( static_cast<ValueY_&&>( y_ ) )
    {}

    /**
     * Per-member equality operator.
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator == ( const Pair& p ) const
    {
      return x == p.x && y == p.y;
    }

    /**
     * Per-member inequality operator.
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator != ( const Pair& p ) const
    {
      return x != p.x || y != p.y;
    }

};

}
