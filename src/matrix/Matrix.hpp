/*
 *  Matrix.hpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Matrix
  {
    private:

      void loadStressTest();
      void loadSample();

    public:

      static const float MAX_VELOCITY2;

      void update();

      void load( InputStream* istream );
      void unload( OutputStream* ostream );

      void init();
      void free();

  };

  extern Matrix matrix;

}
