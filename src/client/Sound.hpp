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
 * @file client/Sound.hpp
 */

#pragma once

#include "matrix/Orbis.hpp"
#include "matrix/Library.hpp"

#include "client/Context.hpp"

#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include <mad.h>

namespace oz
{
namespace client
{

class Sound
{
  private:

    static const float MAX_DISTANCE;

    static const int DEFAULT_FREQUENCY = 44100;
    static const int MUSIC_BUFFER_SIZE = 64 * 1024;
    static const int MAD_INPUT_BUFFER_SIZE  = 16 * 1024;

    enum StreamType
    {
      NONE,
      OGG,
      MP3
    };

    ALCdevice*     soundDevice;
    ALCcontext*    soundContext;

    Bitset         playedStructs;
    float          volume;

    StreamType     musicStreamType;

    int            musicRate;
    int            musicChannels;
    int            musicFormat;
    uint           musicBuffers[2];
    uint           musicSource;

    char           musicBuffer[MUSIC_BUFFER_SIZE];

    OggVorbis_File oggStream;

    void*          libmad;

    FILE*          mp3File;

    mad_stream     madStream;
    mad_frame      madFrame;
    mad_synth      madSynth;

    int            madWrittenSamples;
    int            madFrameSamples;
    ubyte          madInputBuffer[MAD_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];

    // music track id to switch to, -1 to do nothing, -2 stop playing
    int            selectedTrack;
    // music track id, -1 for not playing
    int            currentTrack;

    bool           enableMP3;

    void playCell( int cellX, int cellY );

    static short madFixedToShort( mad_fixed_t f );

    void streamOpen( const char* path );
    void streamClear();
    int  streamDecode();

    bool loadMusicBuffer( uint buffer );

  public:

    void setVolume( float volume );
    void setMusicVolume( float volume ) const;

    void playMusic( int track );
    void stopMusic();
    bool isMusicPlaying() const;

    void resume() const;
    void suspend() const;

    void play();
    void update();

    void init();
    void free();

};

extern Sound sound;

}
}
