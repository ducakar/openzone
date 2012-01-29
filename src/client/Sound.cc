/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/Sound.cc
 */

#include "stable.hh"

#include "client/Sound.hh"

#include "client/Camera.hh"
#include "client/OpenAL.hh"

#include <limits>

#define OZ_DLDECL( name ) \
  static decltype( ::name )* name = null

#define OZ_DLLOAD( l, name ) \
  *reinterpret_cast<void**>( &name ) = SDL_LoadFunction( l, #name ); \
  if( name == null ) { \
    throw Exception( "Failed loading " #name " from libmad" ); \
  }

namespace oz
{
namespace client
{

Sound sound;

const float Sound::MAX_DISTANCE = 160.0f;

OZ_DLDECL( mad_stream_init   );
OZ_DLDECL( mad_stream_finish );
OZ_DLDECL( mad_stream_buffer );
OZ_DLDECL( mad_frame_init    );
OZ_DLDECL( mad_frame_finish  );
OZ_DLDECL( mad_frame_decode  );
OZ_DLDECL( mad_synth_init    );
OZ_DLDECL( mad_synth_frame   );

OZ_DLDECL( NeAACDecInit      );
OZ_DLDECL( NeAACDecOpen      );
OZ_DLDECL( NeAACDecClose     );
OZ_DLDECL( NeAACDecDecode    );

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle );
static ov_callbacks VORBIS_CALLBACKS = { vorbisRead, null, null, null };

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle )
{
  return size_t( PHYSFS_read( reinterpret_cast<PHYSFS_File*>( handle ), buffer,
                              uint( size ), uint( n ) ) );
}

static inline short madFixedToShort( mad_fixed_t f )
{
  if( f < -MAD_F_ONE ) {
    return std::numeric_limits<short>::min();
  }
  else if( f > MAD_F_ONE ) {
    return std::numeric_limits<short>::max();
  }
  else {
    return short( f >> ( MAD_F_FRACBITS - 15 ) );
  }
}

int Sound::musicMain( void* )
{
  try {
    sound.musicRun();
  }
  catch( const std::exception& e ) {
    log.verboseMode = false;
    log.printException( &e );

    System::bell();
    System::abort();
  }
  return 0;
}

void Sound::musicOpen( const char* path )
{
  File file( path );

  if( file.hasExtension( "oga" ) || file.hasExtension( "ogg" ) ) {
    musicStreamType = OGG;
  }
  else if( file.hasExtension( "mp3" ) ) {
    if( libmad != null ) {
      musicStreamType = MP3;
    }
    else {
      musicStreamType = NONE;
    }
  }
  else if( file.hasExtension( "aac" ) ) {
    if( libfaad != null ) {
      musicStreamType = AAC;
    }
    else {
      musicStreamType = NONE;
    }
  }
  else {
    throw Exception( "Unknown extension for file '%s'", path );
  }

  switch( musicStreamType ) {
    case NONE: {
      break;
    }
    case OGG: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == null ) {
        throw Exception( "Failed to open file '%s'", path );
      }

      if( ov_open_callbacks( musicFile, &oggStream, null, 0, VORBIS_CALLBACKS ) < 0 ) {
        throw Exception( "Failed to open Ogg stream" );
      }

      vorbis_info* vorbisInfo = ov_info( &oggStream, -1 );

      if( vorbisInfo == null ) {
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
        throw Exception( "Invalid number of channels, should be 1 or 2" );
      }

      break;
    }
    case MP3: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == null ) {
        throw Exception( "Failed to open file '%s'", path );
      }

      mad_stream_init( &madStream );
      mad_frame_init( &madFrame );
      mad_synth_init( &madSynth );

      size_t readSize = size_t( PHYSFS_read( musicFile, musicInputBuffer,
                                             1, MUSIC_INPUT_BUFFER_SIZE ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        throw Exception( "Failed to read MP3 stream" );
      }

      mad_stream_buffer( &madStream, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE );

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
        throw Exception( "Invalid number of channels, should be 1 or 2" );
      }

      break;
    }
    case AAC: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == null ) {
        throw Exception( "Failed to open file '%s'", path );
      }

      aacDecoder = NeAACDecOpen();

      size_t readSize = size_t( PHYSFS_read( musicFile, musicInputBuffer,
                                             1, MUSIC_INPUT_BUFFER_SIZE ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        throw Exception( "Failed to read AAC stream" );
      }

      ulong aacRate;
      ubyte aacChannels;

      long skipBytes = NeAACDecInit( aacDecoder, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE,
                                     &aacRate, &aacChannels );
      if( skipBytes < 0 ) {
        throw Exception( "Failed to decode AAC header" );
      }

      memmove( musicInputBuffer, musicInputBuffer + skipBytes, size_t( skipBytes ) );

      readSize = size_t( PHYSFS_read( musicFile,
                                      musicInputBuffer + MUSIC_INPUT_BUFFER_SIZE - skipBytes,
                                      1, uint( skipBytes ) ) );

      if( readSize != size_t( skipBytes ) ) {
        throw Exception( "Failed to read AAC stream" );
      }

      aacBufferBytes  = 0;
      aacWrittenBytes = 0;
      aacInputBytes   = MUSIC_INPUT_BUFFER_SIZE;

      musicRate = int( aacRate );
      musicChannels = int( aacChannels );

      if( musicChannels == 1 ) {
        musicFormat = AL_FORMAT_MONO16;
      }
      else if( musicChannels == 2 ) {
        musicFormat = AL_FORMAT_STEREO16;
      }
      else {
        throw Exception( "Invalid number of channels, should be 1 or 2" );
      }

      break;
    }
  }
}

