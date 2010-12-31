/*
 *  LuaMind.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/Mind.hpp"

namespace oz
{
namespace nirvana
{

  class LuaMind : public Mind
  {
    public:

      static Mind* create( int bot );
      static Mind* read( InputStream* istream );

      static Pool<LuaMind> pool;

      explicit LuaMind( int bot );
      virtual ~LuaMind();

      virtual const char* type() const;
      virtual void update();

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
}
