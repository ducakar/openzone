/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file client/Loader.cpp
 */

#include "stable.hpp"

#include "client/Loader.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Terra.hpp"
#include "client/Caelum.hpp"
#include "client/SMM.hpp"
#include "client/MD2.hpp"
#include "client/MD3.hpp"

#include "client/OpenGL.hpp"
#include "client/OpenAL.hpp"

#include <ctime>

namespace oz
{
namespace client
{

Loader loader;

void Loader::cleanupRender()
{
  OZ_GL_CHECK_ERROR();

  // delete imagines of removed objects
  for( auto i = context.imagines.citer(); i.isValid(); ) {
    auto imago = i;
    ++i;

    // We can afford to do this as orbis.objects[key] will remain null at least one whole tick
    // after the object has been removed (because matrix also needs to clear references to this
    // object).
    if( orbis.objects[ imago.key() ] == null ) {
      delete imago.value();
      context.imagines.exclude( imago.key() );
    }
  }

  if( tick % IMAGO_CLEAR_INTERVAL == IMAGO_CLEAR_LAG ) {
    // remove unused models
    for( auto i = context.imagines.citer(); i.isValid(); ) {
      auto imago = i;
      ++i;

      if( imago.value()->flags & Imago::UPDATED_BIT ) {
        imago.value()->flags &= ~Imago::UPDATED_BIT;
      }
      else {
        delete imago.value();
        context.imagines.exclude( imago.key() );
      }
    }
  }

  if( tick % FRAG_CLEAR_INTERVAL == FRAG_CLEAR_LAG ) {
    // remove unused frag pools
    for( auto i = context.fragPools.citer(); i.isValid(); ) {
      auto pool = i;
      ++i;

      if( pool.value()->flags & FragPool::UPDATED_BIT ) {
        pool.value()->flags &= ~FragPool::UPDATED_BIT;
      }
      else {
        delete pool.value();
        context.fragPools.exclude( pool.key() );
      }
    }
  }

  if( tick % IMAGOCLASS_CLEAR_INTERVAL == IMAGOCLASS_CLEAR_LAG ) {
    for( int i = 0; i < library.models.length(); ++i ) {
      Context::Resource<SMM*>& smm = context.smms[i];
      Context::Resource<MD2*>& md2 = context.md2s[i];
      Context::Resource<MD3*>& md3 = context.md3s[i];

      if( smm.object != null && smm.nUsers == 0 ) {
        delete smm.object;
        smm.object = null;
      }
      if( md2.object != null && md2.nUsers == 0 ) {
        delete md2.object;
        md2.object = null;
      }
      if( md3.object != null && md3.nUsers == 0 ) {
        delete md3.object;
        md3.object = null;
      }
    }
  }

  if( tick % BSP_CLEAR_INTERVAL == BSP_CLEAR_LAG ) {
    // remove unused BSPs
    for( int i = 0; i < library.nBSPs; ++i ) {
      Context::Resource<BSP*>& bsp = context.bsps[i];

      if( bsp.nUsers != 0 ) {
        bsp.nUsers = 0;
      }
      else {
        delete bsp.object;
        bsp.object = null;
      }
    }
  }

  OZ_GL_CHECK_ERROR();
}

void Loader::cleanupSound()
{
  OZ_AL_CHECK_ERROR();

  // remove sones of removed objects
  for( auto i = context.audios.citer(); i.isValid(); ) {
    auto audio = i;
    ++i;

    // We can afford to do this as orbis.objects[key] will remain null at least one whole tick
    // after the object has been removed (because matrix also needs to clear references to this
    // object).
    if( orbis.objects[ audio.key() ] == null ) {
      delete audio.value();
      context.audios.exclude( audio.key() );
    }
  }

  // remove continuous sounds that are not played any more
  for( auto i = context.bspSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    if( src.value().isUpdated ) {
      src.value().isUpdated = false;
    }
    else {
      alDeleteSources( 1, &src.value().id );
      context.removeBSPSource( &src.value(), src.key() );
    }
  }

  for( auto i = context.objSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    if( src.value().isUpdated ) {
      src.value().isUpdated = false;
    }
    else {
      alDeleteSources( 1, &src.value().id );
      context.removeObjSource( &src.value(), src.key() );
    }
  }

  if( tick % SOUND_CLEAR_INTERVAL == SOUND_CLEAR_LAG ) {
    for( int i = 0; i < library.sounds.length(); ++i ) {
      Context::Resource<uint>& sound = context.sounds[i];

      if( sound.nUsers == 0 ) {
        context.freeSound( i );
      }
    }
  }

  if( tick % SOURCE_CLEAR_INTERVAL == SOURCE_CLEAR_LAG ) {
    // remove stopped sources of non-continous sounds
    Context::Source* prev = null;
    Context::Source* src  = context.sources.first();

    while( src != null ) {
      Context::Source* next = src->next[0];

      ALint value;
      alGetSourcei( src->id, AL_SOURCE_STATE, &value );

      if( value != AL_PLAYING ) {
        alDeleteSources( 1, &src->id );
        context.removeSource( src, prev );
      }
      else {
        prev = src;
      }
      src = next;
    }
  }

  if( tick % AUDIO_CLEAR_INTERVAL == AUDIO_CLEAR_LAG ) {
    // remove unused Audio objects
    for( auto i = context.audios.citer(); i.isValid(); ) {
      auto audio = i;
      ++i;

      if( audio.value()->flags & Audio::UPDATED_BIT ) {
        audio.value()->flags &= ~Audio::UPDATED_BIT ;
      }
      else {
        delete audio.value();
        context.audios.exclude( audio.key() );
      }
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Loader::update()
{
  tick = ( tick + 1 ) % TICK_PERIOD;

  // terra
  if( terra.id != orbis.terra.id ) {
    terra.unload();
    terra.load();
  }

  // caelum
  if( caelum.id != orbis.caelum.id ) {
    caelum.unload();
    caelum.load();
  }

  // BSP
  for( int i = 0; i < library.nBSPs; ++i ) {
    BSP* bsp = context.bsps[i].object;

    if( bsp != null && !bsp->isLoaded ) {
      bsp->load();
    }
  }

  // SMM
  for( int i = 0; i < library.models.length(); ++i ) {
    SMM* smm = context.smms[i].object;

    if( smm != null && !smm->isLoaded ) {
      smm->load();
    }
  }

  // MD2
  for( int i = 0; i < library.models.length(); ++i ) {
    MD2* md2 = context.md2s[i].object;

    if( md2 != null && !md2->isLoaded ) {
      md2->load();
    }
  }

  // MD3
  for( int i = 0; i < library.models.length(); ++i ) {
    MD3* md3 = context.md3s[i].object;

    if( md3 != null && !md3->isLoaded ) {
      md3->load();
    }
  }
}

void Loader::cleanup()
{
  cleanupRender();
  cleanupSound();
}

void Loader::makeScreenshot()
{
  uint* pixels = new uint[camera.width * camera.height * 4];
  char fileName[256];
  time_t ct;
  struct tm t;

  ct = time( null );
  t = *localtime( &ct );

  snprintf( fileName, 256, "%s/screenshot %04d-%02d-%02d %02d:%02d:%02d.bmp",
            config.get( "dir.rc", "" ),
            1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );

  log.print( "Saving screenshot to '%s' ...", fileName );

  glReadPixels( 0, 0, camera.width, camera.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  SDL_Surface* surf = SDL_CreateRGBSurfaceFrom( pixels, camera.width, camera.height, 32,
                                                camera.width * 4,
                                                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
  // flip image
  for( int i = 0; i < camera.height / 2; ++i ) {
    for( int j = 0; j < camera.width; ++j ) {
      swap( pixels[i * camera.width + j],
            pixels[ ( camera.height - i - 1 ) * camera.width + j ] );
    }
  }
  SDL_SaveBMP( surf, fileName );
  SDL_FreeSurface( surf );
  delete[] pixels;

  log.printEnd( " OK" );
}

void Loader::init()
{
  tick = 0;
}

void Loader::free()
{}

}
}
