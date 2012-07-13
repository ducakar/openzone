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

#include "matrix/Library.hh"

#include "client/Camera.hh"
#include "client/Sound.hh"

#include "client/ui/GalileoFrame.hh"
#include "client/ui/MusicPlayer.hh"
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
  Step step = { Quat::ID, Point::ORIGIN, Mat44::ID, -1, null, 0.0f, endState };
  steps.add( step );
}

void CinematicProxy::addTransform( const Quat& rot, const Point& p, const Mat44& colour, int track,
                                   const char* title, float time )
{
  Step step = { rot, p, colour, track, title, time, Camera::CINEMATIC };
  steps.add( step );
}

void CinematicProxy::executeSequence( const char* path )
{
  JSON sequence;
  PFile file( path );

  if( !sequence.load( &file ) ) {
    throw Exception( "Failed to load sequence from '%s'", file.path().cstr() );
  }

  int nSteps = sequence.length();
  if( nSteps <= 0 ) {
    return;
  }

  Step step = { camera.rot, camera.p, camera.colour, -1, null, 0.0f, Camera::CINEMATIC };

  steps.alloc( nSteps );

  for( int i = 0; i < nSteps; ++i ) {
    const JSON& stepConfig = sequence[i];

    const JSON& rotArray = stepConfig["rot"];

    if( !rotArray.isNull() ) {
      float h = Math::rad( rotArray[0].get( 0.0f ) );
      float v = Math::rad( rotArray[1].get( 0.0f ) );
      float w = Math::rad( rotArray[2].get( 0.0f ) );

      step.rot = Quat::rotationZXZ( h, v, w );
    }

    const JSON& posArray = stepConfig["pos"];

    if( !posArray.isNull() ) {
      float x = posArray[0].get( 0.0f );
      float y = posArray[1].get( 0.0f );
      float z = posArray[2].get( 0.0f );

      step.p = Point( x, y, z );
    }

    const JSON& colourArray = stepConfig["colour"];

    if( !colourArray.isNull() ) {
      float m11 = colourArray[ 0].get( 1.0f );
      float m12 = colourArray[ 1].get( 0.0f );
      float m13 = colourArray[ 2].get( 0.0f );
      float m14 = colourArray[ 3].get( 0.0f );
      float m21 = colourArray[ 4].get( 0.0f );
      float m22 = colourArray[ 5].get( 1.0f );
      float m23 = colourArray[ 6].get( 0.0f );
      float m24 = colourArray[ 7].get( 0.0f );
      float m31 = colourArray[ 8].get( 0.0f );
      float m32 = colourArray[ 9].get( 0.0f );
      float m33 = colourArray[10].get( 1.0f );
      float m34 = colourArray[11].get( 0.0f );
      float m41 = colourArray[12].get( 0.0f );
      float m42 = colourArray[13].get( 0.0f );
      float m43 = colourArray[14].get( 0.0f );
      float m44 = colourArray[15].get( 1.0f );

      step.colour = Mat44( m11, m21, m31, m41,
                           m12, m22, m32, m42,
                           m13, m23, m33, m43,
                           m14, m24, m34, m44 );
    }

    const JSON& trackConfig = stepConfig["track"];
    if( trackConfig.isNull() ) {
      step.track = -1;
    }
    else if( trackConfig.asString().isEmpty() ) {
      step.track = -2;
    }
    else {
      step.track = library.musicIndex( trackConfig.asString() );
    }

    const JSON& titleConfig = stepConfig["title"];
    if( titleConfig.isNull() ) {
      step.title = "";
    }
    else if( titleConfig.asString().isEmpty() ) {
      step.title = " ";
    }
    else {
      step.title = titleConfig.asString();
    }

    step.time = stepConfig["time"].get( 0.0f );

    const JSON& stateConfig = stepConfig["state"];

    if( stateConfig.isNull() ) {
      step.endState = Camera::CINEMATIC;
    }
    else {
      const char* sEndState = stateConfig.get( "CINEMATIC" );

      if( String::equals( sEndState, "CINEMATIC" ) ) {
        step.endState = Camera::CINEMATIC;
      }
      else if( String::equals( sEndState, "STRATEGIC" ) ) {
        step.endState = Camera::STRATEGIC;
      }
      else if( String::equals( sEndState, "UNIT" ) ) {
        step.endState = Camera::UNIT;
      }
      else {
        throw Exception( "Invalid state '%s' for sequence step; should be either CINEMATIC, "
                         "STRATEGIC or UNIT.", sEndState );
      }
    }

    steps.add( step );
  }
}

