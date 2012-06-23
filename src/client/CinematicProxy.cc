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

#include "stable.hh"

#include "client/CinematicProxy.hh"

#include "client/Camera.hh"

#include "client/ui/GalileoFrame.hh"
#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

CinematicProxy::CinematicProxy()
{
  reset();
}

void CinematicProxy::addStateSwitch( int endState )
{
  Step step = { Quat::ID, Point::ORIGIN, 0.0f, endState, Step::SWITCH };
  steps.add( step );
}

void CinematicProxy::addWait( float time )
{
  Step step = { Quat::ID, Point::ORIGIN, time, Camera::CINEMATIC, Step::WAIT };
  steps.add( step );
}

void CinematicProxy::addMove( const Quat& rot, const Point& p, float time )
{
  Step step = { rot, p, time, Camera::CINEMATIC, Step::MOVE };
  steps.add( step );
}

void CinematicProxy::begin()
{
  ui::ui.galileoFrame->show( false );
  ui::mouse.doShow = false;

  beginRot = camera.rot;
  beginPos = camera.p;
  stepTime = 0.0f;
}

void CinematicProxy::end()
{
  steps.clear();
  steps.dealloc();
}

void CinematicProxy::prepare()
{}

void CinematicProxy::update()
{
  if( !steps.isEmpty() ) {
    const Step& step = steps.first();

    switch( step.type ) {
      case Step::SWITCH: {
        camera.setState( Camera::State( step.endState ) );
        break;
      }
      case Step::WAIT: {
        if( stepTime >= step.time ) {
          stepTime = 0.0f;
          steps.popFirst();
        }
        else {
          stepTime += Timer::TICK_TIME;
        }
        break;
      }
      case Step::MOVE: {
        float t = min( stepTime / step.time, 1.0f );

        camera.smoothRotateTo( Quat::slerp( beginRot, step.rot, t ) );
        camera.smoothMoveTo( Math::mix( beginPos, step.p, t ) );
        camera.align();

        if( t == 1.0f ) {
          beginPos = step.p;
          beginRot = step.rot;
          stepTime = 0.0f;

          steps.popFirst();
        }
        else {
          stepTime += Timer::TICK_TIME;
        }
        break;
      }
    }
  }
}

void CinematicProxy::reset()
{
  beginRot = Quat::ID;
  beginPos = Point::ORIGIN;
  stepTime = 0.0f;

  steps.clear();
  steps.dealloc();
}

void CinematicProxy::read( InputStream* istream )
{
  beginRot = istream->readQuat();
  beginPos = istream->readPoint();
  stepTime = istream->readFloat();

  int nSteps = istream->readInt();
  for( int i = 0; i < nSteps; ++i ) {
    Step step;

    step.rot      = istream->readQuat();
    step.p        = istream->readPoint();
    step.time     = istream->readFloat();
    step.endState = istream->readInt();
    step.type     = Step::Type( istream->readInt() );

    steps.add( step );
  }
}

void CinematicProxy::write( BufferStream* ostream ) const
{
  ostream->writeQuat( beginRot );
  ostream->writePoint( beginPos );
  ostream->writeFloat( stepTime );

  ostream->writeInt( steps.length() );
  for( int i = 0; i < steps.length(); ++i ) {
    const Step& step = steps[i];

    ostream->writeQuat( step.rot );
    ostream->writePoint( step.p );
    ostream->writeFloat( step.time );
    ostream->writeInt( step.endState );
    ostream->writeInt( step.type );
  }
}

}
}
