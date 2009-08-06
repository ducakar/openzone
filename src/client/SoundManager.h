/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Audio.h"
#include "matrix/World.h"
#include "matrix/Translator.h"

#include <vorbis/vorbisfile.h>

namespace oz
{
namespace client
{

  class SoundManager
  {
    private:

      // only play sounds within 100 m range
      static const float DMAX;
      static const float DMAX_SQ;
      // release continous sound if not used for 60 s
      static const int RELEASE_COUNT = 3000;

      static const int MUSIC_BUFFER_SIZE = 16 * 1024;
      // cleanup interval (remove stopped sources, unused audios)
      static const int CLEAR_INTERVAL = 60 * 50;

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
      int clearCount;

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

      void playSector( int sectorX, int sectorY );

      void loadMusicBuffer( uint buffer );
      void updateMusic();
      void freeMusic();

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
      void sync();
      void play();
      void update();

      bool init( int *argc, char *argv[] );
      void free();

  };

  extern SoundManager soundManager;

}
}
