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

#include "client/OpenAL.hpp"

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

  void Sound::setVolume( float volume_ )
  {
    volume = volume_;
    alListenerf( AL_GAIN, volume_ );
  }

  void Sound::setMusicVolume( float volume ) const
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

  void Sound::resume() const
  {
    alcProcessContext( soundContext );
    alListenerf( AL_GAIN, volume );
  }

  void Sound::suspend() const
  {
    alListenerf( AL_GAIN, 0.0f );
    alcSuspendContext( soundContext );
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

    OZ_AL_CHECK_ERROR();
  }

  void Sound::update()
  {
    OZ_AL_CHECK_ERROR();

    if( selectedTrack != -1 ) {
      alSourceStop( musicSource );

      int nQueued;
      alGetSourcei( musicSource, AL_BUFFERS_QUEUED, &nQueued );

      uint buffer[2];
      alSourceUnqueueBuffers( musicSource, nQueued, buffer );

      ov_clear( &oggStream );

      OZ_AL_CHECK_ERROR();

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

        OZ_AL_CHECK_ERROR();

        log.printEnd( " OK" );
      }
    }
    else if( currentTrack != -1 ) {
      bool hasLoaded = false;

      int nProcessed;
      alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &nProcessed );

      for( int i = 0; i < nProcessed; ++i ) {
        uint buffer;
        alSourceUnqueueBuffers( musicSource, 1, &buffer );

        if( loadMusicBuffer( buffer ) ) {
          alSourceQueueBuffers( musicSource, 1, &buffer );
          hasLoaded = true;
        }
      }

      ALint value;
      alGetSourcei( musicSource, AL_SOURCE_STATE, &value );

      if( value == AL_STOPPED ) {
        if( hasLoaded ) {
          alSourcePlay( musicSource );
        }
        else {
          currentTrack = -1;

          ov_clear( &oggStream );
        }
      }
    }
  }

  void Sound::init()
  {
    log.println( "Initialising Sound {" );
    log.indent();

    musicBuffers[0] = 0;
    musicBuffers[1] = 0;

    const char* deviceName = config.getSet( "sound.device", "" );

    soundDevice = alcOpenDevice( deviceName );
    if( soundDevice == null ) {
      log.println( "Failed to open OpenAL device" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Failed to open OpenAL device" );
    }

    int defaultAttributes[] = {
      ALC_FREQUENCY, config.getSet( "sound.frequency", 44100 ),
      ALC_SYNC, 0,
      ALC_MONO_SOURCES, 255,
      ALC_STEREO_SOURCES, 1,
      0
    };

    soundContext = alcCreateContext( soundDevice, defaultAttributes );
    if( soundContext == null ) {
      log.println( "Failed to create OpenAL context" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Failed to create OpenAL context" );
    }

    if( alcMakeContextCurrent( soundContext ) != ALC_TRUE ) {
      log.println( "Failed to select OpenAL context" );
      log.unindent();
      log.println( "}" );
      throw Exception( "Failed to select OpenAL context" );
    }

    OZ_AL_CHECK_ERROR();

    log.println( "OpenAL device: %s", alcGetString( soundDevice, ALC_DEVICE_SPECIFIER ) );

    int nAttributes;
    alcGetIntegerv( soundDevice, ALC_ATTRIBUTES_SIZE, 1, &nAttributes );

    int* attributes = new int[nAttributes];
    alcGetIntegerv( soundDevice, ALC_ALL_ATTRIBUTES, nAttributes, attributes );

    log.println( "OpenAL attributes {" );
    log.indent();

    for( int i = 0; i < nAttributes; i += 2 ) {
      switch( attributes[i] ) {
        case ALC_FREQUENCY: {
          log.println( "ALC_FREQUENCY: %d Hz", attributes[i + 1] );
          break;
        }
        case ALC_REFRESH: {
          log.println( "ALC_REFRESH: %d Hz", attributes[i + 1] );
          break;
        }
        case ALC_SYNC: {
          log.println( "ALC_SYNC: %s", attributes[i + 1] != 0 ? "on" : "off" );
          break;
        }
        case ALC_MONO_SOURCES: {
          log.println( "ALC_MONO_SOURCES: %d", attributes[i + 1] );
          break;
        }
        case ALC_STEREO_SOURCES: {
          log.println( "ALC_STEREO_SOURCES: %d", attributes[i + 1] );
          break;
        }
      }
    }

    delete[] attributes;

    log.unindent();
    log.println( "}" );

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

    selectedTrack = -1;
    currentTrack  = -1;

    alGenBuffers( 2, musicBuffers );
    alGenSources( 1, &musicSource );

    alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );

    setVolume( config.getSet( "sound.volume", 1.0f ) );
    setMusicVolume( config.getSet( "sound.volume.music", 0.5f ) );

    log.unindent();
    log.println( "}" );

    OZ_AL_CHECK_ERROR();
  }

  void Sound::free()
  {
    log.print( "Shutting down Sound ..." );

    if( soundContext != null ) {
      stopMusic();

      playedStructs.dealloc();

      alDeleteSources( 1, &musicSource );
      alDeleteBuffers( 2, musicBuffers );

      OZ_AL_CHECK_ERROR();

      alcDestroyContext( soundContext );
      soundContext = null;

      alcCloseDevice( soundDevice );
      soundDevice = null;
    }

    log.printEnd( " OK" );
  }

}
}
