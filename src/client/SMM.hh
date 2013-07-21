/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/SMM.hh
 *
 * Single mesh model.
 *
 * Common model format that all simple models are compiled to.
 */

#pragma once

#include <client/Mesh.hh>

namespace oz
{
namespace client
{

class SMM
{
  public:

    typedef SMM* CreateFunc( int id );

  protected:

    int  id;
    Mesh mesh;

    explicit SMM( int id );

  public:

    static SMM* create( int id );
    virtual ~SMM();

    Vec3 dim() const
    {
      return mesh.dim;
    }

    bool isPreloaded() const
    {
      return mesh.isPreloaded();
    }

    bool isLoaded() const
    {
      return mesh.isLoaded();
    }

    void schedule( int component, Mesh::QueueType queue )
    {
      mesh.schedule( component, queue );
    }

    virtual void preload();
    virtual void load();

};

}
}
