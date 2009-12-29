/*
 *  Proxy.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{
namespace client
{

  struct Proxy
  {
    public:

      virtual ~Proxy() {}

      virtual void begin()   = 0;
      virtual void update()  = 0;
      virtual void prepare() = 0;

  };

}
}
