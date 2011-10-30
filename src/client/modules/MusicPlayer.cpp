/*
 *  MusicPlayer.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/MusicPlayer.hpp"

#include "matrix/Library.hpp"

#include "client/Camera.hpp"
#include "client/Sound.hpp"

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
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack );
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
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack );
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
    musicPlayer->trackLabel.setText( "%d", musicPlayer->currentTrack );
    musicPlayer->isPlaying = true;
  }
}

void MusicPlayer::stopTrack( Button* sender )
{
  MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
  int nTracks = library.musics.length();

  if( nTracks != 0 ) {
    sound.stopMusic();

    musicPlayer->title.setText( "" );
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
      title.setText( "" );
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
      trackLabel.setText( "%d", currentTrack );
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
    Frame( 8, -8, 240, 36 + Font::INFOS[Font::SMALL].height, gettext( "Music Player" ) ),
    title( width / 2, 32, ALIGN_HCENTRE, Font::SMALL, "" ),
    trackLabel( 39, 14, ALIGN_CENTRE, Font::SMALL, "0" ),
    volumeLabel( 201, 14, ALIGN_CENTRE, Font::SMALL, "" ),
    currentTrack( 0 ), isPlaying( false ), isVisible( true )
{
  flags = UPDATE_BIT | PINNED_BIT;

  volume = int( config.get( "sound.volume.music", 0.50f ) * 10.0f + 0.5f );
  volume = clamp( volume, 0, 10 );

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
