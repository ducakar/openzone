/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file client/Loader.cc
 */

#include "stable.hh"

#include "client/Loader.hh"

#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Terra.hh"
#include "client/Caelum.hh"
#include "client/SMM.hh"
#include "client/MD2.hh"
#include "client/MD3.hh"

#include "client/OpenGL.hh"
#include "client/OpenAL.hh"

#include <ctime>
#include <IL/il.h>

namespace oz
{
namespace client
{

Loader loader;

Loader::ScreenshotInfo Loader::screenshotInfo;

int Loader::saveScreenshot( void* )
{
  // flip image
  char* top    = screenshotInfo.pixels;
  char* bottom = screenshotInfo.pixels + ( camera.height - 1 ) * camera.width * 3;

  for( int i = 0; i < screenshotInfo.height / 2; ++i ) {
    for( int j = 0; j < screenshotInfo.width; ++j ) {
      swap( top[0], bottom[0] );
      swap( top[1], bottom[1] );
      swap( top[2], bottom[2] );

      top    += 3;
      bottom += 3;
    }

    bottom -= 2 * screenshotInfo.width * 3;
  }

  uint image = ilGenImage();
  ilBindImage( image );

  ilLoadDataL( screenshotInfo.pixels, uint( screenshotInfo.width * screenshotInfo.height * 3 ),
               uint( screenshotInfo.width ), uint( screenshotInfo.height ), 1, 3 );

  ilSave( IL_PNG, screenshotInfo.path );
  ilDeleteImage( image );

  delete[] screenshotInfo.pixels;

  return 0;
}

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
  if( shotThread != null ) {
    SDL_WaitThread( shotThread, null );
    shotThread = null;
  }

  time_t currentTime = std::time( null );
  struct tm timeStruct = *std::localtime( &currentTime );

  snprintf( screenshotInfo.path, 256,
            "%s/screenshots/" OZ_APPLICATION_NAME " %04d-%02d-%02d %02d:%02d:%02d.png",
            config.get( "dir.config", "" ),
            1900 + timeStruct.tm_year, 1 + timeStruct.tm_mon, timeStruct.tm_mday,
            timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec );

  log.println( "Screenshot to '%s' scheduled in background thread", screenshotInfo.path );

  screenshotInfo.width  = camera.width;
  screenshotInfo.height = camera.height;
  screenshotInfo.pixels = new char[camera.width * camera.height * 3];

  glReadPixels( 0, 0, camera.width, camera.height, GL_RGB, GL_UNSIGNED_BYTE, screenshotInfo.pixels );

  shotThread = SDL_CreateThread( saveScreenshot, null );
}

void Loader::init()
{
  shotThread = null;
  tick = 0;
}

void Loader::free()
{
  if( shotThread != null ) {
    SDL_WaitThread( shotThread, null );
    shotThread = null;
  }
}

}
}
