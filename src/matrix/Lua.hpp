/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"

struct lua_State;

namespace oz
{

  class Struct;
  class Bot;
  class Particle;

  class Lua
  {
    private:

      lua_State* l;

      void callFunc( const char* functionName, int index );

    public:

      Object*         self;
      Bot*            user;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;

      float           damage;
      float           hitMomentum;

      Vector<Object*> objects;
      Vector<Struct*> structs;

      List<Object::Event>::Iterator event;

      void call( const char* functionName, Object* self_, Bot* user_ = null )
      {
        self   = self_;
        user   = user_;

        callFunc( functionName, self->index );
      }

      void registerObject( int index );
      void unregisterObject( int index );

      void init();
      void free();
  };

  extern Lua lua;

}
