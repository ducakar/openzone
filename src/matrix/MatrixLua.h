/*
 *  MatrixLua.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  class Object;
  class Bot;

  struct MatrixLua
  {
    private:

      void callFunc( const char *functionName );

    public:

      Object *obj;
      Bot    *user;
      Object *target;
      float  damage;
      float  hitMomentum;

      void init();
      void free();

      void call( const char *functionName, Object *obj_, Bot *user_ = null )
      {
        obj    = obj_;
        user   = user_;
        target = obj_;

        callFunc( functionName );
      }
  };

  extern MatrixLua matrixLua;

}