void Sound::musicClear()
{
  switch( musicStreamType ) {
    case NONE: {
      break;
    }
    case OGG: {
      ov_clear( &oggStream );

      PHYSFS_close( musicFile );
      break;
    }
    case MP3: {
      mad_synth_finish( &madSynth );
      mad_frame_finish( &madFrame );
      mad_stream_finish( &madStream );

      PHYSFS_close( musicFile );
      break;
    }
    case AAC: {
      NeAACDecClose( aacDecoder );

      PHYSFS_close( musicFile );
      break;
    }
  }
}

int Sound::musicDecode()
{
  switch( musicStreamType ) {
    case NONE: {
      return 0;
    }
    case OGG: {
      int bytesRead = 0;
      int result;
      int section;

      do {
        result = int( ov_read( &oggStream, &musicBuffer[bytesRead],
                               MUSIC_BUFFER_SIZE - bytesRead, false, 2, true, &section ) );
        bytesRead += result;

        if( result < 0 ) {
          throw Exception( "Error during Ogg Vorbis decoding" );
        }
      }
      while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

      return bytesRead;
    }
    case MP3: {
      short* musicOutput    = reinterpret_cast<short*>( musicBuffer );
      short* musicOutputEnd = reinterpret_cast<short*>( musicBuffer + MUSIC_BUFFER_SIZE );

      do {
        for( ; madWrittenSamples < madFrameSamples; ++madWrittenSamples ) {
          hard_assert( musicOutput <= musicOutputEnd );

          if( musicOutput == musicOutputEnd ) {
            return MUSIC_BUFFER_SIZE;
          }

          *musicOutput = madFixedToShort( madSynth.pcm.samples[0][madWrittenSamples] );
          ++musicOutput;

          if( musicChannels == 2 ) {
            *musicOutput = madFixedToShort( madSynth.pcm.samples[1][madWrittenSamples] );
            ++musicOutput;
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

              memmove( musicInputBuffer, madStream.next_frame, bytesLeft );
            }

            size_t bytesRead = size_t( PHYSFS_read( musicFile, musicInputBuffer + bytesLeft, 1,
                                                    uint( MUSIC_INPUT_BUFFER_SIZE - bytesLeft ) ) );

            if( bytesRead == 0 ) {
              return int( reinterpret_cast<char*>( musicOutput ) - musicBuffer );
            }
            else if( bytesRead < MUSIC_INPUT_BUFFER_SIZE - bytesLeft ) {
              memset( musicInputBuffer + bytesLeft + bytesRead, 0, MAD_BUFFER_GUARD );
            }

            mad_stream_buffer( &madStream, musicInputBuffer, bytesLeft + bytesRead );
          }
          else if( !MAD_RECOVERABLE( madStream.error ) ) {
            throw Exception( "Unrecoverable error during MP3 decoding" );
          }
        }

        mad_synth_frame( &madSynth, &madFrame );

        madFrameSamples = madSynth.pcm.length;
        madWrittenSamples = 0;
      }
      while( true );
    }
    case AAC: {
      char* musicOutput    = musicBuffer;
      char* musicOutputEnd = musicBuffer + MUSIC_BUFFER_SIZE;

      do {
        if( aacWrittenBytes < aacBufferBytes ) {
          int length = int( aacBufferBytes - aacWrittenBytes );
          int space  = int( musicOutputEnd - musicOutput );

          if( length >= space ) {
            memcpy( musicOutput, aacOutputBuffer + aacWrittenBytes, size_t( space ) );
            aacWrittenBytes += space;

            return MUSIC_BUFFER_SIZE;
          }
          else {
            memcpy( musicOutput, aacOutputBuffer + aacWrittenBytes, size_t( length ) );
            aacWrittenBytes += length;
            musicOutput += length;
          }
        }

        NeAACDecFrameInfo frameInfo;
        aacOutputBuffer = reinterpret_cast<char*>
                          ( NeAACDecDecode( aacDecoder, &frameInfo, musicInputBuffer, aacInputBytes ) );

        if( aacOutputBuffer == null ) {
          return int( musicOutput - musicBuffer );
        }

        size_t bytesConsumed = size_t( frameInfo.bytesconsumed );
        aacInputBytes -= bytesConsumed;
        aacBufferBytes = int( frameInfo.samples * frameInfo.channels );
        aacWrittenBytes = 0;

        memmove( musicInputBuffer, musicInputBuffer + bytesConsumed, aacInputBytes );

        size_t bytesRead = size_t( PHYSFS_read( musicFile, musicInputBuffer + aacInputBytes, 1,
                                                uint( MUSIC_INPUT_BUFFER_SIZE - aacInputBytes ) ) );

        aacInputBytes += bytesRead;
      }
      while( true );
    }
  }
}

