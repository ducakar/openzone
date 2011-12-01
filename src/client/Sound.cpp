/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Sound.cpp
 */

#include "stable.hpp"

#include "client/Sound.hpp"

#include "client/Camera.hpp"
#include "client/OpenAL.hpp"

namespace oz
{
namespace client
{

Sound sound;

const float Sound::MAX_DISTANCE = 160.0f;

void Sound::playCell( int cellX, int cellY )
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for( int i = 0; i < cell.structs.length(); ++i ) {
    int strIndex = cell.structs[i];

    if( !playedStructs.get( strIndex ) ) {
      playedStructs.set( strIndex );

      const Struct* str = orbis.structs[strIndex];
      context.playBSP( str );
    }
  }

  foreach( obj, cell.objects.citer() ) {
    if( obj->flags & Object::AUDIO_BIT ) {
      if( ( camera.p - obj->p ).sqL() < MAX_DISTANCE*MAX_DISTANCE ) {
        context.playAudio( obj, null );
      }
    }
  }
}

#ifdef OZ_NONFREE
inline short Sound::madFixedToShort( mad_fixed_t f )
{
  if( f >= MAD_F_ONE ) {
    return +short( ( 1 << 15 ) - 1 );
  }
  if( f <= -MAD_F_ONE ) {
    return -short( ( 1 << 15 ) - 1 );
  }
  return short( f >> ( MAD_F_FRACBITS - 15 ) );
}
#endif

void Sound::streamOpen( const char* path )
{
  log.print( "Opening music '%s' ...", path );

  File file( path );

  if( file.hasExtension( "oga" ) || file.hasExtension( "ogg" ) ) {
    musicStreamType = OGG;
  }
  else if( file.hasExtension( "mp3" ) ) {
    musicStreamType = MP3;
  }
  else {
    throw Exception( "Unknown extension for file '%s'", path );
  }

  switch( musicStreamType ) {
    case OGG: {
      if( ov_fopen( path, &oggStream ) < 0 ) {
        throw Exception( "Failed to open Ogg stream" );
      }

      vorbis_info* vorbisInfo = ov_info( &oggStream, -1 );

      if( vorbisInfo == null ) {
        ov_clear( &oggStream );
        throw Exception( "Failed to read Vorbis header" );
      }

      musicRate = int( vorbisInfo->rate );
      musicChannels = vorbisInfo->channels;

      if( vorbisInfo->channels == 1 ) {
        musicFormat = AL_FORMAT_MONO16;
      }
      else if( vorbisInfo->channels == 2 ) {
        musicFormat = AL_FORMAT_STEREO16;
      }
      else {
        ov_clear( &oggStream );
        throw Exception( "Invalid number of channels, should be 1 or 2" );
      }

      log.printRaw( " Ogg Vorbis %d Hz %d ch %d kb/s ...", musicRate, musicChannels,
                    int( float( vorbisInfo->bitrate_nominal ) / 1000.0f + 0.5f ) );

      break;
    }
    case MP3: {
#ifdef OZ_NONFREE
      mad_stream_init( &madStream );
      mad_frame_init( &madFrame );
      mad_synth_init( &madSynth );

      mp3File = fopen( path, "rb" );
      if( mp3File == null ) {
        throw Exception( "Failed to open MP3 stream" );
      }

      size_t readSize = fread( madInputBuffer, 1, MAD_INPUT_BUFFER_SIZE, mp3File );
      if( readSize != size_t( MAD_INPUT_BUFFER_SIZE ) ) {
        throw Exception( "Failed to read MP3 stream" );
      }

      mad_stream_buffer( &madStream, madInputBuffer, MAD_INPUT_BUFFER_SIZE );

      while( mad_frame_decode( &madFrame, &madStream ) != 0 ) {
        if( !MAD_RECOVERABLE( madStream.error ) ) {
          throw Exception( "Failed to decode MP3 header" );
        }
      }

      mad_synth_frame( &madSynth, &madFrame );

      madFrameSamples   = madSynth.pcm.length;
      madWrittenSamples = 0;

      musicRate = int( madFrame.header.samplerate );
      musicChannels = MAD_NCHANNELS( &madFrame.header );

      if( musicChannels == 1 ) {
        musicFormat = AL_FORMAT_MONO16;
      }
      else if( musicChannels == 2 ) {
        musicFormat = AL_FORMAT_STEREO16;
      }
      else {
        fclose( mp3File );

        mad_synth_finish( &madSynth );
        mad_frame_finish( &madFrame );
        mad_stream_finish( &madStream );

        throw Exception( "Invalid number of channels, should be 1 or 2" );
      }

      log.printRaw( " MP3 %d Hz %d ch %d kb/s ...", musicRate, musicChannels,
                    int( float( madFrame.header.bitrate ) / 1000.0f + 0.5f ) );
#else
      log.printRaw( " no MP3 support ..." );
#endif
      break;
    }
  }

  log.printEnd( " OK" );
}

void Sound::streamClear()
{
  switch( musicStreamType ) {
    case OGG: {
      ov_clear( &oggStream );
      break;
    }
    case MP3: {
#ifdef OZ_NONFREE
      mad_synth_finish( &madSynth );
      mad_frame_finish( &madFrame );
      mad_stream_finish( &madStream );
#endif
      break;
    }
  }
}

int Sound::streamDecode()
{
  switch( musicStreamType ) {
    case OGG: {
      int bytesRead = 0;
      int result;
      int section;

      do {
        result = int( ov_read( &oggStream, &musicBuffer[bytesRead], MUSIC_BUFFER_SIZE - bytesRead,
                               false, 2, true, &section ) );
        bytesRead += result;
        if( result <= 0 ) {
          return 0;
        }
      }
      while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

      return bytesRead;
    }
    case MP3: {
#ifdef OZ_NONFREE
      short*       output    = reinterpret_cast<short*>( musicBuffer );
      const short* outputEnd = reinterpret_cast<const short*>( musicBuffer + MUSIC_BUFFER_SIZE );

      do {
        for( ; madWrittenSamples < madFrameSamples; ++madWrittenSamples ) {
          hard_assert( output <= outputEnd );

          if( output == outputEnd ) {
            return MUSIC_BUFFER_SIZE;
          }

          *output = madFixedToShort( madSynth.pcm.samples[0][madWrittenSamples] );
          ++output;

          if( musicChannels == 2 ) {
            *output = madFixedToShort( madSynth.pcm.samples[1][madWrittenSamples] );
            ++output;
          }
        }

        while( mad_frame_decode( &madFrame, &madStream ) != 0 ) {
          if( madStream.error == MAD_ERROR_BUFLEN ) {
            size_t bytesLeft;

            if( madStream.next_frame == null ) {
              bytesLeft = 0;
            }
            else {
              bytesLeft = size_t( madStream.bufend - madStream.next_frame );

              memmove( madInputBuffer, madStream.next_frame, bytesLeft );
            }

            size_t bytesRead = fread( madInputBuffer + bytesLeft, 1,
                                      MAD_INPUT_BUFFER_SIZE - bytesLeft, mp3File );

            if( bytesRead == 0 ) {
              return int( reinterpret_cast<char*>( output ) - musicBuffer );
            }
            else if( bytesRead < MAD_INPUT_BUFFER_SIZE - bytesLeft ) {
              memset( madInputBuffer + bytesLeft + bytesRead, 0, MAD_BUFFER_GUARD );
            }

            mad_stream_buffer( &madStream, madInputBuffer, bytesLeft + bytesRead );
          }
          else if( !MAD_RECOVERABLE( madStream.error ) ) {
            throw Exception( "Unrecoverable error during MP3 decoding" );
          }
        }

        mad_synth_frame( &madSynth, &madFrame );

        madFrameSamples   = madSynth.pcm.length;
        madWrittenSamples = 0;
      }
      while( true );
#else
      return 0;
#endif
    }
  }
}

bool Sound::loadMusicBuffer( uint buffer )
{
  int length = streamDecode();
  if( length == 0 ) {
    return false;
  }

  alBufferData( buffer, musicFormat, musicBuffer, length, musicRate );
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
  float orientation[] = { camera.at.x, camera.at.y, camera.at.y,
                          camera.up.x, camera.up.y, camera.up.z };
  // add new sounds
  alListenerfv( AL_ORIENTATION, orientation );
  alListenerfv( AL_POSITION, camera.p );

  if( playedStructs.length() < orbis.structs.length() ) {
    playedStructs.dealloc();
    playedStructs.alloc( orbis.structs.length() );
  }
  playedStructs.clearAll();

  Span span = orbis.getInters( camera.p, MAX_DISTANCE + AABB::MAX_DIM );

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

    streamClear();

    OZ_AL_CHECK_ERROR();

    currentTrack = -1;

    if( selectedTrack == -2 ) {
      selectedTrack = -1;
    }
    else {
      const char* path = library.musics[selectedTrack].path;

      streamOpen( path );

      if( loadMusicBuffer( musicBuffers[0] ) && loadMusicBuffer( musicBuffers[1] ) ) {
        alSourceQueueBuffers( musicSource, 2, &musicBuffers[0] );
        alSourcePlay( musicSource );

        currentTrack = selectedTrack;
      }
      else {
        streamClear();

        currentTrack = -1;
      }

      selectedTrack = -1;

      OZ_AL_CHECK_ERROR();
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
        streamClear();
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

  log.print( "Initialising device '%s' ...", deviceName );

  soundDevice = alcOpenDevice( deviceName );
  if( soundDevice == null ) {
    throw Exception( "Failed to open OpenAL device" );
  }

  int defaultAttributes[] = {
    ALC_SYNC, AL_FALSE,
    ALC_MONO_SOURCES, 255,
    ALC_STEREO_SOURCES, 1,
    0
  };

  soundContext = alcCreateContext( soundDevice, defaultAttributes );
  if( soundContext == null ) {
    throw Exception( "Failed to create OpenAL context" );
  }

  if( alcMakeContextCurrent( soundContext ) != ALC_TRUE ) {
    throw Exception( "Failed to select OpenAL context" );
  }

  log.printEnd( " OK" );

  OZ_AL_CHECK_ERROR();

  log.println( "OpenAL context device: %s", alcGetString( soundDevice, ALC_DEVICE_SPECIFIER ) );

  if( log.isVerbose ) {
    int nAttributes;
    alcGetIntegerv( soundDevice, ALC_ATTRIBUTES_SIZE, 1, &nAttributes );

    int* attributes = new int[nAttributes];
    alcGetIntegerv( soundDevice, ALC_ALL_ATTRIBUTES, nAttributes, attributes );

    log.println( "OpenAL context attributes {" );
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
  }

  log.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
  log.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
  log.println( "OpenAL version: %s", alGetString( AL_VERSION ) );

  if( log.isVerbose ) {
    String sExtensions = alGetString( AL_EXTENSIONS );
    DArray<String> extensions = sExtensions.trim().split( ' ' );

    log.println( "OpenAL extensions {" );
    log.indent();

    foreach( extension, extensions.citer() ) {
      log.println( "%s", extension->cstr() );
    }

    log.unindent();
    log.println( "}" );
  }

  selectedTrack = -1;
  currentTrack  = -1;

  alGenBuffers( 2, musicBuffers );
  alGenSources( 1, &musicSource );

  alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );

  setVolume( config.getSet( "sound.volume", 1.0f ) );
  setMusicVolume( 0.5f );

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
