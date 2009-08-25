/*
 *  RandomMind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Mind.h"

namespace oz
{

  class RandomMind : public Mind
  {
    public:

      RandomMind( int botIndex ) : Mind( botIndex ) {}

      void update();

  };

}