void Sound::musicRun()
{
  streamedTrack = -1;

  while( isMusicAlive ) {
    SDL_SemPost( musicMainSemaphore );
    SDL_SemWait( musicAuxSemaphore );

    if( currentTrack != streamedTrack ) {
      if( streamedTrack != -1 ) {
        musicClear();
      }

      streamedTrack = currentTrack;

      if( streamedTrack != -1 ) {
        musicOpen( library.musics[streamedTrack].path );
      }
    }

    if( streamedTrack != -1 ) {
      streamedBytes = musicDecode();
    }
  }
}

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

void Sound::updateMusic()
{
  OZ_AL_CHECK_ERROR();

  if( SDL_SemTryWait( musicMainSemaphore ) != 0 ) {
    return;
  }

  if( selectedTrack != -1 ) {
    currentTrack = selectedTrack == -2 ? -1 : selectedTrack;
    selectedTrack = -1;

    musicBuffersQueued = 0;

    alSourceStop( musicSource );

    int nQueued;
    alGetSourcei( musicSource, AL_BUFFERS_QUEUED, &nQueued );

    if( nQueued != 0 ) {
      uint buffers[2];
      alSourceUnqueueBuffers( musicSource, nQueued, buffers );
    }

    OZ_AL_CHECK_ERROR();

    SDL_SemPost( musicAuxSemaphore );
  }
  else if( currentTrack == -1 ) {
    SDL_SemPost( musicMainSemaphore );
  }
  else if( streamedBytes == 0 ) {
    currentTrack = -1;

    SDL_SemPost( musicAuxSemaphore );
  }
  else {
    bool hasLoaded = false;

    int nProcessed;
    alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &nProcessed );

    if( nProcessed != 0 ) {
      hasLoaded = true;

      uint buffer;
      alSourceUnqueueBuffers( musicSource, 1, &buffer );
      OZ_AL_CHECK_ERROR();
      alBufferData( buffer, musicFormat, musicBuffer, streamedBytes, musicRate );
      OZ_AL_CHECK_ERROR();
      alSourceQueueBuffers( musicSource, 1, &buffer );
      OZ_AL_CHECK_ERROR();
    }
    else if( musicBuffersQueued != 2 ) {
      hasLoaded = true;

      int i = musicBuffersQueued;
      ++musicBuffersQueued;

      alBufferData( musicBufferIds[i], musicFormat, musicBuffer, streamedBytes, musicRate );
      alSourceQueueBuffers( musicSource, 1, &musicBufferIds[i] );
      alSourcePlay( musicSource );
    }

    ALint value;
    alGetSourcei( musicSource, AL_SOURCE_STATE, &value );

    if( value == AL_STOPPED ) {
      alSourcePlay( musicSource );
    }

    if( hasLoaded ) {
      SDL_SemPost( musicAuxSemaphore );
    }
    else {
      SDL_SemPost( musicMainSemaphore );
    }
  }
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
}

void Sound::stopMusic()
{
  selectedTrack = -2;
}

