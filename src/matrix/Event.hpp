/*
 *  Event.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Event : PoolAlloc<Event, 0>
  {
    static const int HIT           = 0;
    static const int SPLASH_SOFT   = 1;
    static const int SPLASH_HARD   = 2;

    int   id;
    Event *next[1];

    Event()
    {}

    explicit Event( int id_ ) : id( id_ )
    {}
  };

}
