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

      static const int MUSIC_BUFFER_SIZE = 32 * 1024;

      /*
       * SFX
       */
      void playCell( int cellX, int cellY );
      void loadMusicBuffer( uint buffer );

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

      void setVolume( float volume );
      void setMusicVolume( float volume );
      bool loadMusic( const char* path );
      void unloadMusic();

      void play();
      void update();

      bool init( int* argc, char** argv );
      void free();

  };

  extern Sound sound;

}
}