bool Sound::isMusicPlaying() const
{
  return currentTrack != -1 || selectedTrack != -1;
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
  float orientation[] = {
    camera.at.x, camera.at.y, camera.at.y,
    camera.up.x, camera.up.y, camera.up.z
  };

  // add new sounds
  alListenerfv( AL_ORIENTATION, orientation );
  alListenerfv( AL_POSITION, camera.p );

  if( playedStructs.length() < orbis.structs.length() ) {
    playedStructs.dealloc();
    playedStructs.alloc( orbis.structs.length() );
  }
  playedStructs.clearAll();

  Span span = orbis.getInters( camera.p, MAX_DISTANCE + Object::MAX_DIM );

  for( int x = span.minX ; x <= span.maxX; ++x ) {
    for( int y = span.minY; y <= span.maxY; ++y ) {
      playCell( x, y );
    }
  }

  updateMusic();

  OZ_AL_CHECK_ERROR();
}

void Sound::init()
{
  log.println( "Initialising Sound {" );
  log.indent();

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

  log.println( "OpenAL device: %s", alcGetString( soundDevice, ALC_DEVICE_SPECIFIER ) );

  const char* deviceSpec = alcGetString( null, ALC_DEVICE_SPECIFIER );

  log.verboseMode = true;
  log.println( "Available OpenAL devices {" );
  log.indent();

  for( const char* s = deviceSpec; *s != '\0'; s += String::length( s ) + 1 ) {
    log.println( "%s", s );
  }

  log.unindent();
  log.println( "}" );
  log.verboseMode = false;

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

  log.println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
  log.println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
  log.println( "OpenAL version: %s", alGetString( AL_VERSION ) );

  String sExtensions = alGetString( AL_EXTENSIONS );
  DArray<String> extensions = sExtensions.trim().split( ' ' );

  log.verboseMode = true;
  log.println( "OpenAL extensions {" );
  log.indent();

  foreach( extension, extensions.citer() ) {
    log.println( "%s", extension->cstr() );
  }

  log.unindent();
  log.println( "}" );
  log.verboseMode = false;

  selectedTrack = -1;
  currentTrack  = -1;
  streamedTrack = -1;

  alGenBuffers( 2, musicBufferIds );
  alGenSources( 1, &musicSource );

  musicBuffersQueued = 0;

  alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );

  setVolume( config.getSet( "sound.volume", 1.0f ) );
  setMusicVolume( 0.5f );

#ifdef _WIN32
  libmad  = SDL_LoadObject( "libmad.dll" );
  libfaad = SDL_LoadObject( "libfaad2.dll" );
#else
  libmad  = SDL_LoadObject( "libmad.so" );
  libfaad = SDL_LoadObject( "libfaad.so" );
#endif

  if( libmad != null ) {
    OZ_DLLOAD( libmad, mad_stream_init   );
    OZ_DLLOAD( libmad, mad_stream_finish );
    OZ_DLLOAD( libmad, mad_stream_buffer );
    OZ_DLLOAD( libmad, mad_frame_init    );
    OZ_DLLOAD( libmad, mad_frame_finish  );
    OZ_DLLOAD( libmad, mad_frame_decode  );
    OZ_DLLOAD( libmad, mad_synth_init    );
    OZ_DLLOAD( libmad, mad_synth_frame   );
  }

  if( libfaad != null ) {
    OZ_DLLOAD( libfaad, NeAACDecInit   );
    OZ_DLLOAD( libfaad, NeAACDecOpen   );
    OZ_DLLOAD( libfaad, NeAACDecClose  );
    OZ_DLLOAD( libfaad, NeAACDecDecode );
  }

  isMusicAlive       = true;
  musicMainSemaphore = SDL_CreateSemaphore( 0 );
  musicAuxSemaphore  = SDL_CreateSemaphore( 0 );
  musicThread        = SDL_CreateThread( musicMain, null );

  log.unindent();
  log.println( "}" );

  OZ_AL_CHECK_ERROR();
}

void Sound::free()
{
  log.print( "Freeing Sound ..." );

  selectedTrack = -1;
  currentTrack  = -1;
  streamedTrack = -1;

  isMusicAlive = false;

  SDL_SemPost( musicAuxSemaphore );
  SDL_WaitThread( musicThread, null );

  SDL_DestroySemaphore( musicAuxSemaphore );
  SDL_DestroySemaphore( musicMainSemaphore );

  musicMainSemaphore = null;
  musicAuxSemaphore  = null;
  musicThread        = null;

  if( libfaad != null ) {
    SDL_UnloadObject( libfaad );
  }
  if( libmad != null ) {
    SDL_UnloadObject( libmad );
  }

  if( soundContext != null ) {
    playedStructs.dealloc();

    alSourceStop( musicSource );
    alDeleteSources( 1, &musicSource );
    alDeleteBuffers( 2, musicBufferIds );

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
