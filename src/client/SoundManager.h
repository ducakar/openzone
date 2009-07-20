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

      // size of hashtable for continous sources
      static const int HASHTABLE_SIZE = 256;
      static const int MUSIC_BUFFER_SIZE = 16 * 1024;
      // clear stopped sources each second
      static const int CLEAR_INTERVAL = 1000;

      /*
       * SFX
       */
      struct Source
      {
        Source *prev[1];
        Source *next[1];

        ALuint source;

        Source( ALuint sourceId ) : source( sourceId ) {}
      };

      struct ContSource
      {
        enum State
        {
          NOT_UPDATED,
          UPDATED
        };

        State  state;
        ALuint source;

        ContSource() {}
        ContSource( ALuint sourceId ) : state( UPDATED ), source( sourceId ) {}
      };

    private:

      /*
       * SFX
       */
      HashIndex<Audio*, HASHTABLE_SIZE>     audios;
      DList<Source, 0>                      sources;
      HashIndex<ContSource, HASHTABLE_SIZE> contSources;
      int clearCount;

      /*
       * Music
       */
      OggVorbis_File oggStream;
      vorbis_info    *vorbisInfo;

      bool           isMusicPlaying;
      bool           isMusicLoaded;

      ALuint         musicBuffers[2];
      ALuint         musicSource;
      ALenum         musicFormat;

      void playSector( int sectorX, int sectorY );

      void loadMusicBuffer( ALuint buffer );
      void updateMusic();
      void freeMusic();

    public:

      void addSource( ALuint sourceId )
      {
        sources << new Source( sourceId );
      }

      void addContSource( uint key, ALuint sourceId  )
      {
        contSources.add( key, sourceId );
      }

      ALuint getCachedContSourceId() const
      {
        return contSources.cachedValue().source;
      }

      bool updateContSource( uint key )
      {
        if( contSources.contains( key ) ) {
          contSources.cachedValue().state = ContSource::UPDATED;
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
      void update();

      void init();
      void free();

  };

  extern SoundManager soundManager;

}
}
