/*
 *  Matrix.hpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Module.hpp"

namespace oz
{

  class Matrix
  {
    public:

      static const float MAX_VELOCITY2;

    private:

      Vector<Module*> modules;

    public:

      void update();

      void read( InputStream* istream );
      void write( OutputStream* ostream ) const;

      void load();
      void unload();

      void init();
      void free();

  };

  extern Matrix matrix;

}
