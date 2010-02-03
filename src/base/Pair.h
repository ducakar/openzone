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

  template <typename TypeA, typename TypeB = TypeA>
  class Pair
  {
    public:

      TypeA x;
      TypeB y;

      Pair() {}
      Pair( const TypeA& x_, const TypeB& y_ ) : x( x_ ), y( y_ ) {}

      Pair& operator = ( const Pair& p )
      {
        x = p.x;
        y = p.y;
        return *this;
      }

      bool operator == ( const Pair& p ) const
      {
        return x == p.x && y == p.y;
      }

      bool operator != ( const Pair& p ) const
      {
        return x != p.x || y != p.y;
      }

      static Pair zero()
      {
        return Pair( TypeA( 0 ), TypeB( 0 ) );
      }

      bool isZero() const
      {
        return x == TypeA( 0 ) && y == TypeB( 0 );
      }

      const Pair& setZeto()
      {
        x = TypeA( 0 );
        y = TypeB( 0 );
      }

      const Pair& operator + () const
      {
        return *this;
      }

      Pair operator - () const
      {
        return Pair( -x, -y );
      }

      const Pair& operator += ( const Pair& p )
      {
        x += p.x;
        y += p.y;
        return *this;
      }

      const Pair& operator -= ( const Pair& p )
      {
        x -= p.x;
        y -= p.y;
        return *this;
      }

      const Pair& operator *= ( const Pair& p )
      {
        x *= p.x;
        y *= p.y;
        return *this;
      }

      const Pair& operator /= ( const Pair& p )
      {
        assert( p.x != TypeA( 0 ) && p.y != TypeB( 0 ) );

        x /= p.x;
        y /= p.y;
        return *this;
      }

      const Pair& operator *= ( const TypeA& k )
      {
        x *= k;
        y *= k;
        return *this;
      }

      const Pair& operator /= ( const TypeA& k )
      {
        assert( k != Type( 0 ) );

        x /= k;
        y /= k;
        return *this;
      }

      const Pair& operator %= ( const TypeA& k )
      {
        assert( k != Type( 0 ) );

        x %= k;
        y %= k;
        return *this;
      }

      Pair operator + ( const Pair& p ) const
      {
        return Pair( x + p.x, y + p.y );
      }

      Pair operator - ( const Pair& p ) const
      {
        return Pair( x - p.x, y - p.y );
      }

      Pair operator * ( const Pair& p ) const
      {
        return Pair( x * p.x, y * p.y );
      }

      Pair operator / ( const Pair& p ) const
      {
        assert( p.x != Type( 0 ) && p.y != Type( 0 ) );

        return Pair( x / p.x, y / p.y );
      }

      Pair operator % ( const Pair& p ) const
      {
        assert( p.x != Type( 0 ) && p.y != Type( 0 ) );

        return Pair( x % p.x, y % p.y );
      }

      Pair operator * ( const TypeA& k ) const
      {
        return Pair( x * k, y * k );
      }

      Pair operator / ( const TypeA& k ) const
      {
        assert( k != TypeA( 0 ) );

        return Pair( x / k, y / k );
      }

      Pair operator % ( const TypeA& k ) const
      {
        assert( k != TypeA( 0 ) );

        return Pair( x % k, y % k );
      }

      friend Pair operator * ( const TypeA& k, const Pair& p )
      {
        return Pair( p.x * k, p.y * k );
      }

  };

  template <typename TypeA, typename TypeB>
  inline Pair<TypeA, TypeB> pair( const TypeA& x, const TypeB& y )
  {
    return Pair<Type>( x, y );
  }

  template <typename TypeA, typename TypeB>
  inline Pair<TypeA&, TypeB&> tiePair( TypeA& x, TypeB& y )
  {
    return Pair<TypeA&, TypeB&>( x, y );
  }

}
