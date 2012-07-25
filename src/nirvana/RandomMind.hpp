/*
 *  RandomMind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "nirvana/Mind.hpp"

namespace oz
{
namespace nirvana
{

  class RandomMind : public Mind
  {
    public:

      static Pool<RandomMind> pool;

      static Mind* create( int botIndex );
      static Mind* read( InputStream* istream );

      RandomMind( int botIndex ) : Mind( botIndex ) {}

      virtual const char* type() const;
      virtual void update();

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
