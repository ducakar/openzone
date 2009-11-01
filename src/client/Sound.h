/*
 *  Sound.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Audio.h"
#include "matrix/World.h"
#include "matrix/Translator.h"

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
      struct Source : PoolAlloc<Source, 0>
      {
        uint   source;
        Source *next[1];

        Source( uint sourceId ) : source( sourceId ) {}
      };

      struct ContSource
      {
        uint source;
        bool isUpdated;

        ContSource() {}
        ContSource( uint sourceId ) : source( sourceId ), isUpdated( true ) {}
      };

    private:

      /*
       * SFX
       */
      List<Source, 0>            sources;
      HashIndex<ContSource, 251> contSources;

      int  sourceClearCount;
      int  fullClearCount;

      /*
       * Music
       */
      OggVorbis_File             oggStream;
      vorbis_info                *vorbisInfo;

      bool                       isMusicPlaying;
      bool                       isMusicLoaded;

      uint                       musicBuffers[2];
      uint                       musicSource;
      ALenum                     musicFormat;

      /*
       * Common
       */

      HashIndex<Audio*, 1021>    audios;

      void playCell( int cellX, int cellY );

      void loadMusicBuffer( uint buffer );
      void updateMusic();

    public:

      void addSource( uint sourceId )
      {
        sources << new Source( sourceId );
      }

      void addContSource( uint key, uint sourceId  )
      {
        contSources.add( key, sourceId );
      }

      uint getCachedContSourceId() const
      {
        return contSources.cachedValue().source;
      }

      bool updateContSource( uint key )
      {
        if( contSources.contains( key ) ) {
          contSources.cachedValue().isUpdated = true;
          return true;
        }
        else {
          return false;
        }
      }

      void setVolume( float volume )
      {
        alListenerf( AL_GAIN, volume );
      }

      void setMusicVolume( float volume )
      {
        alSourcef( musicSource, AL_GAIN, volume );
      }

      bool loadMusic( const char *path );
      void unloadMusic();

      void sync();
      void play();
      void update();

      bool init( int *argc, char *argv[] );
      void free();

  };

  extern Sound sound;

}
}
