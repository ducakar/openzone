/*
 *  Pair.h
 *
 *  Generic pair template, also provide vector-like functionality.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <typename Type>
  struct Pair
  {
    Type x;
    Type y;

    Pair()
    {}

    Pair( const Type& x_, const Type& y_ ) : x( x_ ), y( y_ )
    {}

    Pair( const Type* p )
    {
      *this = reinterpret_cast<const Type*>( p );
    }

    static Pair zero()
    {
      return Pair( Type( 0 ), Type( 0 ) );
    }

    operator Type* ()
    {
      return reinterpret_cast<Type*>( this );
    }

    operator const Type* () const
    {
      return reinterpret_cast<const Type*>( this );
    }

    Type& operator [] ( int i )
    {
      assert( 0 <= i && i < 2 );

      return reinterpret_cast<Type*>( this )[i];
    }

    const Type& operator [] ( int i ) const
    {
      assert( 0 <= i && i < 2 );

      return reinterpret_cast<const Type*>( this )[i];
    }

    bool isZero() const
    {
      return x == Type( 0 ) && y == Type( 0 );
    }

    const Pair& setZeto()
    {
      x = Type( 0 );
      y = Type( 0 );
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
      assert( p.x != Type( 0 ) && p.y != Type( 0 ) );

      x /= p.x;
      y /= p.y;
      return *this;
    }

    const Pair& operator *= ( const Type& k )
    {
      x *= k;
      y *= k;
      return *this;
    }

    const Pair& operator /= ( const Type& k )
    {
      assert( k != Type( 0 ) );

      x /= k;
      y /= k;
      return *this;
    }

    const Pair& operator %= ( const Type& k )
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

    Pair operator * ( const Type& k ) const
    {
      return Pair( x * k, y * k );
    }

    Pair operator / ( const Type& k ) const
    {
      assert( k != Type( 0 ) );

      return Pair( x / k, y / k );
    }

    Pair operator % ( const Type& k ) const
    {
      assert( k != Type( 0 ) );

      return Pair( x % k, y % k );
    }

    friend Pair operator * ( const Type& k, const Pair& p )
    {
      return Pair( p.x * k, p.y * k );
    }
  };

}
