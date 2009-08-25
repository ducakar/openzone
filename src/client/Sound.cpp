/*
 *  Sound.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Sound.h"

#include "Camera.h"
#include "Context.h"

namespace oz
{
namespace client
{

  Sound sound;

  const float Sound::DMAX = 100.0f;
  const float Sound::DMAX_SQ = DMAX * DMAX;

  void Sound::playSector( int sectorX, int sectorY )
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

  void Sound::loadMusicBuffer( uint buffer )
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

  void Sound::updateMusic()
  {
    assert( alGetError() == AL_NO_ERROR );

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

  void Sound::freeMusic()
  {
    if( isMusicLoaded ) {
      alSourceStop( musicSource );
      alDeleteSources( 1, &musicSource );
      alDeleteBuffers( 2, musicBuffers );

      ov_clear( &oggStream );

      isMusicLoaded = false;
    }
  }

  bool Sound::loadMusic( const char *path )
  {
    log.print( "Loading music '%s' ...", path );

    FILE *oggFile = fopen( path, "rb" );

    if( oggFile == null ) {
      log.printEnd( " Failed to open file" );
      return false;
    }
    if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
      fclose( oggFile );
      log.printEnd( " Failed to open Ogg stream" );
      return false;
    }

    isMusicLoaded = true;

    vorbisInfo = ov_info( &oggStream, -1 );
    if( vorbisInfo == null ) {
      ov_clear( &oggStream );
      log.printEnd( " Failed to read Vorbis header" );
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
      log.printEnd( " Invalid number of channels, should be 1 or 2" );
      return AL_NONE;
    }

    isMusicPlaying = true;

    alGenBuffers( 2, musicBuffers );
    loadMusicBuffer( musicBuffers[0] );
    loadMusicBuffer( musicBuffers[1] );
    alSourceQueueBuffers( musicSource, 2, &musicBuffers[0] );
    alSourcePlay( musicSource );

    assert( alGetError() == AL_NO_ERROR );

    log.printEnd( " OK" );
    return true;
  }

  void Sound::sync()
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

  void Sound::play()
  {
    // add new sounds
    alListenerfv( AL_ORIENTATION, camera.at );
    alListenerfv( AL_POSITION, camera.p );

    world.getInters( camera.p, DMAX + AABB::MAX_DIM );

    for( int x = world.minX ; x <= world.maxX; x++ ) {
      for( int y = world.minY; y <= world.maxY; y++ ) {
        playSector( x, y );
      }
    }
    assert( alGetError() == AL_NO_ERROR );

    if( clearCount % SOURCES_CLEAR_INTERVAL == 0 ) {
      doSourceClean = true;
    }
    if( clearCount >= FULL_CLEAR_INTERVAL ) {
      doFullClean = true;
      clearCount = 0;
    }
    clearCount++;
  }

  void Sound::update()
  {
    assert( alGetError() == AL_NO_ERROR );

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

    assert( alGetError() == AL_NO_ERROR );

    if( doSourceClean ) {
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
      doSourceClean = false;
    }

    assert( alGetError() == AL_NO_ERROR );

    updateMusic();

    // cleanups
    if( doFullClean ) {
      assert( alGetError() == AL_NO_ERROR );

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

      assert( alGetError() == AL_NO_ERROR );

      // remove unused (no object audio uses it) buffers
      for( int i = 0; i < translator.sounds.length(); i++ ) {
        // first, only
        if( context.sounds[i].nUsers == 0 ) {
          context.sounds[i].nUsers = -2;
        }
        else if( context.sounds[i].nUsers == -2 ) {
          context.freeSound( i );
        }
      }
      doFullClean = false;
    }

    assert( alGetError() == AL_NO_ERROR );
  }

  bool Sound::init( int *argc, char *argv[] )
  {
    log.println( "Initializing SoundManager {" );
    log.indent();

    alutInit( argc, argv );
    if( alutGetError() != ALUT_ERROR_NO_ERROR ) {
      log.printEnd( "Failed to initialize ALUT" );
      log.unindent();
      log.println( "}" );
      return false;
    }

    assert( alGetError() == AL_NO_ERROR );

    String sExtensions = (const char*) alGetString( AL_EXTENSIONS );
    Vector<String> extensions = sExtensions.trim().split( ' ' );

    log.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
    log.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
    log.println( "OpenAL version: %s", alGetString( AL_VERSION ) );
    log.println( "OpenAL extensions {" );
    log.indent();
    foreach( extension, extensions.iterator() ) {
      log.println( "%s", extension->cstr() );
    }
    log.unindent();
    log.println( "}" );

    log.println( "ALUT version: %d.%d", alutGetMajorVersion(), alutGetMinorVersion() );
    log.println( "ALUT suppored formats: %s", alutGetMIMETypes( ALUT_LOADER_BUFFER ) );

    clearCount     = 0;
    doSourceClean  = false;
    doFullClean    = false;

    isMusicLoaded  = false;
    isMusicPlaying = false;

    alGenSources( 1, &musicSource );
    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );
    alSourcei( musicSource, AL_ROLLOFF_FACTOR, 0 );

    setVolume( config.get( "sound.volume.effects", 1.0f ) );
    setMusicVolume( config.get( "sound.volume.music", 1.0f ) );

    log.unindent();
    log.println( "}" );

    assert( alGetError() == AL_NO_ERROR );

    return true;
  }

  void Sound::free()
  {
    assert( alGetError() == AL_NO_ERROR );

    log.print( "Shutting down SoundManager ..." );

    foreach( src, sources.iterator() ) {
      alSourceStop( src->source );
      alDeleteSources( 1, &src->source );
      assert( alGetError() == AL_NO_ERROR );
    }
    sources.free();

    foreach( i, contSources.iterator() ) {
      const ContSource &src = *(const ContSource*)i;

      alSourceStop( src.source );
      alDeleteSources( 1, &src.source );
      assert( alGetError() == AL_NO_ERROR );
    }
    contSources.clear();

    audios.free();
    assert( alGetError() == AL_NO_ERROR );

    freeMusic();
    assert( alGetError() == AL_NO_ERROR );

    alutExit();
    log.printEnd( " OK" );
  }

}
}
