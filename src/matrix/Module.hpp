/*
 *  Module.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Module
  {
    public:

      virtual ~Module()
      {}

      virtual void update()
      {}

      virtual void read( InputStream* )
      {}

      virtual void write( OutputStream* ) const
      {}

      virtual void load()
      {}

      virtual void unload()
      {}

      virtual void init()
      {}

      virtual void free()
      {}

  };

}
