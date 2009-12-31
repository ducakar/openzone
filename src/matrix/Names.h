/*
 *  Names.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  struct Names
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
