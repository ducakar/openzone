/*
 *  Pair.hpp
 *
 *  Generic pair template
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  template <typename TypeX, typename TypeY = TypeX>
  class Pair
  {
    public:

      TypeX x;
      TypeY y;

      Pair() {}
      Pair( const TypeX& x_, const TypeY& y_ ) : x( x_ ), y( y_ ) {}

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

      void set( const TypeX& x_, const TypeY& y_ )
      {
        x = x_;
        y = y_;
      }

  };

  template <typename TypeX, typename TypeY>
  inline Pair<TypeX, TypeY> pair( const TypeX& x, const TypeY& y )
  {
    return Pair<TypeX, TypeY>( x, y );
  }

  template <typename TypeX, typename TypeY>
  inline Pair<TypeX&, TypeY&> tie( TypeX& x, TypeY& y )
  {
    return Pair<TypeX&, TypeY&>( x, y );
  }

}
