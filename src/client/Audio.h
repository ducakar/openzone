/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/World.h"
#include "matrix/Translator.h"

#include <vorbis/vorbisfile.h>

namespace oz
{
namespace client
{

  class Audio
  {
    private:

      // only play sounds within 100 m range
      static const float DMAX;
      static const float DMAX_SQ;
      // release continous sound if not used for 60 s
      static const int RELEASE_COUNT = 3000;

      // size of hashtable for continous sources
      static const int HASHTABLE_SIZE = 256;
      static const int MUSIC_BUFFER_SIZE = 64 * 1024;
      // clear stopped sources each second
      static const int CLEAR_INTERVAL = 1000;

      // FIXME MAX_BUFFERS
      static const int MAX_BUFFERS = 1024;

      struct Source : PoolAlloc<Source, 0>
      {
        Source *prev[1];
        Source *next[1];

        ALuint source;
      };

      struct ContSource : PoolAlloc<ContSource, 0>
      {
        enum State
        {
          NOT_UPDATED,
          UPDATED
        };

        State  state;
        ALuint source;
      };

      /*
       * Sound effects
       */
      ALCdevice  *device;
      ALCcontext *context;

      ALuint     buffers[MAX_BUFFERS];

      DList<Source, 0>                      sources;
      HashIndex<ContSource, HASHTABLE_SIZE> contSources;

      int          clearCount;

      bool         load( int sample, const char *file );
      void         playSector( int sectorX, int sectorY );

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

      void loadMusicBuffer( ALuint buffer );
      void updateMusic();
      void freeMusic();

    public:

      bool init();
      void free();

      bool loadMusic( const char *file );
      void update();

      void setVolume( float volume );
      void setMusicVolume( float volume );

  };

  extern Audio audio;

}
}
