/*
 *  Pair.h
 *
 *  Generic pair template, also provide vector-like functionality.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <typename Type0, typename Type1 = Type0>
  class Pair
  {
    public:

      Type0 x;
      Type1 y;

      Pair() {}
      Pair( const Type0& x_, const Type1& y_ ) : x( x_ ), y( y_ ) {}

      template <typename Type0_, typename Type1_>
      Pair( const Pair<Type0_, Type1_>& p ) : x( p.x ), y( p.y ) {}

      template <typename Type0_, typename Type1_>
      Pair& operator = ( const Pair<Type0_, Type1_>& p )
      {
        x = p.x;
        y = p.y;
        return *this;
      }

      template <typename Type0_, typename Type1_>
      bool operator == ( const Pair<Type0_, Type1_>& p ) const
      {
        return x == p.x && y == p.y;
      }

      template <typename Type0_, typename Type1_>
      bool operator != ( const Pair<Type0_, Type1_>& p ) const
      {
        return x != p.x || y != p.y;
      }

      void set( const Type0& x_, const Type1& y_ )
      {
        x = x_;
        y = y_;
      }

  };

  template <typename Type0, typename Type1>
  inline Pair<Type0, Type1> pair( const Type0& x, const Type1& y )
  {
    return Pair<Type0, Type1>( x, y );
  }

  template <typename Type0, typename Type1>
  inline Pair<Type0&, Type1&> tie( Type0& x, Type1& y )
  {
    return Pair<Type0&, Type1&>( x, y );
  }

}
