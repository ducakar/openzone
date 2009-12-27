/*
 *  Stage.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{
namespace client
{

  class Stage
  {
    public:

      virtual ~Stage() {}

      virtual bool update() = 0;
      virtual void render() = 0;

      virtual void load()   = 0;
      virtual void unload() = 0;

      virtual void begin()  = 0;
      virtual void end()    = 0;

  };

}
}
