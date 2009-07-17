/*
 *  Net.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "io.h"

namespace oz
{

  class Net
  {
    public:

      static const int BUFFER_SIZE = 4096;

      enum ObjectType
      {
        PARTICLE,
        OBJECT,
        STRUCTURE,
        BSP
      };

      enum ActionType
      {
        ADD,
        ADD_NOPUT,
        REMOVE,
        PUT,
        CUT,
        CLEAR,
        USE,
        TAKE,
        ACTION
      };

      struct Action
      {
        ActionType type;
        int        index;
        int        param;

        Action() {}
        Action( ActionType type_ ) : type( type_ ) {}
        Action( ActionType type_, int index_ ) : type( type_ ), index( index_ ) {}
        Action( ActionType type_, int index_, int param_ ) :
            type( type_ ), index( index_ ), param( param_ )
        {}
      };

    public:

      // isSingle XOR isServer XOR isClient

      // Singleplayer. No need to worry about object creation, deletion, synchronization etc.
      bool isSingle;
      // If server, create notification about every object creation/deletion/put/cut/etc. That can
      // be later serialized and sent to clients.
      bool isServer;
      // If client, do not create or delete any objects by yourself. Every creation/deletion must be
      // ordered by server.
      bool isClient;

      Vector<Action> particles;
      Vector<Action> objects;
      Vector<Action> structs;
      Vector<Action> bsps;
      Vector<Action> world;

      Net();

  };

  extern Net net;

}
