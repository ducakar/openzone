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
#include "client/NaCl.hh"

#if PHYSFS_VER_MAJOR == 2 && PHYSFS_VER_MINOR == 0
# define PHYSFS_readBytes( handle, buffer, len ) PHYSFS_read( handle, buffer, 1, uint( len ) )
#endif

#ifdef OZ_NONFREE
# define OZ_DLDECL( name ) \
  static decltype( ::name )* name = null

# define OZ_DLLOAD( l, name ) \
  *( void** )( &name ) = SDL_LoadFunction( l, #name ); \
  if( name == null ) { \
    throw Exception( "Failed loading " #name " from libmad" ); \
  }
#endif

namespace oz
{
namespace client
{

Sound sound;

const float Sound::MAX_DISTANCE = 160.0f;

#ifdef OZ_NONFREE

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

#endif

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle );
static ov_callbacks VORBIS_CALLBACKS = { vorbisRead, null, null, null };

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle )
{
  return size_t( PHYSFS_readBytes( static_cast<PHYSFS_File*>( handle ), buffer,
                                   ulong64( size * n ) ) );
}

#ifdef OZ_NONFREE

inline short madFixedToShort( mad_fixed_t f )
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

#endif

void Sound::musicMain( void* )
{
  try {
    sound.musicRun();
  }
  catch( const std::exception& e ) {
    System::error( e );
  }
}

void Sound::soundMain( void* )
{
  try {
    sound.soundRun();
  }
  catch( const std::exception& e ) {
    System::error( e );
  }
}

void Sound::musicOpen( const char* path )
{
  PFile file( path );

  if( file.hasExtension( "oga" ) || file.hasExtension( "ogg" ) ) {
    musicStreamType = OGG;
  }
#ifdef OZ_NONFREE
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
#endif
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
        throw Exception( "Failed to open Ogg stream in '%s'", path );
      }

      vorbis_info* vorbisInfo = ov_info( &oggStream, -1 );
      if( vorbisInfo == null ) {
        throw Exception( "Corrupted Vorbis header in '%s'", path );
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
        throw Exception( "Invalid number of channels in '%s', should be 1 or 2", path );
      }

      break;
    }
#ifdef OZ_NONFREE
    case MP3: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == null ) {
        throw Exception( "Failed to open file '%s'", path );
      }

      mad_stream_init( &madStream );
      mad_frame_init( &madFrame );
      mad_synth_init( &madSynth );

      size_t readSize = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer,
                                                  ulong64( MUSIC_INPUT_BUFFER_SIZE ) ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        throw Exception( "Failed to read MP3 stream in '%s'", path );
      }

      mad_stream_buffer( &madStream, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE );

      while( mad_frame_decode( &madFrame, &madStream ) != 0 ) {
        if( !MAD_RECOVERABLE( madStream.error ) ) {
          throw Exception( "Corrupted MP3 header in '%s'", path );
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
        throw Exception( "Invalid number of channels in '%s', should be 1 or 2", path );
      }

      break;
    }
    case AAC: {
      musicFile = PHYSFS_openRead( path );
      if( musicFile == null ) {
        throw Exception( "Failed to open file '%s'", path );
      }

      aacDecoder = NeAACDecOpen();

      size_t readSize = size_t( PHYSFS_readBytes( musicFile, musicInputBuffer,
                                                  ulong64( MUSIC_INPUT_BUFFER_SIZE ) ) );
      if( readSize != size_t( MUSIC_INPUT_BUFFER_SIZE ) ) {
        throw Exception( "Failed to read AAC stream in '%s'", path );
      }

      ulong aacRate;
      ubyte aacChannels;

      long skipBytes = NeAACDecInit( aacDecoder, musicInputBuffer, MUSIC_INPUT_BUFFER_SIZE,
                                     &aacRate, &aacChannels );
      if( skipBytes < 0 ) {
        throw Exception( "Corrupted AAC header in '%s'", path );
      }

      memmove( musicInputBuffer, musicInputBuffer + skipBytes, size_t( skipBytes ) );

      readSize = size_t( PHYSFS_readBytes( musicFile,
                                           musicInputBuffer + MUSIC_INPUT_BUFFER_SIZE - skipBytes,
                                           ulong64( skipBytes ) ) );

      if( readSize != size_t( skipBytes ) ) {
        throw Exception( "Failed to read AAC stream in '%s'", path );
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
        throw Exception( "Invalid number of channels in '%s', should be 1 or 2", path );
      }

      break;
    }
