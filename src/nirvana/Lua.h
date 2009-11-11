/*
 *  Lua.h
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/Bot.h"

struct lua_State;

namespace oz
{
namespace nirvana
{

  struct Lua
  {
    private:

      lua_State *l;

      void callFunc( const char *functionName, int botIndex );

    public:

      Bot    *self;
      Object *target;

      // point to either self, user or created
      Object **obj;

      int    index;
      Vector<Object*> objects;

      void call( const char *functionName, Bot *self_ )
      {
        self    = self_;

        callFunc( functionName, self->index );
      }

    public:

      // create a table for a mind that can act as mind's local storage, mind's local variables
      void registerMind( int botIndex );
      void unregisterMind( int botIndex );

      void init();
      void free();
  };

  extern Lua lua;

}
}
