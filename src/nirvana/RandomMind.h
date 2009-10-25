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

      static Mind *create( int botIndex );
      static Mind *read( InputStream *istream );

      RandomMind( int botIndex ) : Mind( botIndex ) {}

      virtual const char *type() const;
      virtual void update();

  };

}
