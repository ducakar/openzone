/*
 *  Sound.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Sound.h"

#include "matrix/Collider.h"
#include "Camera.h"

#include "BasicAudio.h"
#include "BotAudio.h"

namespace oz
{
namespace client
{

  Sound sound;

  const float Sound::DMAX = 100.0f;
  const float Sound::DMAX_SQ = DMAX * DMAX;

  Pool<Sound::Source> Sound::Source::pool;

  void Sound::playCell( int cellX, int cellY )
  {
    const Cell& cell = world.cells[cellX][cellY];

    foreach( obj, cell.objects.iterator() ) {
      if( obj->flags & Object::AUDIO_BIT ) {
        if( ( camera.p - obj->p ).sqL() < DMAX_SQ ) {
          playAudio( obj, null );
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

  bool Sound::loadMusic( const char* path )
  {
    log.print( "Loading music '%s' ...", path );

    FILE* oggFile = fopen( path, "rb" );

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

  void Sound::unloadMusic()
  {
    if( isMusicLoaded ) {
      alSourceStop( musicSource );
      alDeleteSources( 1, &musicSource );
      alDeleteBuffers( 2, musicBuffers );

      ov_clear( &oggStream );

      isMusicLoaded = false;
    }
  }

  void Sound::sync()
  {
    // remove Audio objects of removed objects
    for( typeof( audios.iterator() ) i = audios.iterator(); !i.isPassed(); ) {
      Audio* audio = i.value();
      uint key = i.key();
      ++i;

      if( world.objects[key] == null ) {
        delete audio;
        audios.remove( key );
      }
    }
  }

  void Sound::play()
  {
    // add new sounds
    alListenerfv( AL_ORIENTATION, camera.at );
    alListenerfv( AL_POSITION, camera.p );

    Span span;
    world.getInters( span, camera.p, DMAX + AABB::MAX_DIM );

    for( int x = span.minX ; x <= span.maxX; x++ ) {
      for( int y = span.minY; y <= span.maxY; y++ ) {
        playCell( x, y );
      }
    }
    assert( alGetError() == AL_NO_ERROR );
  }

  void Sound::update()
  {
    assert( alGetError() == AL_NO_ERROR );

    // remove continous sounds that are not played any more
    for( typeof( contSources.iterator() ) i = contSources.iterator(); !i.isPassed(); ) {
      ContSource* src = i;
      uint key = i.key();

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

    if( sourceClearCount >= SOURCES_CLEAR_INTERVAL ) {
      // remove stopped sources of non-continous sounds
      Source* prev = null;
      Source* src  = sources.first();

      while( src != null ) {
        Source* next = src->next[0];

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
      sourceClearCount = 0;
    }
    else {
      sourceClearCount += timer.frameMillis;
    }

    assert( alGetError() == AL_NO_ERROR );

    updateMusic();

    // cleanups
    if( fullClearCount >= FULL_CLEAR_INTERVAL ) {
      assert( alGetError() == AL_NO_ERROR );

      // remove Audio objects that are not used any more
      for( typeof( audios.iterator() ) i = audios.iterator(); !i.isPassed(); ) {
        Audio* audio = *i;
        uint key = i.key();

        // we should advance now, so that we don't remove the element the iterator is pointing at
        ++i;

        if( audio->flags & Audio::UPDATED_BIT ) {
          audio->flags &= ~Audio::UPDATED_BIT ;
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
      fullClearCount = 0;
    }
    else {
      fullClearCount += timer.frameMillis;
    }

    assert( alGetError() == AL_NO_ERROR );
  }

  bool Sound::init( int* argc, char** argv )
  {
    log.println( "Initializing SoundManager {" );
    log.indent();

    alutInit( argc, argv );
    if( alutGetError() != ALUT_ERROR_NO_ERROR ) {
      log.println( "Failed to initialize ALUT" );
      log.unindent();
      log.println( "}" );
      return false;
    }

    assert( alGetError() == AL_NO_ERROR );

    String sExtensions = reinterpret_cast<const char*>( alGetString( AL_EXTENSIONS ) );
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

    sourceClearCount = 0;
    fullClearCount   = 0;

    isMusicLoaded  = false;
    isMusicPlaying = false;

    alGenSources( 1, &musicSource );
    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );
    alSourcei( musicSource, AL_ROLLOFF_FACTOR, 0 );

    setVolume( config.getSet( "sound.volume.effects", 1.0f ) );
    setMusicVolume( config.getSet( "sound.volume.music", 1.0f ) );

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
    Source::pool.free();

    foreach( i, contSources.iterator() ) {
      const ContSource& src = *static_cast<const ContSource*>( i );

      alSourceStop( src.source );
      alDeleteSources( 1, &src.source );
      assert( alGetError() == AL_NO_ERROR );
    }
    contSources.clear();
    contSources.deallocate();

    audios.free();
    audios.deallocate();
    BasicAudio::pool.free();
    BotAudio::pool.free();
    assert( alGetError() == AL_NO_ERROR );

    unloadMusic();
    assert( alGetError() == AL_NO_ERROR );

    alutExit();
    log.printEnd( " OK" );
  }

}
}
