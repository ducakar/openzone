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
 * @file client/Loader.cc
 */

#include <client/Loader.hh>

#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/Terra.hh>
#include <client/Caelum.hh>

#include <cstdio>

namespace oz
{
namespace client
{

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

    // We can afford to do this as orbis.objects[key] will remain nullptr at least one whole tick
    // after the object has been removed (because matrix also needs to clear references to this
    // object).
    if( orbis.obj( imago->key ) == nullptr ) {
      delete imago->value;
      context.imagines.exclude( imago->key );
    }
  }

  if( tick % IMAGO_CLEAR_INTERVAL == IMAGO_CLEAR_LAG ) {
    // remove unused models
    for( auto i = context.imagines.citer(); i.isValid(); ) {
      auto imago = i;
      ++i;

      if( imago->value->flags & Imago::UPDATED_BIT ) {
        imago->value->flags &= ~Imago::UPDATED_BIT;
      }
      else {
        delete imago->value;
        context.imagines.exclude( imago->key );
      }
    }
  }

  if( tick % FRAG_CLEAR_INTERVAL == FRAG_CLEAR_LAG ) {
    // remove unused frag pools
    for( int i = 0; i < liber.nFragPools; ++i ) {
      FragPool* pool = context.fragPools[i];

      if( pool == nullptr ) {
        continue;
      }

      if( pool->flags & FragPool::UPDATED_BIT ) {
        pool->flags &= ~FragPool::UPDATED_BIT;
      }
      else {
        delete pool;
        context.fragPools[i] = nullptr;
      }
    }
  }

  if( tick % BSP_CLEAR_INTERVAL == BSP_CLEAR_LAG ) {
    // remove unused BSPs
    for( int i = 0; i < liber.nBSPs; ++i ) {
      Context::Resource<BSP*>& bsp = context.bsps[i];

      if( bsp.nUsers != 0 ) {
        bsp.nUsers = 0;
      }
      else {
        delete bsp.handle;

        bsp.handle = nullptr;
        bsp.nUsers = -1;
      }
    }
  }

  if( tick % MODEL_CLEAR_INTERVAL == MODEL_CLEAR_LAG ) {
    for( int i = 0; i < liber.models.length(); ++i ) {
      Context::Resource<Model*>& model = context.models[i];

      if( model.nUsers == 0 ) {
        delete model.handle;

        model.handle = nullptr;
        model.nUsers = -1;
      }
    }
  }

  if( tick % PARTICLE_CLEAR_INTERVAL == PARTICLE_CLEAR_LAG ) {
    for( int i = 0; i < liber.parts.length(); ++i ) {
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

    // We can afford to do this as orbis.objects[key] will remain nullptr at least one whole tick after
    // the object has been removed (because matrix also needs to clear references to this object).
    if( orbis.obj( audio->key ) == nullptr ) {
      delete audio->value;
      context.audios.exclude( audio->key );
    }
  }

  if( tick % BSPAUDIO_CLEAR_INTERVAL == BSPAUDIO_CLEAR_LAG ) {
    // remove unused BSPAudios
    for( int i = 0; i < liber.nBSPs; ++i ) {
      Context::Resource<BSPAudio*>& bspAudio = context.bspAudios[i];

      if( bspAudio.nUsers != 0 ) {
        bspAudio.nUsers = 0;
      }
      else {
        delete bspAudio.handle;

        bspAudio.handle = nullptr;
        bspAudio.nUsers = -1;
      }
    }
  }

  if( tick % AUDIO_CLEAR_INTERVAL == AUDIO_CLEAR_LAG ) {
    // remove unused Audio objects
    for( auto i = context.audios.citer(); i.isValid(); ) {
      auto audio = i;
      ++i;

      if( audio->value->flags & Audio::UPDATED_BIT ) {
        audio->value->flags &= ~Audio::UPDATED_BIT ;
      }
      else {
        delete audio->value;
        context.audios.exclude( audio->key );
      }
    }
  }

  if( tick % SOURCE_CLEAR_INTERVAL == SOURCE_CLEAR_LAG ) {
    // remove stopped sources of non-continous sounds
    Context::Source* prev = nullptr;
    Context::Source* src  = context.sources.first();

    while( src != nullptr ) {
      Context::Source* next = src->next[0];

      ALint value;
      alGetSourcei( src->id, AL_SOURCE_STATE, &value );

      if( value != AL_PLAYING ) {
        context.removeSource( src, prev );
      }
      else {
        prev = src;
      }
      src = next;
    }
  }

  // Remove continuous sounds that are not played any more.
  for( auto i = context.contSources.iter(); i.isValid(); ) {
    auto src = i;
    ++i;

    if( src->value.isUpdated ) {
      src->value.isUpdated = false;
    }
    else {
      context.removeContSource( &src->value, src->key );
    }
  }

  int speaker = context.speakSource.owner;
  if( speaker < 0 ) {
    if( context.speakSource.thread.isValid() ) {
      context.releaseSpeakSource();
    }
  }
  else if( orbis.obj( speaker ) == nullptr ) {
    context.speakSource.isAlive = false;
  }

  if( tick % SOUND_CLEAR_INTERVAL == SOUND_CLEAR_LAG ) {
    for( int i = 0; i < liber.sounds.length(); ++i ) {
      Context::Resource<uint>& sound = context.sounds[i];

      if( sound.nUsers == 0 ) {
        context.freeSound( i );
      }
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Loader::preloadRender()
{
  for( int i = 0; i < liber.nBSPs; ++i ) {
    BSP* bsp = context.bsps[i].handle;

    if( bsp != nullptr && !bsp->isPreloaded() ) {
      bsp->preload();
    }
  }

  for( int i = 0; i < liber.models.length(); ++i ) {
    Model* model = context.models[i].handle;

    if( model != nullptr && !model->isPreloaded() ) {
      model->preload();
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

  for( int i = 0; i < liber.nBSPs; ++i ) {
    BSP* bsp = context.bsps[i].handle;

    if( bsp != nullptr && !bsp->isLoaded() && bsp->isPreloaded() ) {
      bsp->load();
      return;
    }
  }

  for( int i = 0; i < liber.models.length(); ++i ) {
    Model* model = context.models[i].handle;

    if( model != nullptr && !model->isLoaded() && model->isPreloaded() ) {
      model->load();
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
  const char* picturesDir = config["dir.pictures"].get( File::PICTURES );

  File::mkdir( picturesDir );

  char path[256];
  snprintf( path, 256, "%s/OpenZone %s.png", picturesDir, Time::local().toString().cstr() );

  Log::println( "Screenshot to '%s' scheduled in background thread", path );
  Window::screenshot( path );
}

void Loader::syncUpdate()
{
  preloadRender();
  loader.uploadRender();
}

void Loader::update()
{
  cleanupSound();

  if( !preloadMainSemaphore.tryWait() ) {
    return;
  }

  loader.cleanupRender();
  loader.uploadRender();

  tick = ( tick + 1 ) % TICK_PERIOD;

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

  preloadThread.start( "preload", Thread::JOINABLE, preloadMain );
}

void Loader::destroy()
{
  isPreloadAlive = false;

  preloadAuxSemaphore.post();
  preloadThread.join();

  preloadAuxSemaphore.destroy();
  preloadMainSemaphore.destroy();
}

Loader loader;

}
}
