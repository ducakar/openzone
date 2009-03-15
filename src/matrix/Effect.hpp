/*
 *  Effect.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Effect : PoolAlloc<Effect, 0>
  {
    int    id;
    Effect *next[1];

    Effect()
    {}

    explicit Effect( int id_ ) : id( id_ )
    {}
  };

}
