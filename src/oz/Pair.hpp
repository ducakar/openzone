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
       * Move constructor.
       */
      Pair( Pair&& ) = default;

      /**
       * Copy constructor for constructing from pairs of different types.
       * @param p
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      constexpr Pair( const Pair<TypeX_, TypeY_>& p ) : x( p.x ), y( p.y )
      {}

      /**
       * Move constructor for constructing from pairs of different types.
       * @param p
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      constexpr Pair( Pair<TypeX_, TypeY_>&& p ) :
          x( static_cast<TypeX_&&>( p.x ) ), y( static_cast<TypeY_&&>( p.y ) )
      {}

      /**
       * Copy operator.
       */
      Pair& operator = ( const Pair& ) = default;

      /**
       * Move operator.
       */
      Pair& operator = ( Pair&& ) = default;

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
       * Move operator for moving pairs of different types.
       * @param p
       * @return
       */
      template <typename TypeX_, typename TypeY_>
      OZ_ALWAYS_INLINE
      Pair& operator = ( Pair<TypeX_, TypeY_>&& p )
      {
        x = static_cast<TypeX_&&>( p.x );
        y = static_cast<TypeY_&&>( p.y );
        return *this;
      }

      /**
       * Constructor that initialises members.
       * @param x_
       * @param y_
       */
      template <typename ValueX, typename ValueY>
      OZ_ALWAYS_INLINE
      constexpr Pair( ValueX&& x_, ValueY&& y_ ) :
          x( static_cast<ValueX&&>( x_ ) ), y( static_cast<ValueY&&>( y_ ) )
      {}

      /**
       * Equality operator.
       * @param p
       * @return
       */
      OZ_ALWAYS_INLINE
      constexpr bool operator == ( const Pair& p ) const
      {
        return x == p.x && y == p.y;
      }

      /**
       * Inequality operator.
       * @param p
       * @return
       */
      OZ_ALWAYS_INLINE
      constexpr bool operator != ( const Pair& p ) const
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
      constexpr bool operator == ( const Pair<TypeX_, TypeY_>& p ) const
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
      constexpr bool operator != ( const Pair<TypeX_, TypeY_>& p ) const
      {
        return x != p.x || y != p.y;
      }

  };

}
