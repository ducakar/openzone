/*
 *  Names.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Names
  {
    private:

      static const int LINE_LENGTH = 64;

      Vector<String> names;

    public:

      const String& genName() const;

      void init();
      void free();

  };

  extern Names names;

}
