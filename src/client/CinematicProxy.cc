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
#include "client/NaCl.hh"

#include "client/ui/GalileoFrame.hh"
#include "client/ui/MusicPlayer.hh"
#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

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

void CinematicProxy::executeSequence( const char* path, const Lingua* missionLingua )
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
      float rot[3] = { 0.0f, 0.0f, 0.0f };

      rotArray.get( rot, 3 );
      step.rot = Quat::rotationZXZ( Math::rad( rot[0] ), Math::rad( rot[1] ), Math::rad( rot[2] ) );
    }

    const JSON& posArray = stepConfig["pos"];

    if( !posArray.isNull() ) {
      Point p = Point( 0.0f, 0.0f, 0.0f );

      posArray.get( p, 3 );
      step.p = p;
    }

    const JSON& colourArray = stepConfig["colour"];

    if( !colourArray.isNull() ) {
      Mat44 m;

      colourArray.get( m, 16 );
      step.colour = m;
    }

    const JSON& trackConfig = stepConfig["track"];
    if( trackConfig.isNull() ) {
      step.track = -1;
    }
    else if( trackConfig.asString().isEmpty() ) {
      step.track = -2;
    }
    else {
      step.track = library.musicTrackIndex( trackConfig.asString() );
    }

    const JSON& titleConfig = stepConfig["title"];
    if( titleConfig.isNull() ) {
      step.title = "";
    }
    else if( titleConfig.asString().isEmpty() ) {
      step.title = " ";
    }
    else {
      step.title = missionLingua->get( titleConfig.asString() );
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
        throw Exception( "Invalid state '%s' for sequence step; should be either CINEMATIC,"
                         " STRATEGIC or UNIT.", sEndState );
      }
    }

    steps.add( step );
  }

  sequence.clear( true );
}

void CinematicProxy::begin()
{
  ui::ui.galileoFrame->enable( false );
  ui::ui.musicPlayer->enable( false );

  OZ_MAIN_CALL( this, {
    _this->cinematicText = new ui::CinematicText();
    ui::ui.root->add( _this->cinematicText, ui::Area::CENTRE, 200 );
  } )

  ui::mouse.doShow = false;

  beginRot    = camera.rot;
  beginPos    = camera.p;
  beginColour = camera.colour;

  stepTime    = 0.0f;

  cinematicText->set( title.substring( 0, nTitleChars ) );
}

void CinematicProxy::end()
{
  OZ_MAIN_CALL( this, {
    ui::ui.root->remove( _this->cinematicText );
  } )

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

    // Take all bytes of UTF-8 characters.
    while( nTitleChars > 0 && nTitleChars < title.length() &&
           ( title[nTitleChars - 1] & title[nTitleChars] & 0x80 ) )
    {
      ++nTitleChars;
    }

    cinematicText->set( title.substring( 0, nTitleChars ) );
  }

  stepTime += Timer::TICK_TIME;

  if( t == 1.0f ) {
    beginPos    = step.p;
    beginRot    = step.rot;
    beginColour = step.colour;

    stepTime    = 0.0f;

    steps.popFirst();
  }
  else if( t == 0.0f ) {
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
