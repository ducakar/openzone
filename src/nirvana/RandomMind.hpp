/*
 *  RandomMind.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

#include "nirvana/Mind.hpp"

namespace oz
{
namespace nirvana
{

  class RandomMind : public Mind
  {
    public:

      static Pool<RandomMind> pool;

      static Mind* create( int bot );
      static Mind* read( InputStream* istream );

      explicit RandomMind( int bot ) : Mind( bot ) {}

      virtual const char* type() const;
      virtual void update();

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
