/*
 *  Module.hpp
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

  class Module
  {
    public:

      static const int UPDATE_FUNC_BIT = 0x01;

      int flags;

      Module() : flags( 0 )
      {}

      virtual ~Module()
      {}

      virtual void onUpdate()
      {}

      virtual void onRegister()
      {}

  };

}
