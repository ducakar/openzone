/*
 *  Caelum.hpp
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

  class Caelum
  {
    public:

      int   id;

      float heading;
      float period;
      float time;

      Caelum();

      void update();

      void read( InputStream* istream );
      void write( OutputStream* ostream ) const;

  };

}
