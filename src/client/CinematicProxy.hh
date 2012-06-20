/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/CinematicProxy.hh
 */

#pragma once

#include "client/Proxy.hh"

namespace oz
{
namespace client
{

class CinematicProxy : public Proxy

{
  private:

    struct Step
    {
      enum Type
      {
        SWITCH,
        WAIT,
        MOVE
      };

      Quat  rot;
      Point p;
      float time;
      int   endState;
      Type  type;
    };

    Vector<Step> steps;

    Quat  beginRot;
    Point beginPos;
    float stepTime;

  public:

    CinematicProxy();

    void addStateSwitch( int endState );
    void addWait( float time );
    void addMove( const Quat& rot, const Point& p, float time );

    void begin() override;
    void end() override;

    void prepare() override;
    void update() override;

    void reset() override;

    void read( InputStream* istream ) override;
    void write( BufferStream* ostream ) const override;

};

}
}
