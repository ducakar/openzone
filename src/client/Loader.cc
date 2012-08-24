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

namespace oz
{
namespace client
{

Loader loader;

Loader::ScreenshotInfo Loader::screenshotInfo;

void Loader::screenshotMain( void* )
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

  SDL_Surface* surf = SDL_CreateRGBSurfaceFrom( screenshotInfo.pixels, screenshotInfo.width,
                                                screenshotInfo.height, 24, screenshotInfo.width * 3,
                                                0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 );

  SDL_SaveBMP( surf, screenshotInfo.path );
  SDL_FreeSurface( surf );

  delete[] screenshotInfo.pixels;
}

void Loader::preloadMain( void* )
{
  loader.preloadRun();
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
    for( int i = 0; i < library.nFragPools; ++i ) {
      FragPool* pool = context.fragPools[i];

      if( pool == null ) {
        continue;
      }

      if( pool->flags & FragPool::UPDATED_BIT ) {
        pool->flags &= ~FragPool::UPDATED_BIT;
      }
      else {
        delete pool;
        context.fragPools[i] = null;
      }
    }
  }

  if( tick % IMAGOCLASS_CLEAR_INTERVAL == IMAGOCLASS_CLEAR_LAG ) {
    for( int i = 0; i < library.models.length(); ++i ) {
      Context::Resource<SMM*>& smm = context.smms[i];
      Context::Resource<MD2*>& md2 = context.md2s[i];
      Context::Resource<MD3*>& md3 = context.md3s[i];

      if( smm.nUsers == 0 ) {
        delete smm.object;

        smm.object = null;
        smm.nUsers = -1;
      }
      if( md2.nUsers == 0 ) {
        delete md2.object;

        md2.object = null;
        md2.nUsers = -1;
      }
      if( md3.nUsers == 0 ) {
        delete md3.object;

        md3.object = null;
        md3.nUsers = -1;
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
        bsp.nUsers = -1;
      }
    }
  }

  OZ_GL_CHECK_ERROR();
}

void Loader::cleanupSound()
{
  OZ_AL_CHECK_ERROR();

  // remove audios of removed objects
  for( auto i = context.audios.citer(); i.isValid(); ) {
    auto audio = i;
    ++i;

    // We can afford to do this as orbis.objects[key] will remain null at least one whole tick after
    // the object has been removed (because matrix also needs to clear references to this object).
    if( orbis.objects[ audio.key() ] == null ) {
      delete audio.value();
      context.audios.exclude( audio.key() );
    }
  }

  // Remove continuous sounds that are not played any more.
  for( auto i = context.contSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    if( src.value().isUpdated ) {
      src.value().isUpdated = false;
    }
    else {
      alDeleteSources( 1, &src.value().id );
      context.removeContSource( &src.value(), src.key() );
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

  if( tick % BSPAUDIO_CLEAR_INTERVAL == BSPAUDIO_CLEAR_LAG ) {
    // remove unused BSPAudios
    for( int i = 0; i < library.nBSPs; ++i ) {
      Context::Resource<BSPAudio*>& bspAudio = context.bspAudios[i];

      if( bspAudio.nUsers != 0 ) {
        bspAudio.nUsers = 0;
      }
      else {
        delete bspAudio.object;

        bspAudio.object = null;
        bspAudio.nUsers = -1;
      }
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Loader::preloadRender()
{
  for( int i = 0; i < library.nBSPs; ++i ) {
    BSP* bsp = context.bsps[i].object;

    if( bsp != null && !bsp->isPreloaded ) {
      bsp->preload();
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    SMM* smm = context.smms[i].object;

    if( smm != null && !smm->isPreloaded ) {
      smm->preload();
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    MD2* md2 = context.md2s[i].object;

    if( md2 != null && !md2->isPreloaded ) {
      md2->preload();
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    MD3* md3 = context.md3s[i].object;

    if( md3 != null && !md3->isPreloaded ) {
      md3->preload();
    }
  }
}

void Loader::uploadRender()
{
  if( caelum.id != orbis.caelum.id ) {
    caelum.unload();
    caelum.load();
  }

  if( terra.id != orbis.terra.id ) {
    terra.unload();
    terra.load();
  }

  for( int i = 0; i < library.nBSPs; ++i ) {
    BSP* bsp = context.bsps[i].object;

    if( bsp != null && !bsp->isLoaded && bsp->isPreloaded ) {
      bsp->load();
      return;
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    SMM* smm = context.smms[i].object;

    if( smm != null && !smm->isLoaded && smm->isPreloaded ) {
      smm->load();
      return;
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    MD2* md2 = context.md2s[i].object;

    if( md2 != null && !md2->isLoaded && md2->isPreloaded ) {
      md2->load();
      return;
    }
  }

  for( int i = 0; i < library.models.length(); ++i ) {
    MD3* md3 = context.md3s[i].object;

    if( md3 != null && !md3->isLoaded && md3->isPreloaded ) {
      md3->load();
      return;
    }
  }
}

void Loader::preloadRun()
{
  preloadAuxSemaphore.wait();

  while( isPreloadAlive ) {
    preloadRender();

    preloadMainSemaphore.post();
    preloadAuxSemaphore.wait();
  }
}

void Loader::makeScreenshot()
{
  if( screenshotThread.isValid() ) {
    screenshotThread.join();
  }

  snprintf( screenshotInfo.path, 256, "%s/screenshots/OpenZone %s.bmp",
            config["dir.config"].asString().cstr(), Time::local().toString().cstr() );

  Log::println( "Screenshot to '%s' scheduled in background thread", screenshotInfo.path );

  screenshotInfo.width  = camera.width;
  screenshotInfo.height = camera.height;
  screenshotInfo.pixels = new char[camera.width * camera.height * 3];

  glReadPixels( 0, 0, camera.width, camera.height, GL_RGB, GL_UNSIGNED_BYTE, screenshotInfo.pixels );

  screenshotThread.start( screenshotMain, null );
}

void Loader::syncUpdate()
{
  Log::verboseMode = true;

  preloadRender();
  OZ_MAIN_CALL( this, {
    loader.uploadRender();
  } )

  Log::verboseMode = false;
}

void Loader::update()
{
  Log::verboseMode = true;

  cleanupSound();

  Log::verboseMode = false;

  if( !preloadMainSemaphore.tryWait() ) {
    return;
  }

  Log::verboseMode = true;

  OZ_MAIN_CALL( this, {
    loader.cleanupRender();
    loader.uploadRender();
  } )

  tick = ( tick + 1 ) % TICK_PERIOD;

  Log::verboseMode = false;

  preloadAuxSemaphore.post();
}

void Loader::load()
{
  tick = 0;

  preloadAuxSemaphore.post();
}

void Loader::unload()
{
  preloadMainSemaphore.wait();
}

void Loader::init()
{
  isPreloadAlive = true;

  preloadMainSemaphore.init();
  preloadAuxSemaphore.init();

  preloadThread.start( preloadMain, null );
}

void Loader::free()
{
  isPreloadAlive = false;

  preloadAuxSemaphore.post();
  preloadThread.join();

  preloadAuxSemaphore.destroy();
  preloadMainSemaphore.destroy();

  if( screenshotThread.isValid() ) {
    screenshotThread.join();
  }
}

}
}
