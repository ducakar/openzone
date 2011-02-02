/*
 *  Sound.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Translator.hpp"
#include "client/Audio.hpp"
#include "client/Context.hpp"

#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

namespace oz
{
namespace client
{

  class Sound
  {
    private:

      // only play sounds within 100 m range
      static const float DMAX;
      static const float DMAX_SQ;

      static const int MUSIC_BUFFER_SIZE = 16 * 1024;
      // stopped sources clear interval -- 4 s for 50 fps
      static const int SOURCES_CLEAR_INTERVAL = 4 * 1000;
      // cleanup interval (remove unused audios, unload unused sounds) -- 5 min for 50 fps
      static const int FULL_CLEAR_INTERVAL = 291 * 1000;

      /*
       * SFX
       */
      int  sourceClearCount;
      int  fullClearCount;

      void playCell( int cellX, int cellY );

      void loadMusicBuffer( uint buffer );
      void updateMusic();

      /*
       * Music
       */
      OggVorbis_File             oggStream;
      vorbis_info*               vorbisInfo;

      uint                       musicBuffers[2];
      uint                       musicSource;
      ALenum                     musicFormat;

      bool                       isMusicPlaying;
      bool                       isMusicLoaded;

    public:

      void setVolume( float volume )
      {
        alListenerf( AL_GAIN, volume );
      }

      void setMusicVolume( float volume )
      {
        alSourcef( musicSource, AL_GAIN, volume );
      }

      bool loadMusic( const char* path );
      void unloadMusic();

      void sync();
      void play();
      void update();

      bool init( int* argc, char** argv );
      void free();

  };

  extern Sound sound;

}
}