void CinematicProxy::begin()
{
  ui::ui.galileoFrame->enable( false );
  ui::ui.musicPlayer->enable( false );

  cinematicText = new ui::CinematicText();
  ui::ui.root->add( cinematicText, ui::Area::CENTRE, 200 );

  ui::mouse.doShow = false;

  beginRot    = camera.rot;
  beginPos    = camera.p;
  beginColour = camera.colour;

  stepTime    = 0.0f;

  cinematicText->set( title.substring( 0, nTitleChars ) );
}

void CinematicProxy::end()
{
  ui::ui.root->remove( cinematicText );

  ui::ui.musicPlayer->enable( true );
  ui::ui.galileoFrame->enable( true );

  title       = "";
  nTitleChars = 0;

  steps.clear();
  steps.dealloc();
}

void CinematicProxy::prepare()
{}

void CinematicProxy::update()
{
  if( steps.isEmpty() ) {
    camera.setState( Camera::State( prevState ) );
    return;
  }

  const Step& step = steps.first();

  if( step.endState != Camera::CINEMATIC ) {
    camera.setState( Camera::State( step.endState ) );
    return;
  }

  float t = step.time == 0.0f ? 1.0f : min( stepTime / step.time, 1.0f );

  camera.smoothRotateTo( Quat::slerp( beginRot, step.rot, t ) );
  camera.smoothMoveTo( Math::mix( beginPos, step.p, t ) );
  camera.colour = Math::mix( beginColour, step.colour, t );
  camera.align();

  if( nTitleChars < title.length() ) {
    nTitleChars = min( nTitleChars + int( timer.frameTicks ), title.length() );

    cinematicText->set( title.substring( 0, nTitleChars ) );
  }

  if( t == 1.0f ) {
    beginPos    = step.p;
    beginRot    = step.rot;
    beginColour = step.colour;

    stepTime    = 0.0f;

    steps.popFirst();

    if( step.track == -2 ) {
      sound.stopMusic();
    }
    else if( step.track != -1 ) {
      sound.playMusic( step.track );
    }

    if( !step.title.isEmpty() ) {
      title       = step.title;
      nTitleChars = 0;

      cinematicText->set( " " );
    }
  }
  else {
    stepTime += Timer::TICK_TIME;
  }
}

void CinematicProxy::reset()
{
  beginRot    = Quat::ID;
  beginPos    = Point::ORIGIN;
  beginColour = Mat44::ID;

  title       = "";
  nTitleChars = 0;

  stepTime    = 0.0f;

  camera.colour = camera.baseColour;

  steps.clear();
  steps.dealloc();
}

void CinematicProxy::read( InputStream* istream )
{
  beginRot    = istream->readQuat();
  beginPos    = istream->readPoint();
  beginColour = istream->readMat44();

  title       = istream->readString();
  nTitleChars = istream->readInt();

  stepTime    = istream->readFloat();
  prevState   = istream->readInt();

  int nSteps = istream->readInt();
  for( int i = 0; i < nSteps; ++i ) {
    Step step;

    step.rot      = istream->readQuat();
    step.p        = istream->readPoint();
    step.colour   = istream->readMat44();

    step.track    = istream->readInt();
    step.title    = istream->readString();

    step.time     = istream->readFloat();
    step.endState = istream->readInt();

    steps.add( step );
  }
}

void CinematicProxy::write( BufferStream* ostream ) const
{
  ostream->writeQuat( beginRot );
  ostream->writePoint( beginPos );
  ostream->writeMat44( beginColour );

  ostream->writeString( title );
  ostream->writeInt( nTitleChars );

  ostream->writeFloat( stepTime );
  ostream->writeInt( prevState );

  ostream->writeInt( steps.length() );
  for( int i = 0; i < steps.length(); ++i ) {
    const Step& step = steps[i];

    ostream->writeQuat( step.rot );
    ostream->writePoint( step.p );
    ostream->writeMat44( step.colour );

    ostream->writeInt( step.track );
    ostream->writeString( step.title );

    ostream->writeFloat( step.time );
    ostream->writeInt( step.endState );
  }
}

}
}
