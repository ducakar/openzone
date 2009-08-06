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

  void SoundManager::playSector( int sectorX, int sectorY )
  {
    Sector &sector = world.sectors[sectorX][sectorY];

    foreach( obj, sector.objects.iterator() ) {
      if( obj->flags & Object::AUDIO_BIT ) {
        if( ( camera.p - obj->p ).sqL() < DMAX_SQ ) {
          if( !audios.contains( obj->index ) ) {
            audios.add( obj->index, context.createAudio( &*obj ) );
          }
          audios.cachedValue()->update();
          audios.cachedValue()->isUpdated = true;
        }
      }
    }
  }

  void SoundManager::loadMusicBuffer( uint buffer )
  {
    char data[MUSIC_BUFFER_SIZE];
    int  section;
    int  bytesRead = 0;
    int  result;

    do {
      result = ov_read( &oggStream, &data[bytesRead], MUSIC_BUFFER_SIZE - bytesRead, 0, 2, 1,
                         &section );
      bytesRead += result;
      if( result < 0 ) {
        isMusicPlaying = false;
        return;
      }
    }
    while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

    alBufferData( buffer, musicFormat, data, bytesRead, vorbisInfo->rate );
  }

  void SoundManager::updateMusic()
  {
    if( !isMusicPlaying ) {
      return;
    }

    int processed;
    alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &processed );

    while( processed > 0 ) {
      uint buffer;
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

  bool SoundManager::loadMusic( const char *path )
  {
    logFile.print( "Loading music '%s' ...", path );

    FILE *oggFile = fopen( path, "rb" );

    if( oggFile == null ) {
      logFile.printEnd( " Failed to open file" );
      return false;
    }
    if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
      fclose( oggFile );
      logFile.printEnd( " Failed to open Ogg stream" );
      return false;
    }

    isMusicLoaded = true;

    vorbisInfo = ov_info( &oggStream, -1 );
    if( vorbisInfo == null ) {
      ov_clear( &oggStream );
      logFile.printEnd( " Failed to read Vorbis header" );
      return false;
    }

    if( vorbisInfo->channels == 1 ) {
      musicFormat = AL_FORMAT_MONO16;
    }
    else if( vorbisInfo->channels == 2 ) {
      musicFormat = AL_FORMAT_STEREO16;
    }
    else {
      ov_clear( &oggStream );
      logFile.printEnd( " Invalid number of channels, should be 1 or 2" );
      return AL_NONE;
    }

    isMusicPlaying = true;

    alGenBuffers( 2, musicBuffers );
    loadMusicBuffer( musicBuffers[0] );
    loadMusicBuffer( musicBuffers[1] );
    alSourceQueueBuffers( musicSource, 2, &musicBuffers[0] );
    alSourcePlay( musicSource );

    logFile.printEnd( " OK" );
    return true;
  }

  void SoundManager::sync()
  {
    // remove Audio objects of removed objects
    foreach( i, synapse.cutObjects.iterator() ) {
      if( audios.contains( ( *i )->index ) ) {
        delete audios.cachedValue();
        audios.remove( ( *i )->index );
      }
    }
    foreach( i, synapse.removeObjects.iterator() ) {
      if( audios.contains( ( *i )->index ) ) {
        delete audios.cachedValue();
        audios.remove( ( *i )->index );
      }
    }
  }

  void SoundManager::play()
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
  }

  void SoundManager::update()
  {
    // remove continous sounds that are not played any more
    for( typeof( contSources.iterator() ) i( contSources ); !i.isPassed(); ) {
      ContSource *src = i;
      uint       key  = i.key();

      // we should advance now, so that we don't remove the element the iterator is pointing at
      ++i;

      if( src->isUpdated ) {
        src->isUpdated = false;
      }
      else {
        alSourceStop( src->source );
        alDeleteSources( 1, &src->source );
        contSources.remove( key );
      }
    }

    updateMusic();

    // cleanups
    if( clearCount >= CLEAR_INTERVAL ) {
      // remove Audio objects that are not used any more
      for( typeof( audios.iterator() ) i( audios ); !i.isPassed(); ) {
        Audio *audio = *i;
        uint  key    = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( audio->isUpdated ) {
          audio->isUpdated = false;
        }
        else {
          audios.remove( key );
          delete audio;
        }
      }

      // remove stopped sources of non-continous sounds
      Source *prev = null;
      Source *src  = sources.first();

      while( src != null ) {
        Source *next = src->next[0];

        ALint value;
        alGetSourcei( src->source, AL_SOURCE_STATE, &value );

        if( value != AL_PLAYING ) {
          alDeleteSources( 1, &src->source );

          sources.remove( src, prev );
          delete src;
        }
        else {
          prev = src;
        }
        src = next;
      }
      clearCount = 0;
    }
    clearCount += timer.frameMillis;
  }

  bool SoundManager::init( int *argc, char *argv[] )
  {
    logFile.println( "Initializing SoundManager {" );
    logFile.indent();

    alutInit( argc, argv );
    if( alutGetError() != ALUT_ERROR_NO_ERROR ) {
      logFile.printEnd( "Failed to initialize ALUT" );
      logFile.unindent();
      logFile.println( "}" );
      return false;
    }

    String sExtensions = (const char*) alGetString( AL_EXTENSIONS );
    Vector<String> extensions = sExtensions.trim().split( ' ' );

    logFile.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
    logFile.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
    logFile.println( "OpenAL version: %s", alGetString( AL_VERSION ) );
    logFile.println( "OpenAL extensions {" );
    logFile.indent();
    foreach( extension, extensions.iterator() ) {
      logFile.println( "%s", extension->cstr() );
    }
    logFile.unindent();
    logFile.println( "}" );

    logFile.println( "ALUT version: %d.%d", alutGetMajorVersion(), alutGetMinorVersion() );
    logFile.println( "ALUT suppored formats: %s", alutGetMIMETypes( ALUT_LOADER_BUFFER ) );

    clearCount = 0;

    isMusicLoaded = false;
    isMusicPlaying = false;

    alGenSources( 1, &musicSource );
    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );
    alSourcei( musicSource, AL_ROLLOFF_FACTOR, 0 );

    setVolume( config.get( "sound.volume.effects", 1.0f ) );
    setMusicVolume( config.get( "sound.music.effects", 1.0f ) );

    logFile.unindent();
    logFile.println( "}" );

    return true;
  }

  void SoundManager::free()
  {
    logFile.print( "Shutting down SoundManager ..." );

    foreach( src, sources.iterator() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
    }
    sources.free();

    foreach( i, contSources.iterator() ) {
      const ContSource &src = *(const ContSource*)i;

      alSourceStop( src.source );
      alDeleteSources( 1, &src.source );
    }
    contSources.clear();

    audios.free();

    freeMusic();

    alutExit();
    logFile.printEnd( " OK" );
  }

}
}
