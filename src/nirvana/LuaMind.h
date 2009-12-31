/*
 *  LuaMind.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Mind.h"

namespace oz
{
namespace nirvana
{

  struct LuaMind : Mind
  {
    public:

      static Mind* create( int botIndex );
      static Mind* read( InputStream* istream );

      LuaMind( int botIndex );
      virtual ~LuaMind();

      virtual const char* type() const;
      virtual void update();

  };

}
}
