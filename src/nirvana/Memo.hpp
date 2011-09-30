/*
 *  Memo.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/Device.hpp"

namespace oz
{
namespace nirvana
{

  class Memo : public Device
  {
    private:

      String text;

    public:

      explicit Memo( const char* text );

      virtual const char* getMemo() const;

  };

}
}
