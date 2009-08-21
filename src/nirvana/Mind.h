/*
 *  Mind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Bot.h"

namespace oz
{

  class Mind
  {
    friend class DList<Mind, 0>;

    private:

      Mind *prev[1];
      Mind *next[1];

    protected:

      virtual void onUpdate() = 0;

    public:

      int  botIndex;
      Bot  *bot;

      Mind( Bot *bot_ ) : bot( bot_ ) {}

      virtual ~Mind();

      void update()
      {
        onUpdate();
      }
  };

}
