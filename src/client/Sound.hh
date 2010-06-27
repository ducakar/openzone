/*
 *  Sound.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/World.hh"
#include "matrix/Translator.hh"
#include "client/Audio.hh"
#include "client/Context.hh"

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
      struct Source
      {
        uint    source;
        Source* next[1];

        Source( uint sourceId ) : source( sourceId ) {}

        static Pool<Source> pool;

        OZ_STATIC_POOL_ALLOC( pool )
      };

      struct ContSource
      {
        uint source;
        bool isUpdated;

        ContSource( uint sourceId ) : source( sourceId ), isUpdated( true ) {}
      };

      /*
       * SFX
       */
      ContSource* cachedSource;
      List<Source> sources;
      HashIndex<ContSource, 512> contSources;

      int  sourceClearCount;
      int  fullClearCount;

      HashIndex<Audio*, 2039> audios;

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
        cachedSource = contSources.add( key, sourceId );
      }

      uint getCachedContSourceId() const
      {
        return cachedSource->source;
      }

      bool updateContSource( uint key )
      {
        cachedSource = contSources.find( key );
        if( cachedSource != null ) {
          cachedSource->isUpdated = true;
          return true;
        }
        else {
          return false;
        }
      }

      void playAudio( const Object* obj, const Audio* parent )
      {
        Audio* const* value = audios.find( obj->index );
        if( value == null ) {
          value = audios.add( obj->index, context.createAudio( &*obj ) );
        }
        Audio* audio = *value;

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
