/*
 *  Pair.hpp
 *
 *  Generic pair template
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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
      explicit Pair()
      {}

      /**
       * Constructor that initialises members.
       * @param x_
       * @param y_
       */
      explicit Pair( const TypeX& x_, const TypeY& y_ ) : x( x_ ), y( y_ )
      {}

      /**
       * Copy constructor for constructing from pairs of different types.
       * @param p
       */
      template <typename TypeX_, typename TypeY_>
      Pair( const Pair<TypeX_, TypeY_>& p ) : x( p.x ), y( p.y )
      {}

      /**
       * Copy operator for copying pairs of different types.
       * @param p
       * @return
       */
      template <typename TypeX_, typename TypeY_>
      Pair& operator = ( const Pair<TypeX_, TypeY_>& p )
      {
        x = p.x;
        y = p.y;
        return *this;
      }

      /**
       * Equality operator.
       * @param p
       * @return
       */
      bool operator == ( const Pair& p ) const
      {
        return x == p.x && y == p.y;
      }

      /**
       * Inequality operator.
       * @param p
       * @return
       */
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
      bool operator != ( const Pair<TypeX_, TypeY_>& p ) const
      {
        return x != p.x || y != p.y;
      }

      /**
       * Set both values at once.
       * @param x_
       * @param y_
       */
      void set( const TypeX& x_, const TypeY& y_ )
      {
        x = x_;
        y = y_;
      }

  };

  /**
   * Utility for constructing a pair.
   * @param x
   * @param y
   * @return
   */
  template <typename TypeX, typename TypeY>
  inline Pair<TypeX, TypeY> pair( const TypeX& x, const TypeY& y )
  {
    return Pair<TypeX, TypeY>( x, y );
  }

}
