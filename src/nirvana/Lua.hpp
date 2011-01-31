/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/common.hpp"

struct lua_State;

namespace oz
{
namespace nirvana
{

  class Lua
  {
    private:

      lua_State* l;

      void callFunc( const char* functionName, int botIndex );

    public:

      Bot*            self;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;

      Vector<Object*> objects;
      Vector<Struct*> structs;

      List<Object::Event>::CIterator event;

      bool            forceUpdate;

      void call( const char* functionName, Bot* self_ )
      {
        forceUpdate = false;
        self        = self_;

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
