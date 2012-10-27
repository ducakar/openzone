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

#include <stable.hh>
#include <client/Sound.hh>

#include <client/Camera.hh>
#include <client/eSpeak.hh>
#include <client/NaCl.hh>

#ifdef __native_client__
# include <ppapi/cpp/instance.h>
# include <ppapi/cpp/module.h>

extern "C"
void alSetPpapiInfo( PP_Instance instance, PPB_GetInterface getInterface );
#endif

#if PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0
# define PHYSFS_readBytes( handle, buffer, len ) PHYSFS_read( handle, buffer, 1, uint( len ) )
#endif

#define OZ_DLLOAD( l, name ) \
  *( void** )( &name ) = SDL_LoadFunction( l, #name ); \
  if( name == nullptr ) { \
    OZ_ERROR( "Failed loading " #name " from library" ); \
  }

namespace oz
{
namespace client
{

static decltype( ::mad_stream_init   )* mad_stream_init   = nullptr;
static decltype( ::mad_stream_finish )* mad_stream_finish = nullptr;
static decltype( ::mad_stream_buffer )* mad_stream_buffer = nullptr;
static decltype( ::mad_frame_init    )* mad_frame_init    = nullptr;
static decltype( ::mad_frame_finish  )* mad_frame_finish  = nullptr;
static decltype( ::mad_frame_decode  )* mad_frame_decode  = nullptr;
static decltype( ::mad_synth_init    )* mad_synth_init    = nullptr;
static decltype( ::mad_synth_frame   )* mad_synth_frame   = nullptr;

static decltype( ::NeAACDecInit      )* NeAACDecInit      = nullptr;
static decltype( ::NeAACDecOpen      )* NeAACDecOpen      = nullptr;
static decltype( ::NeAACDecClose     )* NeAACDecClose     = nullptr;
static decltype( ::NeAACDecDecode    )* NeAACDecDecode    = nullptr;

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle );
static ov_callbacks VORBIS_CALLBACKS = { vorbisRead, nullptr, nullptr, nullptr };

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle )
{
  return size_t( PHYSFS_readBytes( static_cast<PHYSFS_File*>( handle ), buffer,
                                   ulong64( size * n ) ) );
}

OZ_ALWAYS_INLINE
static inline short madFixedToShort( mad_fixed_t f )
{
  if( f < -MAD_F_ONE ) {
    return SHRT_MIN;
  }
  else if( f > +MAD_F_ONE ) {
    return SHRT_MAX;
  }
  else {
    return short( f >> ( MAD_F_FRACBITS - 15 ) );
  }
}

const float Sound::MAX_DISTANCE = 192.0f;

void Sound::musicMain( void* )
{
  sound.musicRun();
}

void Sound::soundMain( void* )
{
  sound.soundRun();
}

void Sound::musicOpen( const char* path )
{
  PFile file( path );

  if( file.hasExtension( "oga" ) || file.hasExtension( "ogg" ) ) {
    musicStreamType = OGG;
  }
  else if( file.hasExtension( "mp3" ) ) {
    if( libMad != nullptr ) {
      musicStreamType = MP3;
    }
    else {
      musicStreamType = NONE;
    }
  }
  else if( file.hasExtension( "aac" ) ) {
    if( libFaad != nullptr ) {
      musicStreamType = AAC;
    }
    else {
      musicStreamType = NONE;
    }
  }
  else {
    OZ_ERROR( "Unknown extension for file '%s'", path );
  }

  switch( musicStreamType ) {
    case NONE: {
      break;
    }
    case OGG: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == nullptr ) {
        OZ_ERROR( "Failed to open file '%s'", path );
      }

      if( ov_open_callbacks( musicFile, &oggStream, nullptr, 0, VORBIS_CALLBACKS ) < 0 ) {
        OZ_ERROR( "Failed to open Ogg stream in '%s'", path );
      }

      vorbis_info* vorbisInfo = ov_info( &oggStream, -1 );
      if( vorbisInfo == nullptr ) {
        OZ_ERROR( "Corrupted Vorbis header in '%s'", path );
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
        OZ_ERROR( "Invalid number of channels in '%s', should be 1 or 2", path );
      }

      break;
    }
    case MP3: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == nullptr ) {
        OZ_ERROR( "Failed to open file '%s'", path );
      }

      mad_stream_init( &madStream );
      mad_frame_init( &madFrame );
      mad_synth_init( &madSynth );

      size_t readSize = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer,
                                                  ulong64( MUSIC_INPUT_BUFFER_SIZE ) ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        OZ_ERROR( "Failed to read MP3 stream in '%s'", path );
      }

      mad_stream_buffer( &madStream, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE );

      while( mad_frame_decode( &madFrame, &madStream ) != 0 ) {
        if( !MAD_RECOVERABLE( madStream.error ) ) {
          OZ_ERROR( "Corrupted MP3 header in '%s'", path );
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
        OZ_ERROR( "Invalid number of channels in '%s', should be 1 or 2", path );
      }

      break;
    }
    case AAC: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == nullptr ) {
        OZ_ERROR( "Failed to open file '%s'", path );
      }

      aacDecoder = NeAACDecOpen();

      size_t readSize = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer,
                                                  ulong64( MUSIC_INPUT_BUFFER_SIZE ) ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        OZ_ERROR( "Failed to read AAC stream in '%s'", path );
      }

      ulong aacRate;
      ubyte aacChannels;

      long skipBytes = NeAACDecInit( aacDecoder, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE,
                                     &aacRate, &aacChannels );
      if( skipBytes < 0 ) {
        OZ_ERROR( "Corrupted AAC header in '%s'", path );
      }

      memmove( musicInputBuffer, musicInputBuffer + skipBytes, size_t( skipBytes ) );

      readSize = size_t( PHYSFS_readBytes( musicFile,
                                           musicInputBuffer + MUSIC_INPUT_BUFFER_SIZE - skipBytes,
                                           ulong64( skipBytes ) ) );

      if( readSize != size_t( skipBytes ) ) {
        OZ_ERROR( "Failed to read AAC stream in '%s'", path );
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
        OZ_ERROR( "Invalid number of channels in '%s', should be 1 or 2", path );
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
          OZ_ERROR( "Error during Ogg Vorbis decoding of '%s'",
                    liber.musicTracks[streamedTrack].path.cstr() );
        }
      }
      while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

      return bytesRead;
    }
    case MP3: {
      char* musicOutput    = musicBuffer;
      char* musicOutputEnd = musicBuffer + MUSIC_BUFFER_SIZE;

      do {
        for( ; madWrittenSamples < madFrameSamples; ++madWrittenSamples ) {
          hard_assert( musicOutput <= musicOutputEnd );

          if( musicOutput == musicOutputEnd ) {
            return MUSIC_BUFFER_SIZE;
          }

          short value = madFixedToShort( madSynth.pcm.samples[0][madWrittenSamples] );

#ifdef OZ_BIG_ENDIAN
          musicOutput[0] = char( value >> 8 );
          musicOutput[1] = char( value );
#else
          musicOutput[0] = char( value );
          musicOutput[1] = char( value >> 8 );
#endif
          musicOutput += 2;

          if( musicChannels == 2 ) {
            value = madFixedToShort( madSynth.pcm.samples[1][madWrittenSamples] );

#ifdef OZ_BIG_ENDIAN
            musicOutput[0] = char( value >> 8 );
            musicOutput[1] = char( value );
#else
            musicOutput[0] = char( value );
            musicOutput[1] = char( value >> 8 );
#endif
            musicOutput += 2;
          }
        }

        while( mad_frame_decode( &madFrame, &madStream ) != 0 ) {
          if( madStream.error == MAD_ERROR_BUFLEN ) {
            size_t bytesLeft;

            if( madStream.next_frame == nullptr ) {
              bytesLeft = 0;
            }
            else {
              bytesLeft = size_t( madStream.bufend - madStream.next_frame );

              memmove( musicInputBuffer, madStream.next_frame, bytesLeft );
            }

            size_t bytesRead = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer + bytesLeft,
                                                         ulong64( MUSIC_INPUT_BUFFER_SIZE - bytesLeft ) ) );

            if( bytesRead == 0 ) {
              return int( reinterpret_cast<char*>( musicOutput ) - musicBuffer );
            }
            else if( bytesRead < MUSIC_INPUT_BUFFER_SIZE - bytesLeft ) {
              memset( musicInputBuffer + bytesLeft + bytesRead, 0, MAD_BUFFER_GUARD );
            }

            mad_stream_buffer( &madStream, musicInputBuffer, bytesLeft + bytesRead );
          }
          else if( !MAD_RECOVERABLE( madStream.error ) ) {
            OZ_ERROR( "Unrecoverable error during MP3 decoding of '%s'",
                      liber.musicTracks[streamedTrack].path.cstr() );
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
        aacOutputBuffer = static_cast<char*>
                          ( NeAACDecDecode( aacDecoder, &frameInfo, musicInputBuffer, aacInputBytes ) );

        if( aacOutputBuffer == nullptr ) {
          return int( musicOutput - musicBuffer );
        }

        size_t bytesConsumed = size_t( frameInfo.bytesconsumed );
        aacInputBytes -= bytesConsumed;
        aacBufferBytes = int( frameInfo.samples * frameInfo.channels );
        aacWrittenBytes = 0;

        memmove( musicInputBuffer, musicInputBuffer + bytesConsumed, aacInputBytes );

        size_t bytesRead = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer + aacInputBytes,
                                                     ulong64( MUSIC_INPUT_BUFFER_SIZE - aacInputBytes ) ) );

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
    musicMainSemaphore.post();
    musicAuxSemaphore.wait();

    if( currentTrack != streamedTrack ) {
      if( streamedTrack >= 0 ) {
        musicClear();
      }

      streamedTrack = currentTrack;

      if( streamedTrack >= 0 ) {
        musicOpen( liber.musicTracks[streamedTrack].path );
      }
    }

    if( streamedTrack >= 0 ) {
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

  OZ_AL_CHECK_ERROR();

  foreach( obj, cell.objects.citer() ) {
    if( obj->flags & Object::AUDIO_BIT ) {
      context.playAudio( obj, nullptr );
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Sound::updateMusic()
{
  if( !musicMainSemaphore.tryWait() ) {
    return;
  }

  if( selectedTrack != -1 && selectedTrack != currentTrack ) {
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

    musicAuxSemaphore.post();
  }
  else if( currentTrack < 0 ) {
    musicMainSemaphore.post();
  }
  else if( streamedBytes == 0 ) {
    currentTrack = -1;

    musicAuxSemaphore.post();
  }
  else {
    bool hasLoaded = false;

    int nProcessed;
    alGetSourcei( musicSource, AL_BUFFERS_PROCESSED, &nProcessed );

    if( nProcessed != 0 ) {
      hasLoaded = true;

      uint buffer;
      alSourceUnqueueBuffers( musicSource, 1, &buffer );
      alBufferData( buffer, musicFormat, musicBuffer, streamedBytes, musicRate );
      alSourceQueueBuffers( musicSource, 1, &buffer );
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
      musicAuxSemaphore.post();
    }
    else {
      musicMainSemaphore.post();
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Sound::soundRun()
{
  soundAuxSemaphore.wait();

  while( isSoundAlive ) {
#ifndef OZ_ADDRESS_SANITIZER
    float orientation[] = {
      camera.at.x, camera.at.y, camera.at.z,
      camera.up.x, camera.up.y, camera.up.z
    };

    OZ_AL_CHECK_ERROR();

    // add new sounds
    alListenerfv( AL_ORIENTATION, orientation );
    alListenerfv( AL_POSITION, camera.p );
    alListenerfv( AL_VELOCITY, camera.velocity );
#endif

    if( playedStructs.length() < orbis.structs.length() ) {
      playedStructs.deallocate();
      playedStructs.allocate( orbis.structs.length() );
    }
    playedStructs.clearAll();

    Span span = orbis.getInters( camera.p, MAX_DISTANCE + Object::MAX_DIM );

    for( int x = span.minX ; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        playCell( x, y );
      }
    }

#ifndef OZ_ADDRESS_SANITIZER
    updateMusic();
#endif

    soundMainSemaphore.post();
    soundAuxSemaphore.wait();
  }
}

void Sound::setVolume( float volume_ )
{
  volume = volume_;
#ifdef OZ_ADDRESS_SANITIZER
  static_cast<void>( volume_ );
#else
  alListenerf( AL_GAIN, volume_ );
#endif
}

void Sound::setMusicVolume( float volume ) const
{
#ifdef OZ_ADDRESS_SANITIZER
  static_cast<void>( volume );
#else
  alSourcef( musicSource, AL_GAIN, 0.5f * volume );
#endif
}

bool Sound::isMusicPlaying() const
{
  return currentTrack >= 0 || selectedTrack >= 0;
}

int Sound::getCurrentTrack() const
{
  return currentTrack;
}

void Sound::playMusic( int track, bool advanceTrack_ )
{
  hard_assert( track >= 0 );

  selectedTrack = track;
  advanceTrack  = advanceTrack_;
}

void Sound::stopMusic()
{
  selectedTrack = -2;
}

void Sound::resume() const
{
#ifndef OZ_ADDRESS_SANITIZER
  alcProcessContext( soundContext );
  alListenerf( AL_GAIN, volume );
#endif
}

void Sound::suspend() const
{
#ifndef OZ_ADDRESS_SANITIZER
  alListenerf( AL_GAIN, 0.0f );
  alcSuspendContext( soundContext );
#endif
}

void Sound::play()
{
  context.speakSource.mutex.lock();
  soundAuxSemaphore.post();
}

void Sound::sync()
{
  soundMainSemaphore.wait();
  context.speakSource.mutex.unlock();
}

void Sound::init()
{
#ifdef __native_client__
  hard_assert( NaCl::isMainThread() );
#endif

  Log::println( "Initialising Sound {" );
  Log::indent();

#ifdef OZ_ADDRESS_SANITIZER
  config["sound.device"];
#else

#ifdef __native_client__
  alSetPpapiInfo( System::instance->pp_instance(), System::module->get_browser_interface() );
#endif

  const char* deviceSpec = alcGetString( nullptr, ALC_DEVICE_SPECIFIER );

  Log::verboseMode = true;
  Log::println( "Available OpenAL devices {" );
  Log::indent();

  for( const char* s = deviceSpec; *s != '\0'; s += String::length( s ) + 1 ) {
    Log::println( "%s", s );
  }

  Log::unindent();
  Log::println( "}" );
  Log::verboseMode = false;

  const char* deviceName = config.include( "sound.device", "" ).asString();

  if( String::isEmpty( deviceName ) ) {
    deviceName = nullptr;
    Log::print( "Initialising default device ..." );
  }
  else {
    Log::print( "Initialising device '%s' ...", deviceName );
  }

  soundDevice = alcOpenDevice( deviceName );
  if( soundDevice == nullptr ) {
    OZ_ERROR( "Failed to open OpenAL device" );
  }

  int defaultAttributes[] = {
    ALC_SYNC, AL_FALSE,
    ALC_MONO_SOURCES, 255,
    ALC_STEREO_SOURCES, 1,
    0
  };

  soundContext = alcCreateContext( soundDevice, defaultAttributes );
  if( soundContext == nullptr ) {
    OZ_ERROR( "Failed to create OpenAL context" );
  }

  if( alcMakeContextCurrent( soundContext ) != ALC_TRUE ) {
    OZ_ERROR( "Failed to select OpenAL context" );
  }

  Log::printEnd( " OK" );

  OZ_AL_CHECK_ERROR();

  Log::println( "OpenAL context device: %s", alcGetString( soundDevice, ALC_DEVICE_SPECIFIER ) );

  int nAttributes;
  alcGetIntegerv( soundDevice, ALC_ATTRIBUTES_SIZE, 1, &nAttributes );

  int* attributes = new int[nAttributes];
  alcGetIntegerv( soundDevice, ALC_ALL_ATTRIBUTES, nAttributes, attributes );

  Log::println( "OpenAL context attributes {" );
  Log::indent();

  for( int i = 0; i < nAttributes; i += 2 ) {
    switch( attributes[i] ) {
      case ALC_FREQUENCY: {
        Log::println( "ALC_FREQUENCY: %d Hz", attributes[i + 1] );
        break;
      }
      case ALC_REFRESH: {
        Log::println( "ALC_REFRESH: %d Hz", attributes[i + 1] );
        break;
      }
      case ALC_SYNC: {
        Log::println( "ALC_SYNC: %s", attributes[i + 1] != 0 ? "on" : "off" );
        break;
      }
      case ALC_MONO_SOURCES: {
        Log::println( "ALC_MONO_SOURCES: %d", attributes[i + 1] );
        break;
      }
      case ALC_STEREO_SOURCES: {
        Log::println( "ALC_STEREO_SOURCES: %d", attributes[i + 1] );
        break;
      }
      default: {
        break;
      }
    }
  }

  delete[] attributes;

  Log::unindent();
  Log::println( "}" );

  Log::println( "OpenAL vendor: %s", alGetString( AL_VENDOR ) );
  Log::println( "OpenAL renderer: %s", alGetString( AL_RENDERER ) );
  Log::println( "OpenAL version: %s", alGetString( AL_VERSION ) );

  String sExtensions = alGetString( AL_EXTENSIONS );
  DArray<String> extensions = sExtensions.trim().split( ' ' );

  Log::verboseMode = true;
  Log::println( "OpenAL extensions {" );
  Log::indent();

  foreach( extension, extensions.citer() ) {
    Log::println( "%s", extension->cstr() );
  }

  Log::unindent();
  Log::println( "}" );
  Log::verboseMode = false;

#endif // !OZ_ADDRESS_SANITIZER

  selectedTrack = -1;
  currentTrack  = -1;
  streamedTrack = -1;

#ifndef OZ_ADDRESS_SANITIZER
  alGenBuffers( 2, musicBufferIds );
  alGenSources( 1, &musicSource );

  musicBuffersQueued = 0;

  alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );
#endif

  setVolume( config.include( "sound.volume", 1.0f ).asFloat() );
  setMusicVolume( 0.5f );

  const String& speaker = config.include( "sound.speaker", "en" ).asString();

  if( espeak_Initialize != nullptr ) {
    context.speakSampleRate = espeak_Initialize( AUDIO_OUTPUT_SYNCHRONOUS, 500, nullptr, 0 );
    espeak_SetParameter( espeakRATE, 160, 0 );
    espeak_SetVoiceByName( speaker );
    espeak_SetSynthCallback( reinterpret_cast<t_espeak_callback*>( &Context::speakCallback ) );
  }

  isMusicAlive = true;
  isSoundAlive = true;

  musicMainSemaphore.init();
  musicAuxSemaphore.init();
  soundMainSemaphore.init();
  soundAuxSemaphore.init();

  musicThread.start( "music", musicMain, nullptr );
  soundThread.start( "sound", soundMain, nullptr );

  Log::unindent();
  Log::println( "}" );

  OZ_AL_CHECK_ERROR();
}

void Sound::free()
{
#ifdef __native_client__
  hard_assert( NaCl::isMainThread() );
#endif

  Log::print( "Freeing Sound ..." );

  if( espeak_Terminate != nullptr ) {
    espeak_Terminate();
  }

  selectedTrack = -1;
  currentTrack  = -1;
  streamedTrack = -1;

  isSoundAlive = false;
  isMusicAlive = false;

  soundAuxSemaphore.post();
  musicAuxSemaphore.post();
  soundThread.join();
  musicThread.join();

  soundAuxSemaphore.destroy();
  soundMainSemaphore.destroy();
  musicAuxSemaphore.destroy();
  musicMainSemaphore.destroy();

  playedStructs.deallocate();

  if( soundContext != nullptr ) {
#ifndef OZ_ADDRESS_SANITIZER
    alSourceStop( musicSource );
    alDeleteSources( 1, &musicSource );
    alDeleteBuffers( 2, musicBufferIds );

    OZ_AL_CHECK_ERROR();

    alcDestroyContext( soundContext );
    soundContext = nullptr;

    alcCloseDevice( soundDevice );
    soundDevice = nullptr;
#endif
  }

#ifndef __native_client__
  if( libFaad != nullptr ) {
    SDL_UnloadObject( libFaad );
  }
  if( libMad != nullptr ) {
    SDL_UnloadObject( libMad );
  }
  if( libeSpeak != nullptr ) {
    SDL_UnloadObject( libeSpeak );
  }
#endif

  Log::printEnd( " OK" );
}

void Sound::initLibs()
{
#ifndef __native_client__
# ifdef _WIN32
  const char* libeSpeakName = "libespeak.dll";
  const char* libMadName    = "libmad.dll";
  const char* libFaadName   = "libfaad2.dll";
# else
  const char* libeSpeakName = "libespeak.so.1";
  const char* libMadName    = "libmad.so.0";
  const char* libFaadName   = "libfaad.so.2";
# endif

  Log::print( "Linking eSpeak library '%s' ...", libeSpeakName );

  libeSpeak = SDL_LoadObject( libeSpeakName );

  if( libeSpeak == nullptr ) {
    Log::printEnd( " Not found, speech synthesis not supported" );
  }
  else {
    OZ_DLLOAD( libeSpeak, espeak_Initialize );
    OZ_DLLOAD( libeSpeak, espeak_Terminate );
    OZ_DLLOAD( libeSpeak, espeak_SetParameter );
    OZ_DLLOAD( libeSpeak, espeak_SetVoiceByName );
    OZ_DLLOAD( libeSpeak, espeak_SetSynthCallback );
    OZ_DLLOAD( libeSpeak, espeak_Synth );

    Log::printEnd( " OK, speech synthesis supported" );
  }

  Log::print( "Linking MAD library '%s' ...", libMadName );

  libMad = SDL_LoadObject( libMadName );

  if( libMad == nullptr ) {
    liber.mapMP3s = false;

    Log::printEnd( " Not found, MP3 not supported" );
  }
  else {
    OZ_DLLOAD( libMad, mad_stream_init   );
    OZ_DLLOAD( libMad, mad_stream_finish );
    OZ_DLLOAD( libMad, mad_stream_buffer );
    OZ_DLLOAD( libMad, mad_frame_init    );
    OZ_DLLOAD( libMad, mad_frame_finish  );
    OZ_DLLOAD( libMad, mad_frame_decode  );
    OZ_DLLOAD( libMad, mad_synth_init    );
    OZ_DLLOAD( libMad, mad_synth_frame   );

    liber.mapMP3s = true;

    Log::printEnd( " OK, MP3 supported" );
  }

  Log::print( "Linking FAAD library '%s' ...", libFaadName );

  libFaad = SDL_LoadObject( libFaadName );

  if( libFaad == nullptr ) {
    liber.mapAACs = false;

    Log::printEnd( " Not found, AAC not supported" );
  }
  else {
    OZ_DLLOAD( libFaad, NeAACDecInit   );
    OZ_DLLOAD( libFaad, NeAACDecOpen   );
    OZ_DLLOAD( libFaad, NeAACDecClose  );
    OZ_DLLOAD( libFaad, NeAACDecDecode );

    liber.mapAACs = true;

    Log::printEnd( " OK, AAC supported" );
  }
#endif
}

Sound sound;

}
}