#else
    case MP3:
    case AAC: {
      break;
    }
#endif
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
#ifdef OZ_NONFREE
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
#else
    case MP3:
    case AAC: {
      break;
    }
#endif
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
          throw Exception( "Error during Ogg Vorbis decoding of '%s'",
                           library.musicTracks[streamedTrack].path.cstr() );
        }
      }
      while( result > 0 && bytesRead < MUSIC_BUFFER_SIZE );

      return bytesRead;
    }
#ifdef OZ_NONFREE
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
            throw Exception( "Unrecoverable error during MP3 decoding of '%s'",
                             library.musicTracks[streamedTrack].path.cstr() );
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

        if( aacOutputBuffer == null ) {
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
#else
    case MP3:
    case AAC: {
      return 0;
    }
#endif
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
        musicOpen( library.musicTracks[streamedTrack].path );
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

  foreach( obj, cell.objects.citer() ) {
    if( obj->flags & Object::AUDIO_BIT ) {
      if( ( camera.p - obj->p ).sqN() < MAX_DISTANCE*MAX_DISTANCE ) {
        context.playAudio( obj, null );
      }
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
    float orientation[] = {
      camera.at.x, camera.at.y, camera.at.z,
      camera.up.x, camera.up.y, camera.up.z
    };

    // add new sounds
    alListenerfv( AL_ORIENTATION, orientation );
    alListenerfv( AL_POSITION, camera.p );
    alListenerfv( AL_VELOCITY, camera.velocity );

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

    soundMainSemaphore.post();
    soundAuxSemaphore.wait();
  }
}

void Sound::setVolume( float volume_ )
{
  volume = volume_;
  alListenerf( AL_GAIN, volume_ );
}

void Sound::setMusicVolume( float volume ) const
{
  alSourcef( musicSource, AL_GAIN, 0.5f * volume );
}

bool Sound::isMusicPlaying() const
{
  return currentTrack >= 0 || selectedTrack >= 0;
}

int Sound::getCurrentTrack() const
{
  return currentTrack;
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
  soundAuxSemaphore.post();
}

void Sound::sync()
{
  soundMainSemaphore.wait();
}

void Sound::init()
{
#ifdef __native_client__
  hard_assert( NaCl::isMainThread() );
#endif

  Log::println( "Initialising Sound {" );
  Log::indent();

  const char* deviceSpec = alcGetString( null, ALC_DEVICE_SPECIFIER );

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
    deviceName = null;
    Log::print( "Initialising default device ..." );
  }
  else {
    Log::print( "Initialising device '%s' ...", deviceName );
  }

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

  Log::printEnd( " OK" );

  OZ_AL_CHECK_ERROR();

  Log::println( "OpenAL device: %s", alcGetString( soundDevice, ALC_DEVICE_SPECIFIER ) );

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

  selectedTrack = -1;
  currentTrack  = -1;
  streamedTrack = -1;

  alGenBuffers( 2, musicBufferIds );
  alGenSources( 1, &musicSource );

  musicBuffersQueued = 0;

  alSourcei( musicSource, AL_SOURCE_RELATIVE, AL_TRUE );

  setVolume( config.include( "sound.volume", 1.0f ).asFloat() );
  setMusicVolume( 0.5f );

#ifdef OZ_NONFREE

# ifdef _WIN32
  libmad  = SDL_LoadObject( "libmad.dll" );
  libfaad = SDL_LoadObject( "libfaad2.dll" );
# else
  libmad  = SDL_LoadObject( "libmad.so.0" );
  libfaad = SDL_LoadObject( "libfaad.so.2" );
# endif

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

#endif

  isMusicAlive = true;
  isSoundAlive = true;

  musicMainSemaphore.init();
  musicAuxSemaphore.init();
  soundMainSemaphore.init();
  soundAuxSemaphore.init();

  musicThread.start( musicMain );
  soundThread.start( soundMain );

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

#ifdef OZ_NONFREE
  if( libfaad != null ) {
    SDL_UnloadObject( libfaad );
  }
  if( libmad != null ) {
    SDL_UnloadObject( libmad );
  }
#endif

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

  Log::printEnd( " OK" );
}

}
}
