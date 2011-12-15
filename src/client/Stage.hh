/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Stage.hh
 */

#pragma once

#include "client/common.hh"

namespace oz
{
namespace client
{

class Stage
{
  public:

    static Stage* nextStage;

    virtual ~Stage();

    virtual bool update()  = 0;
    virtual void present() = 0;

    virtual void load()    = 0;
    virtual void unload()  = 0;

    virtual void init()    = 0;
    virtual void free()    = 0;

};

}
}
