/*
 *  Arch.hpp
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
namespace arch
{

  class Arch
  {
    private:

    public:

      void apply();
      void sync();
      void update();

      void load( InputStream* istream );
      void unload( OutputStream* ostream );

      void init();
      void free();

  };

  extern Arch arch;

}
}
