/*
 *  Lua.h
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Object.h"

struct lua_State;

namespace oz
{

  struct Structure;
  struct Bot;
  struct Particle;

  struct Lua
  {
    private:

      static const char* HANDLERS_FILE;

      lua_State* l;

      void callFunc( const char* functionName, int index );

    public:

      Object*            self;
      Bot*               user;

      Structure*         str;
      Object*            obj;
      Particle*          part;
      List<Object::Event>::Iterator event;

      int                strIndex;
      int                objIndex;

      float              damage;
      float              hitMomentum;

      Vector<Object*>    objects;
      Vector<Structure*> structs;

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
