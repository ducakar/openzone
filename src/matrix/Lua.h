/*
 *  Lua.h
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Bot.h"

struct lua_State;

namespace oz
{

  class  Object;
  class  Bot;

  struct Lua
  {
    private:

      static const char *HANDLERS_FILE;

      lua_State *l;

      void callFunc( const char *functionName );

    public:

      Object *self;
      Object *user;
      Object *created;

      // point to either self, user or created
      Object **obj;

      float  damage;
      float  hitMomentum;

      void call( const char *functionName, Object *self_, Bot *user_ = null )
      {
        self   = self_;
        user   = user_;

        callFunc( functionName );
      }

    public:

      void init();
      void free();
  };

  extern Lua lua;

}
