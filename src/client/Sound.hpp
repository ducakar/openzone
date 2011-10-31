/*
 *  Sound.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Orbis.hpp"
#include "matrix/Library.hpp"

#include "client/Context.hpp"

#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

namespace oz
{
namespace client
{

class Sound
{
  private:

    static const float DMAX;
    static const float DMAX_SQ;

    static const int DEFAULT_FREQUENCY = 44100;
    static const int MUSIC_BUFFER_SIZE = 64 * 1024;

    ALCdevice      *soundDevice;
    ALCcontext     *soundContext;

    Bitset         playedStructs;
    float          volume;

    OggVorbis_File oggStream;
    vorbis_info*   vorbisInfo;

    int            musicFormat;
    uint           musicBuffers[2];
    uint           musicSource;

    int            preferredFreq;

    char           musicBuffer[MUSIC_BUFFER_SIZE];

    // music track id to switch to, -1 to do nothing, -2 stop playing
    int            selectedTrack;
    // music track id, -1 for not playing
    int            currentTrack;

    void playCell( int cellX, int cellY );
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
