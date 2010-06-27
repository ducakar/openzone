/*
 *  Proxy.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{
namespace client
{

  class Proxy
  {
    public:

      virtual ~Proxy() {}

      virtual void begin()   = 0;
      virtual void update()  = 0;
      virtual void prepare() = 0;

  };

}
}
