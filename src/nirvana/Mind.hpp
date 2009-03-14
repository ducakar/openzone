/*
 *  Mind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Bot.hpp"

namespace oz
{

  class Mind
  {
    protected:

      virtual void onUpdate();

    public:

      Mind *prev[1];
      Mind *next[1];

      bool isUpdated;
      Bot  *body;

      Mind() : isUpdated( true )
      {}

      void update()
      {
        isUpdated = true;
        onUpdate();
      }
  };

}
