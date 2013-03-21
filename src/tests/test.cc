/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file tests/test.cc
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>
#include <AL/alc.h>

using namespace oz;

int main( int argc, char** argv )
{
  System::init();

  const char* sample = argc != 1 ? argv[1] : "/usr/share/sounds/pop.wav";

  File::initVFS();
  File::mount( "/etc/", "", false );

  File fstab( "@fstab" );
  Buffer buf = fstab.read();

  Log() << fstab.realPath() << "\n";
  Log() << buf.begin() << "\n";

  ALCdevice*  device  = alcOpenDevice( "" );
  ALCcontext* context = alcCreateContext( device, nullptr );

  alcMakeContextCurrent( context );

  ALBuffer buffer( sample );
  ALSource source( buffer );

  alSourcePlay( source.id() );

  int state;
  do {
    Time::sleep( 100 );
    alGetSourcei( source.id(), AL_SOURCE_STATE, &state );
  }
  while( state == AL_PLAYING );

  source.destroy();
  buffer.destroy();

  alcDestroyContext( context );
  alcCloseDevice( device );
  return 0;
}
