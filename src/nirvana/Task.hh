/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file nirvana/Task.hh
 */

#pragma once

#include "nirvana/common.hh"

namespace oz
{
namespace nirvana
{

class Mind;

class Task
{
  public:

    static const int ACTIVE_BIT = 0x00000001;

    typedef Task* ( * CreateFunc )( const Task* parent );
    typedef Task* ( * ReadFunc )( InputStream* istream, const Task* parent );

    Task*       prev[1];
    Task*       next[1];

    int         flags;

  private:

    Mind*       mind;
    Task*       parent;
    DList<Task> children;

  public:

    static Task* create( const Task* parent );
    static Task* read( InputStream* istream, const Task* parent );

    explicit Task( Mind* mind_, Task* parent_ ) : flags( 0 ), mind( mind_ ), parent( parent_ )
    {}

    virtual ~Task();

    virtual const char* type() const = 0;

    void addChild( Task* child )
    {
      children.pushLast( child );
    }

    void update()
    {
      if( onUpdate() ) {
        while( children.first() != null && !( children.first()->flags & ACTIVE_BIT ) ) {
          delete children.popFirst();
        }
        if( children.first() != null ) {
          children.first()->update();
        }
      }
    }

  protected:

    // return true if children should be updated too
    virtual bool onUpdate() = 0;

  public:

    virtual void write( BufferStream* ostream ) const;

};

}
}
