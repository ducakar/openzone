/*
 *  Nirvana.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/Mind.hpp"

namespace oz
{
namespace nirvana
{

  class Nirvana
  {
    private:

      // mind is only updated once per UPDATE_INTERVAL frames (maybe less, depends on when it was
      // added to the list)
      // to force update, set Mind::FORCE_UPDATE_BIT
      static const int UPDATE_INTERVAL = 32;

      DList<Mind> minds;
      int updateModulo;

    public:

      void sync();
      void update();

      void read( InputStream* istream );
      void write( OutputStream* ostream ) const;

      void load();
      void unload();

      void init();
      void free();

  };

  extern Nirvana nirvana;

}
}
