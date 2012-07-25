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
 * @file client/ui/MusicPlayer.cc
 */

#include "stable.hh"

#include "client/ui/MusicPlayer.hh"

#include "client/Camera.hh"
#include "client/Sound.hh"

namespace oz
{
namespace client
{
namespace ui
{

void MusicPlayer::prevTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musicTracks.length();

  if( nTracks > 1 ) {
    musicPlayer->currentTrack = ( nTracks + musicPlayer->currentTrack - 1 ) % nTracks;

    sound.stopMusic();
    sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.set( "%s", library.musicTracks[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.set( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::nextTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musicTracks.length();

  if( nTracks > 1 ) {
    musicPlayer->currentTrack = ( musicPlayer->currentTrack + 1 ) % nTracks;

    sound.stopMusic();
    sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.set( "%s", library.musicTracks[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.set( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::playTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musicTracks.length();

  if( nTracks != 0 ) {
    sound.stopMusic();
    sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.set( "%s", library.musicTracks[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.set( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::stopTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musicTracks.length();

  if( nTracks != 0 ) {
    sound.stopMusic();

    musicPlayer->title.set( " " );
    musicPlayer->isPlaying = false;
  }
}

void MusicPlayer::volumeDown( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

  musicPlayer->volume = max( musicPlayer->volume - 1, 0 );
  musicPlayer->volumeLabel.set( "%.1f", float( musicPlayer->volume ) / 10.0f );
  sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
}

void MusicPlayer::volumeUp( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

  musicPlayer->volume = min( musicPlayer->volume + 1, 10 );
  musicPlayer->volumeLabel.set( "%.1f", float( musicPlayer->volume ) / 10.0f );
  sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
}

void MusicPlayer::onUpdate()
{
  const Bot* bot = camera.botObj;

  if( camera.state == Camera::UNIT && ( bot == null || ( bot->state & Bot::DEAD_BIT ) ||
        !bot->hasAttribute( ObjectClass::MUSIC_PLAYER_BIT ) ) )
  {
    if( isPlaying ) {
      isPlaying = false;
      title.set( " " );
      sound.stopMusic();
    }
    if( !( flags & HIDDEN_BIT ) ) {
      show( false );
    }
    return;
  }
  else {
    if( mouse.doShow && ( flags & HIDDEN_BIT ) ) {
      show( true );
    }
  }

  if( sound.getCurrentTrack() != currentTrack ) {
    int soundTrack = sound.getCurrentTrack();

    if( soundTrack == -1 ) {
      title.set( " " );
    }
    else {
      currentTrack = sound.getCurrentTrack();

      title.set( "%s", library.musicTracks[currentTrack].name.cstr() );
      trackLabel.set( "%d", currentTrack + 1 );
    }
  }

  if( isPlaying && !sound.isMusicPlaying() ) {
    int nTracks = library.musicTracks.length();

    if( nTracks > 0 ) {
      currentTrack = ( currentTrack + 1 ) % nTracks;

      sound.stopMusic();
      sound.playMusic( currentTrack );

      title.set( "%s", library.musicTracks[currentTrack].name.cstr() );
      trackLabel.set( "%d", currentTrack + 1 );
    }
  }
}

void MusicPlayer::onDraw()
{
  Frame::onDraw();

  title.draw( this, false );
  trackLabel.draw( this, true );
  volumeLabel.draw( this, true );
}

MusicPlayer::MusicPlayer() :
  Frame( 240, 36 + Font::INFOS[Font::SMALL].height, OZ_GETTEXT( "Music Player" ) ),
  title( width / 2, 32, ALIGN_HCENTRE, Font::SMALL, " " ),
  trackLabel( 39, 14, ALIGN_CENTRE, Font::SMALL, "0" ),
  volumeLabel( 201, 14, ALIGN_CENTRE, Font::SMALL, " " ),
  currentTrack( 0 ), volume( 5 ), isPlaying( false )
{
  flags = UPDATE_BIT;

  if( library.musicTracks.length() > 0 ) {
    trackLabel.set( "1" );
  }
  volumeLabel.set( "%.1f", float( volume ) / 10.0f );

  add( new Button( "−", volumeDown, 20, 20 ), 4, 4 );
  add( new Button( "<<", prevTrack, 30, 20 ), 54, 4 );
  add( new Button( ">", playTrack, 30, 20 ), 88, 4 );
  add( new Button( "x", stopTrack, 30, 20 ), 122, 4 );
  add( new Button( ">>", nextTrack, 30, 20 ), 156, 4 );
  add( new Button( "+", volumeUp, 20, 20 ), 216, 4 );
}

MusicPlayer::~MusicPlayer()
{
  sound.stopMusic();
}

}
}
}
