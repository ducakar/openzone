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
 * @file client/modules/MusicPlayer.cc
 */

#include "stable.hh"

#include "client/modules/MusicPlayer.hh"

#include "matrix/Library.hh"

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
  int nTracks = library.musics.length();

  if( nTracks > 1 ) {
    musicPlayer->currentTrack = ( nTracks + musicPlayer->currentTrack - 1 ) % nTracks;

    sound.stopMusic();
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.setText( "%s", library.musics[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::nextTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musics.length();

  if( nTracks > 1 ) {
    musicPlayer->currentTrack = ( musicPlayer->currentTrack + 1 ) % nTracks;

    sound.stopMusic();
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.setText( "%s", library.musics[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::playTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musics.length();

  if( nTracks != 0 ) {
    sound.stopMusic();
    sound.playMusic( musicPlayer->currentTrack );

    musicPlayer->title.setText( "%s", library.musics[musicPlayer->currentTrack].name.cstr() );
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack + 1 );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::stopTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musics.length();

  if( nTracks != 0 ) {
    sound.stopMusic();

    musicPlayer->title.setText( " " );
    musicPlayer->isPlaying = false;
  }
}

void MusicPlayer::volumeDown( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

  musicPlayer->volume = max( musicPlayer->volume - 1, 0 );
  musicPlayer->volumeLabel.setText( "%.1f", float( musicPlayer->volume ) / 10.0f );
  sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
}

void MusicPlayer::volumeUp( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

  musicPlayer->volume = min( musicPlayer->volume + 1, 10 );
  musicPlayer->volumeLabel.setText( "%.1f", float( musicPlayer->volume ) / 10.0f );
  sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
}

void MusicPlayer::onUpdate()
{
  if( camera.state == Camera::BOT && camera.botObj != null ) {
    const Vector<int>& items = camera.botObj->items;

    for( int i = 0; i < items.length(); ++i ) {
      const Object* item = orbis.objects[ items[i] ];

      if( item != null && item->clazz->name.equals( "musicPlayer" ) ) {
        goto musicPlayerEnabled;
      }
    }

    if( isPlaying ) {
      isPlaying = false;
      title.setText( " " );
      sound.stopMusic();
    }
    isVisible = false;
    return;
  }

musicPlayerEnabled:;

  isVisible = true;

  if( isPlaying && !sound.isMusicPlaying() ) {
    int nTracks = library.musics.length();

    if( nTracks > 0 ) {
      currentTrack = ( currentTrack + 1 ) % nTracks;

      sound.stopMusic();
      sound.playMusic( currentTrack );

      title.setText( "%s", library.musics[currentTrack].name.cstr() );
      trackLabel.setText( "%d", currentTrack + 1 );
    }
  }
}

bool MusicPlayer::onMouseEvent()
{
  if( !isVisible ) {
    return false;
  }
  else {
    return Frame::onMouseEvent();
  }
}

void MusicPlayer::onDraw()
{
  if( !isVisible ) {
    return;
  }

  Frame::onDraw();

  title.draw( this );
  trackLabel.draw( this );
  volumeLabel.draw( this );
}

MusicPlayer::MusicPlayer() :
  Frame( 8, -8, 240, 36 + Font::INFOS[Font::SMALL].height, OZ_GETTEXT( "Music Player" ) ),
  title( width / 2, 32, ALIGN_HCENTRE, Font::SMALL, " " ),
  trackLabel( 39, 14, ALIGN_CENTRE, Font::SMALL, "0" ),
  volumeLabel( 201, 14, ALIGN_CENTRE, Font::SMALL, " " ),
  currentTrack( 0 ), isPlaying( false ), isVisible( true )
{
  flags = UPDATE_BIT | PINNED_BIT;

  volume = int( config.get( "sound.volume.music", 0.50f ) * 10.0f + 0.5f );
  volume = clamp( volume, 0, 10 );

  if( library.musics.length() > 0 ) {
    trackLabel.setText( "1" );
  }
  volumeLabel.setText( "%.1f", float( volume ) / 10.0f );

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
