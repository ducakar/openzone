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

    public:

      int botIndex;

      Mind( int botIndex_ ) : botIndex( botIndex_ ) {}

      virtual ~Mind();
      virtual void update() = 0;

      virtual void readFull( InputStream *istream );
      virtual void writeFull( OutputStream *ostream );

  };

}
