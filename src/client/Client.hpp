/*
 *  Client.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
namespace client
{

  struct Settings
  {
    float mouseXSens;
    float mouseYSens;

    float keyXSens;
    float keyYSens;

    float moveStep;
    float runStep;
  };

  struct Input
  {
    ubyte *currKeys;
    ubyte keys[SDLK_LAST];

    struct Mouse
    {
      int  x;
      int  y;
      char b;
    }
    mouse;
  };

  class Client
  {
    private:

      Settings settings;

    public:

      bool init();
      void start();

      bool update( int time );

      void stop();
      void free();
  };

  extern Input input;
  extern Client client;

}
}
