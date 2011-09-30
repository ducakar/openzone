/*
 *  Device.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/common.hpp"

namespace oz
{
namespace nirvana
{

  class Device
  {
    public:

      static const int MEMO_BIT     = 0x0001;
      static const int COMPUTER_BIT = 0x0002;

      typedef Device* ( * CreateFunc )( int id, InputStream* istream );

      int flags;

    protected:

      explicit Device( int flags_ ) : flags( flags_ )
      {}

      virtual void onUse( const Bot* user );
      virtual void onUpdate();

    public:

      virtual ~Device();

      virtual const char* type() const = 0;

      virtual const char* getMemo() const;

      virtual void write( OutputStream* ostream ) const;

  };

}
}
