/*
 *  Sound.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/World.h"
#include "matrix/Translator.h"
#include "Audio.h"
#include "Context.h"

#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

namespace oz
{
namespace client
{

  struct Sound
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
      struct Source
      {
        uint    source;
        Source* next[1];

        Source( uint sourceId ) : source( sourceId ) {}

        static Pool<Source> pool;

        OZ_STATIC_POOL_ALLOC( pool );
      };

      struct ContSource
      {
        uint source;
        bint isUpdated;

        ContSource() {}
        ContSource( uint sourceId ) : source( sourceId ), isUpdated( true ) {}
      };

      /*
       * SFX
       */
      List<Source> sources;
      HashIndex<ContSource, 512> contSources;

      int  sourceClearCount;
      int  fullClearCount;

      HashIndex<Audio*, 1021> audios;

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

      void playAudio( const Object* obj, const Audio* parent )
      {
        if( !audios.contains( obj->index ) ) {
          audios.add( obj->index, context.createAudio( &*obj ) );
        }
        Audio* audio = audios.cachedValue();

        audio->flags |= Audio::UPDATED_BIT;
        audio->play( parent );
      }

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
