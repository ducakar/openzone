/*
 *  SoundManager.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "SoundManager.h"

#include "Camera.h"
#include "Context.h"

namespace oz
{
namespace client
{

  SoundManager soundManager;

  const float SoundManager::DMAX = 100.0f;
  const float SoundManager::DMAX_SQ = DMAX * DMAX;

  void SoundManager::init()
  {
    logFile.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
    logFile.println( "OpenAL version: %s", alGetString( AL_VERSION ) );
    logFile.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
    logFile.println( "OpenAL extensions: %s", alGetString( AL_EXTENSIONS ) );

    logFile.println( "ALUT version: %d.%d", alutGetMajorVersion(), alutGetMinorVersion() );
    logFile.println( "ALUT suppored formats: %s", alutGetMIMETypes( ALUT_LOADER_BUFFER ) );

    isMusicPlaying = false;

    alGenSources( 1, &musicSource );
    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );
    alSourcei( musicSource, AL_ROLLOFF_FACTOR, 0 );

    setVolume( config.get( "sound.volume.effects", 1.0f ) );
    setMusicVolume( config.get( "sound.music.effects", 1.0f ) );
  }

  void SoundManager::free()
  {
    // FIXME: pool deallocation issues, crashes because of assertions on exit
    foreach( src, sources.iterator() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
    }
    sources.free();

    foreach( i, contSources.iterator() ) {
      ContSource &src = *i;

      alSourceStop( src.source );
      alDeleteSources( 1, &src.source );
    }
    contSources.clear();
    contSources.deallocate();

    audios.clear();
    audios.deallocate();

    freeMusic();
  }

  void SoundManager::playSector( int sectorX, int sectorY )
  {
    Sector &sector = world.sectors[sectorX][sectorY];

    foreach( obj, sector.objects.iterator() ) {
      // TODO sound player
      if( ( camera.p - obj->p ).sqL() < DMAX_SQ ) {
        if( !audios.contains( (uint) &*obj ) ) {
          audios.add( (uint) &*obj, context.createAudio( &*obj ) );
        }
        audios.cachedValue()->update();
      }
    }
  }

  void SoundManager::update()
  {
    // add new sounds
    alListenerfv( AL_ORIENTATION, camera.at );
    alListenerfv( AL_POSITION, camera.p );

    world.getInters( camera.p, DMAX );

    for( int x = world.minSectX ; x <= world.maxSectX; x++ ) {
      for( int y = world.minSectY; y <= world.maxSectY; y++ ) {
        playSector( x, y );
      }
    }

    // remove continous sounds that are not played any more
    for( HashIndex<ContSource, HASHTABLE_SIZE>::Iterator i( contSources ); !i.isPassed(); ) {
      ContSource *src = i;
      uint key = i.key();

      // we should advance now, so that we don't remove the element the iterator is pointing at
      ++i;

      if( src->state == ContSource::NOT_UPDATED ) {
        alSourceStop( src->source );
        alDeleteSources( 1, &src->source );
        contSources.remove( key );
      }
      else {
        src->state = ContSource::NOT_UPDATED;
      }
    }

    // remove stopped sources of non-continous sounds
    if( clearCount >= CLEAR_INTERVAL ) {
      Source *src = sources.first();

      while( src != null ) {
        Source *next = src->next[0];
        ALint value = AL_STOPPED;

        alGetSourcei( src->source, AL_SOURCE_STATE, &value );

        if( value != AL_PLAYING ) {
          alDeleteSources( 1, &src->source );

          sources.remove( src );
          delete src;
        }
        src = next;
      }
      clearCount -= CLEAR_INTERVAL;
    }
    clearCount += timer.frameMillis;

    updateMusic();
  }

  bool SoundManager::loadMusic( const char *file )
  {
    logFile.print( "Loading music '%s' ...", file );

    FILE *oggFile = fopen( file, "rb" );

    if( oggFile == null ) {
      logFile.printEnd( " Failed" );
      return false;
    }
    if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
      fclose( oggFile );
      logFile.printEnd( " Failed" );
      return false;
    }

    isMusicLoaded = true;

    vorbisInfo = ov_info( &oggStream, -1 );
    musicFormat = vorbisInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    isMusicPlaying = true;

    alGenBuffers( 2, musicBuffers );
    loadMusicBuffer( musicBuffers[0] );
    loadMusicBuffer( musicBuffers[1] );
    alSourceQueueBuffers( musicSource, 2, &musicBuffers[0] );
    alSourcePlay( musicSource );

    logFile.printEnd( " OK" );
    return true;
  }

  void SoundManager::freeMusic()
  {
    if( isMusicLoaded ) {
      alSourceStop( musicSource );
      alDeleteSources( 1, &musicSource );
      alDeleteBuffers( 2, musicBuffers );

      ov_clear( &oggStream );

      isMusicLoaded = false;
    }
  }

  void SoundManager::loadMusicBuffer( ALuint buffer )
  {
    char data[MUSIC_BUFFER_SIZE];
    int  section;
    int  size = 0;
    int  result;

    do {
      result = ov_read( &oggStream, &data[size], MUSIC_BUFFER_SIZE - size, 0, 2, 1, &section );
      size += result;
      if( result < 0 ) {
        isMusicPlaying = false;
        return;
      }
    }
    while( result > 0 && size < MUSIC_BUFFER_SIZE );

    alBufferData( buffer, musicFormat, data, size, vorbisInfo->rate );
  }

  void SoundManager::updateMusic()
  {
    if( !isMusicPlaying ) {
      return;
    }

    int processed;
    alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &processed );

    while( processed > 0 ) {
      ALuint buffer;
      alSourceUnqueueBuffers( musicSource, 1, &buffer );
      loadMusicBuffer( buffer );
      alSourceQueueBuffers( musicSource, 1, &buffer );
      processed--;
    }

    ALint value;
    alGetSourcei( musicSource, AL_SOURCE_STATE, &value );

    if( value != AL_PLAYING ) {
      alSourcePlay( musicSource );
    }
  }

  void SoundManager::setVolume( float volume )
  {
    alListenerf( AL_GAIN, volume );
  }

  void SoundManager::setMusicVolume( float volume )
  {
    alSourcef( musicSource, AL_GAIN, volume );
  }

}
}
