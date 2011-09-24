/*
 *  Pair.hpp
 *
 *  Generic pair template
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  /**
   * Pair container.
   * It can hold two elements of different types.
   */
  template <typename TypeX, typename TypeY = TypeX>
  class Pair
  {
    public:

      TypeX x;
      TypeY y;

      /**
       * Default constructor.
       */
      OZ_ALWAYS_INLINE
      Pair()
      {}

      /**
       * Constructor with member initialisation.
       * @param x_
       * @param y_
       */
      OZ_ALWAYS_INLINE
      explicit Pair( const TypeX& x_, const TypeY& y_ ) : x( x_ ), y( y_ )
      {}

      /**
       * Equality operator.
       * @param p
       * @return
       */
      OZ_ALWAYS_INLINE
      bool operator == ( const Pair& p ) const
      {
        return x == p.x && y == p.y;
      }

      /**
       * Inequality operator.
       * @param p
       * @return
       */
      OZ_ALWAYS_INLINE
      bool operator != ( const Pair& p ) const
      {
        return x != p.x || y != p.y;
      }

  };

}
