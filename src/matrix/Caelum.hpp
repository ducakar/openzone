/*
 *  Caelum.hpp
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

  class Caelum
  {
    public:

      float time;
      float period;
      float heading;

      void update();

      void read( InputStream* istream );
      void write( OutputStream* ostream );

  };

}
