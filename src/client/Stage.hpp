/*
 *  Stage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{

  class Stage
  {
    public:

      virtual ~Stage();

      virtual bool update()  = 0;
      virtual void present() = 0;

      virtual void begin()   = 0;
      virtual void end()     = 0;

      virtual void load()    = 0;
      virtual void unload()  = 0;

      virtual void init()    = 0;
      virtual void free()    = 0;

  };

}
}
