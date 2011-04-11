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
      Pair() = default;

      /**
       * Copy constructor.
       */
      Pair( const Pair& ) = default;

      /**
       * Copy constructor for constructing from pairs of different types.
       * @param p
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      Pair( const Pair<TypeX_, TypeY_>& p ) : x( p.x ), y( p.y )
      {}

      /**
       * Copy operator.
       */
      Pair& operator = ( const Pair& ) = default;

      /**
       * Copy operator for copying pairs of different types.
       * @param p
       * @return
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      Pair& operator = ( const Pair<TypeX_, TypeY_>& p )
      {
        x = p.x;
        y = p.y;
        return *this;
      }

      /**
       * Constructor that initialises members.
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

      /**
       * Equality operator for comparing pairs of different types.
       * @param p
       * @return
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      bool operator == ( const Pair<TypeX_, TypeY_>& p ) const
      {
        return x == p.x && y == p.y;
      }

      /**
       * Inequality operator for comparing pairs of different types.
       * @param p
       * @return
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      bool operator != ( const Pair<TypeX_, TypeY_>& p ) const
      {
        return x != p.x || y != p.y;
      }

  };

  /**
   * Utility for constructing a pair.
   * @param x
   * @param y
   * @return
   */
  template <typename TypeX, typename TypeY>
  OZ_ALWAYS_INLINE
  inline Pair<TypeX, TypeY> pair( const TypeX& x, const TypeY& y )
  {
    return Pair<TypeX, TypeY>( x, y );
  }

}
