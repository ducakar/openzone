/*
 *  MusicPlayer.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/MusicPlayer.hpp"

#include "matrix/Translator.hpp"

#include "client/Sound.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  void MusicPlayer::prevSong( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nSongs = translator.musics.length();

    if( nSongs > 1 ) {
      musicPlayer->currentSong = ( nSongs + musicPlayer->currentSong - 1 ) % nSongs;

      sound.stopMusic();
      sound.loadMusic( translator.musics[musicPlayer->currentSong].path );

      musicPlayer->title = translator.musics[musicPlayer->currentSong].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::nextSong( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nSongs = translator.musics.length();

    if( nSongs > 1 ) {
      musicPlayer->currentSong = ( musicPlayer->currentSong + 1 ) % nSongs;

      sound.stopMusic();
      sound.loadMusic( translator.musics[musicPlayer->currentSong].path );

      musicPlayer->title = translator.musics[musicPlayer->currentSong].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::playSong( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nSongs = translator.musics.length();

    if( nSongs != 0 ) {
      sound.stopMusic();
      sound.loadMusic( translator.musics[musicPlayer->currentSong].path );

      musicPlayer->title = translator.musics[musicPlayer->currentSong].name;
      musicPlayer->isPlaying = true;
    }
  }

  void MusicPlayer::stopSong( Button* sender )
  {
    MusicPlayer* musicPlayer = static_cast<MusicPlayer*>( sender->parent );
    int nSongs = translator.musics.length();

    if( nSongs != 0 ) {
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
    if( isPlaying && !sound.isMusicPlaying ) {
      int nSongs = translator.musics.length();

      if( nSongs > 0 ) {
        currentSong = ( currentSong + 1 ) % nSongs;

        sound.stopMusic();
        sound.loadMusic( translator.musics[currentSong].path );

        title = translator.musics[currentSong].name;
      }
    }
  }

  void MusicPlayer::onDraw()
  {
    Frame::onDraw();

    setFont( Font::SMALL );
    print( width / 2, 32, ALIGN_HCENTRE, title );
  }

  MusicPlayer::MusicPlayer() :
      Frame( -8, -289, 240, 36 + Font::INFOS[Font::SMALL].height, gettext( "Music Player" ) ),
      currentSong( 0 ), title( "" ), isPlaying( false )
  {
    flags = UPDATE_BIT;

    volume = int( config.get( "sound.volume.music", 0.40f ) * 10.0f + 0.5f );
    volume = clamp( volume, 0, 10 );

    add( new Button( "−", volumeDown, 20, 20 ), 4, 4 );
    add( new Button( "◁", prevSong, 30, 20 ), 54, 4 );
    add( new Button( "▶", playSong, 30, 20 ), 88, 4 );
    add( new Button( "◼", stopSong, 30, 20 ), 122, 4 );
    add( new Button( "▷", nextSong, 30, 20 ), 156, 4 );
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
