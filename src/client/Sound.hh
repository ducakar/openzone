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
 * @file client/Sound.hh
 */

#pragma once

#include "client/Context.hh"

#include "client/OpenAL.hh"

#include <AL/alc.h>
#include <physfs.h>
#include <vorbis/vorbisfile.h>
#include <mad.h>
#include <neaacdec.h>

namespace oz
{
namespace client
{

class Sound
{
  private:

    static const int   MUSIC_BUFFER_SIZE       = 64 * 1024;
    static const int   MUSIC_INPUT_BUFFER_SIZE = 8 * 1024;
    static const float MAX_DISTANCE;

    enum StreamType
    {
      NONE,
      OGG,
      MP3,
      AAC
    };

    ALCdevice*     soundDevice;
    ALCcontext*    soundContext;

    Bitset         playedStructs;
    float          volume;

    StreamType     musicStreamType;

    int            musicRate;
    int            musicChannels;
    int            musicFormat;
    uint           musicSource;
    uint           musicBufferIds[2];
    int            musicBuffersQueued;
    char           musicBuffer[MUSIC_BUFFER_SIZE];
    ubyte          musicInputBuffer[MUSIC_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];

    void*          libmad;
    void*          libfaad;

    PHYSFS_File*   musicFile;

    OggVorbis_File oggStream;

    mad_stream     madStream;
    mad_frame      madFrame;
    mad_synth      madSynth;

    int            madWrittenSamples;
    int            madFrameSamples;

    NeAACDecHandle aacDecoder;

    char*          aacOutputBuffer;
    int            aacWrittenBytes;
    int            aacBufferBytes;
    size_t         aacInputBytes;

    // music track id to switch to, -1 to do nothing, -2 stop playing
    int            selectedTrack;
    // music track id, -1 for not playing
    volatile int   currentTrack;

    int            streamedTrack;
    volatile int   streamedBytes;

    SDL_Thread*    musicThread;
    SDL_Thread*    soundThread;

    SDL_sem*       musicMainSemaphore;
    SDL_sem*       musicAuxSemaphore;
    SDL_sem*       soundMainSemaphore;
    SDL_sem*       soundAuxSemaphore;

    volatile bool  isMusicAlive;
    volatile bool  isSoundAlive;

    static int musicMain( void* );
    static int soundMain( void* );

    void musicOpen( const char* path );
    void musicClear();
    int  musicDecode();
    void musicRun();

    void playCell( int cellX, int cellY );
    void updateMusic();
    void soundRun();

  public:

    void setVolume( float volume );
    void setMusicVolume( float volume ) const;

    void playMusic( int track );
    void stopMusic();
    bool isMusicPlaying() const;

    void resume() const;
    void suspend() const;

    void play();
    void sync();

    void init();
    void free();

};

extern Sound sound;

}
}
