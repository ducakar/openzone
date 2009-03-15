/*
 *  SoundManager.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/World.hpp"
#include "matrix/Effect.hpp"

#include <vorbis/vorbisfile.h>

namespace oz
{
namespace Client
{

  class SoundManager
  {
    private:

      // only play sounds within 100 m range
      static const float DMAX;
      static const float DMAX_SQ;
      // release continous sound if not used for 60 s
      static const int RELEASE_COUNT = 3000;

      struct Source : ReuseAlloc<Source>
      {
        Source *prev[1];
        Source *next[1];

        ALuint source;
      };

      struct ContSource : ReuseAlloc<ContSource>
      {
        enum State
        {
          NOT_UPDATED,
          UPDATED
        };

        State  state;
        ALuint source;
      };

      // size of hashtable for continous sources
      static const int HASHTABLE_SIZE = 256;
      static const int MUSIC_BUFFER_SIZE = 64 * 1024;
      // clear stopped sources each second
      static const int CLEAR_INTERVAL = 1000;

      // FIXME MAX_BUFFERS
      static const int MAX_BUFFERS = /*SND_MAX (from Translator)*/ 1024;

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

  extern SoundManager soundManager;

}
}
