/*
 *  MusicPlayer.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/MusicPlayer.hpp"

#include "matrix/Translator.hpp"

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
    int nTracks = translator.musics.length();

    if( nTracks > 1 ) {
      musicPlayer->currentTrack = ( nTracks + musicPlayer->currentTrack - 1 ) % nTracks;

      sound.stopMusic();
      sound.playMusic( musicPlayer->currentTrack );

      musicPlayer->title = translator.musics[musicPlayer->currentTrack].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::nextTrack( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nTracks = translator.musics.length();

    if( nTracks > 1 ) {
      musicPlayer->currentTrack = ( musicPlayer->currentTrack + 1 ) % nTracks;

      sound.stopMusic();
      sound.playMusic( musicPlayer->currentTrack );

      musicPlayer->title = translator.musics[musicPlayer->currentTrack].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::playTrack( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nTracks = translator.musics.length();

    if( nTracks != 0 ) {
      sound.stopMusic();
      sound.playMusic( musicPlayer->currentTrack );

      musicPlayer->title = translator.musics[musicPlayer->currentTrack].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::stopTrack( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nTracks = translator.musics.length();

    if( nTracks != 0 ) {
      sound.stopMusic();

      musicPlayer->title = "";
      musicPlayer->isPlaying = false;
    }
  }

  void MusicPlayer::volumeDown( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

    musicPlayer->volume = max( musicPlayer->volume - 1, 0 );
    sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
  }

  void MusicPlayer::volumeUp( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );

    musicPlayer->volume = min( musicPlayer->volume + 1, 10 );
    sound.setMusicVolume( float( musicPlayer->volume ) / 10.0f );
  }

  void MusicPlayer::onUpdate()
  {
    if( isPlaying && !sound.isMusicPlaying() ) {
      int nTracks = translator.musics.length();

      if( nTracks > 0 ) {
        currentTrack = ( currentTrack + 1 ) % nTracks;

        sound.stopMusic();
        sound.playMusic( currentTrack );

        title = translator.musics[currentTrack].name;
      }
    }
  }

  void MusicPlayer::onDraw()
  {
    Frame::onDraw();

    setFont( Font::SMALL );
    print( width / 2, 32, ALIGN_HCENTRE, "%s", title );
    print( 39, 14, ALIGN_CENTRE, "%d", currentTrack );
    print( 201, 14, ALIGN_CENTRE, "%.1f", float( volume ) / 10.0f );
  }

  MusicPlayer::MusicPlayer() :
      Frame( -8, -289, 240, 36 + Font::INFOS[Font::SMALL].height, OZ_GETTEXT( "Music Player" ) ),
      currentTrack( 0 ), title( "" ), isPlaying( false )
  {
    flags = UPDATE_BIT;

    volume = int( config.get( "sound.volume.music", 0.40f ) * 10.0f + 0.5f );
    volume = clamp( volume, 0, 10 );

    add( new Button( "−", volumeDown, 20, 20 ), 4, 4 );
    add( new Button( "◁", prevTrack, 30, 20 ), 54, 4 );
    add( new Button( "▶", playTrack, 30, 20 ), 88, 4 );
    add( new Button( "◼", stopTrack, 30, 20 ), 122, 4 );
    add( new Button( "▷", nextTrack, 30, 20 ), 156, 4 );
    add( new Button( "+", volumeUp, 20, 20 ), 216, 4 );

    foreach( child, children.iter() ) {
      child->setFont( Font::SYMBOL );
    }
  }

  MusicPlayer::~MusicPlayer()
  {
    sound.stopMusic();
  }

}
}
}
