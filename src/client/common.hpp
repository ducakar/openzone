/*
 *  common.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  struct TexCoord
  {
    static const TexCoord ZERO;

    float u;
    float v;

    OZ_ALWAYS_INLINE
    TexCoord()
    {}

    OZ_ALWAYS_INLINE
    explicit TexCoord( float u_, float v_ ) : u( u_ ), v( v_ )
    {}

    OZ_ALWAYS_INLINE
    bool operator == ( const TexCoord& tc ) const
    {
      return u == tc.u && v == tc.v;
    }

    OZ_ALWAYS_INLINE
    bool operator != ( const TexCoord& tc ) const
    {
      return u != tc.u || v != tc.v;
    }

    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &u;
    }

    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &u;
    }

    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }

    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }
  };

}
}
