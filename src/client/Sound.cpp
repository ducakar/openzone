/*
 *  Sound.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Sound.hpp"

#include "matrix/Collider.hpp"
#include "client/Camera.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"

namespace oz
{
namespace client
{

  Sound sound;

  const float Sound::DMAX = 100.0f;
  const float Sound::DMAX_SQ = DMAX * DMAX;

  void Sound::playCell( int cellX, int cellY )
  {
    const Cell& cell = orbis.cells[cellX][cellY];

    foreach( strIndex, cell.structs.citer() ) {
      if( !playedStructs.get( *strIndex ) ) {
        playedStructs.set( *strIndex );

        const Struct* str = orbis.structs[*strIndex];
        context.playBSP( str );
      }
    }
    foreach( obj, cell.objects.citer() ) {
      if( obj->flags & Object::AUDIO_BIT ) {
        if( ( camera.p - obj->p ).sqL() < DMAX_SQ ) {
          context.playAudio( obj, null );
        }
      }
    }
  }

  bool Sound::loadMusicBuffer( uint buffer )
  {
    char data[MUSIC_BUFFER_SIZE];
    int  section;
    int  bytesRead = 0;
    int  result;

    do {
      result = int( ov_read( &oggStream, &data[bytesRead], MUSIC_BUFFER_SIZE - bytesRead,
                             0, 2, 1, &section ) );
      bytesRead += result;
      if( result <= 0 ) {
        return false;
      }
    }
    while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

    alBufferData( buffer, musicFormat, data, bytesRead, ALsizei( vorbisInfo->rate ) );

    return true;
  }

  void Sound::setVolume( float volume )
  {
    alListenerf( AL_GAIN, volume );
  }

  void Sound::setMusicVolume( float volume )
  {
    alSourcef( musicSource, AL_GAIN, volume );
  }

  void Sound::playMusic( int track )
  {
    hard_assert( track >= 0 );

    selectedTrack = track;
    currentTrack  = track;
  }

  void Sound::stopMusic()
  {
    selectedTrack = -2;
    currentTrack  = -1;
  }

  bool Sound::isMusicPlaying() const
  {
    return currentTrack != -1;
  }

  void Sound::play()
  {
    Vec3 orientation[2] = { camera.at, camera.up };
    // add new sounds
    alListenerfv( AL_ORIENTATION, orientation[0] );
    alListenerfv( AL_POSITION, camera.p );

    if( playedStructs.length() < orbis.structs.length() ) {
      playedStructs.dealloc();
      playedStructs.alloc( orbis.structs.length() );
    }
    playedStructs.clearAll();

    Span span = orbis.getInters( camera.p, DMAX + AABB::MAX_DIM );

    for( int x = span.minX ; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        playCell( x, y );
      }
    }

    hard_assert( alGetError() == AL_NO_ERROR );
  }

  void Sound::update()
  {
    hard_assert( alGetError() == AL_NO_ERROR );

    if( selectedTrack != -1 ) {
      alSourceStop( musicSource );

      int nQueued;
      alGetSourcei( musicSource, AL_BUFFERS_QUEUED, &nQueued );

      uint buffer[2];
      alSourceUnqueueBuffers( musicSource, nQueued, buffer );

      ov_clear( &oggStream );

      hard_assert( alGetError() == AL_NO_ERROR );

      currentTrack = -1;

      if( selectedTrack == -2 ) {
        selectedTrack = -1;
      }
      else {
        const char* path = translator.musics[selectedTrack].path;

        log.print( "Loading music '%s' ...", path );

        FILE* oggFile = fopen( path, "rb" );

        if( oggFile == null ) {
          log.printEnd( " Failed to open file" );
          throw Exception( "Music loading failed" );
        }
        if( ov_open( oggFile, &oggStream, null, 0 ) < 0 ) {
          fclose( oggFile );
          log.printEnd( " Failed to open Ogg stream" );
          throw Exception( "Music loading failed" );
        }

        vorbisInfo = ov_info( &oggStream, -1 );
        if( vorbisInfo == null ) {
          ov_clear( &oggStream );
          log.printEnd( " Failed to read Vorbis header" );
          throw Exception( "Music loading failed" );
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
          throw Exception( "Music loading failed" );
        }

        loadMusicBuffer( musicBuffers[0] );
        loadMusicBuffer( musicBuffers[1] );

        alSourceQueueBuffers( musicSource, 2, &musicBuffers[0] );
        alSourcePlay( musicSource );

        currentTrack  = selectedTrack;
        selectedTrack = -1;

        hard_assert( alGetError() == AL_NO_ERROR );

        log.printEnd( " OK" );
      }
    }
    else if( currentTrack != -1 ) {
      int nProcessed;
      alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &nProcessed );

      for( int i = 0; i < nProcessed; ++i ) {
        uint buffer;
        alSourceUnqueueBuffers( musicSource, 1, &buffer );

        if( loadMusicBuffer( buffer ) ) {
          alSourceQueueBuffers( musicSource, 1, &buffer );
        }
      }

      ALint value;
      alGetSourcei( musicSource, AL_SOURCE_STATE, &value );

      if( value == AL_STOPPED ) {
        currentTrack = -1;

        ov_clear( &oggStream );
      }
    }
  }

  bool Sound::init( int* argc, char** argv )
  {
    log.println( "Initialising Sound {" );
    log.indent();

    alutInit( argc, argv );
    if( alutGetError() != ALUT_ERROR_NO_ERROR ) {
      log.println( "Failed to initialise ALUT" );
      log.unindent();
      log.println( "}" );
      return false;
    }

    hard_assert( alGetError() == AL_NO_ERROR );

    DArray<String> extensions;
    String sExtensions = alGetString( AL_EXTENSIONS );
    sExtensions.trim().split( ' ', &extensions );

    log.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
    log.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
    log.println( "OpenAL version: %s", alGetString( AL_VERSION ) );
    log.println( "OpenAL extensions {" );
    log.indent();
    foreach( extension, extensions.citer() ) {
      log.println( "%s", extension->cstr() );
    }
    log.unindent();
    log.println( "}" );

    log.println( "ALUT version: %d.%d", alutGetMajorVersion(), alutGetMinorVersion() );
    log.println( "ALUT supported formats: %s", alutGetMIMETypes( ALUT_LOADER_BUFFER ) );

    selectedTrack = -1;
    currentTrack  = -1;

    alGenBuffers( 2, musicBuffers );
    alGenSources( 1, &musicSource );

    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );

    setVolume( config.getSet( "sound.volume", 1.0f ) );
    setMusicVolume( config.getSet( "sound.volume.music", 0.5f ) );

    log.unindent();
    log.println( "}" );

    hard_assert( alGetError() == AL_NO_ERROR );

    return true;
  }

  void Sound::free()
  {
    log.print( "Shutting down Sound ..." );

    stopMusic();

    playedStructs.dealloc();

    alDeleteSources( 1, &musicSource );
    alDeleteBuffers( 2, musicBuffers );

    hard_assert( alGetError() == AL_NO_ERROR );

    alutExit();
    log.printEnd( " OK" );
  }

}
}
