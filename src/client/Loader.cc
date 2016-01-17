/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <client/Loader.hh>

#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/Terra.hh>
#include <client/Caelum.hh>

namespace oz
{
namespace client
{

void Loader::preloadMain(void*)
{
  loader.preloadRun();
}

void Loader::updateRender()
{
  OZ_GL_CHECK_ERROR();

  // Delete imagines of removed objects.
  for (auto i = context.imagines.citerator(); i.isValid();) {
    auto imago = i;
    ++i;

    // We can afford to do this as orbis.objects[key] will remain nullptr at least one whole tick
    // after the object has been removed because matrix also needs to clear references.
    if (orbis.obj(imago->key) == nullptr) {
      delete imago->value;
      context.imagines.exclude(imago->key);
    }
  }

  // Remove unused imagines.
  if (tick % IMAGO_CLEAR_INTERVAL == IMAGO_CLEAR_LAG) {
    for (auto i = context.imagines.citerator(); i.isValid();) {
      auto imago = i;
      ++i;

      if (imago->value->flags & Imago::UPDATED_BIT) {
        imago->value->flags &= ~Imago::UPDATED_BIT;
      }
      else {
        delete imago->value;
        context.imagines.exclude(imago->key);
      }
    }
  }

  OZ_GL_CHECK_ERROR();
}

void Loader::updateSound()
{
  OZ_AL_CHECK_ERROR();

  // Remove audios of removed objects.
  for (auto i = context.audios.citerator(); i.isValid();) {
    auto audio = i;
    ++i;

    // We can afford to do this as orbis.objects[key] will remain nullptr at least one whole tick
    // after the object has been removed because matrix also needs to clear references.
    if (orbis.obj(audio->key) == nullptr) {
      delete audio->value;
      context.audios.exclude(audio->key);
    }
  }

  // Remove unused object Audio objects.
  if (tick % AUDIO_CLEAR_INTERVAL == AUDIO_CLEAR_LAG) {
    for (auto i = context.audios.citerator(); i.isValid();) {
      auto audio = i;
      ++i;

      if (audio->value->flags & Audio::UPDATED_BIT) {
        audio->value->flags &= ~Audio::UPDATED_BIT ;
      }
      else {
        delete audio->value;
        context.audios.exclude(audio->key);
      }
    }
  }

  // Remove stopped sources of non-continous sounds.
  if (tick % SOURCE_CLEAR_INTERVAL == SOURCE_CLEAR_LAG) {
    Context::Source* prev = nullptr;
    Context::Source* src  = context.sources.first();

    while (src != nullptr) {
      Context::Source* next = src->next[0];

      ALint value;
      alGetSourcei(src->id, AL_SOURCE_STATE, &value);

      if (value != AL_PLAYING) {
        context.removeSource(src, prev);
      }
      else {
        prev = src;
      }
      src = next;
    }
  }

  // Remove continuous sources that are not played any more.
  for (auto i = context.contSources.iterator(); i.isValid();) {
    auto src = i;
    ++i;

    if (src->value.isUpdated) {
      src->value.isUpdated = false;
    }
    else {
      context.removeContSource(&src->value, src->key);
    }
  }

  // Stop speaker if owner has been removed.
  int speaker = context.speakSource.owner;
  if (speaker < 0) {
    if (context.speakSource.thread.isValid()) {
      context.releaseSpeakSource();
    }
  }
  else if (orbis.obj(speaker) == nullptr) {
    context.speakSource.isAlive = false;
  }

  OZ_AL_CHECK_ERROR();
}

void Loader::updateEnvironment()
{
  if (caelum.id != orbis.caelum.id) {
    caelum.unload();
    caelum.load();
  }

  if (terra.id != orbis.terra.id) {
    terra.unload();
    terra.load();
  }
}

void Loader::cleanupRender()
{
  if (tick % FRAG_CLEAR_INTERVAL == FRAG_CLEAR_LAG) {
    for (int i = 0; i < liber.fragPools.length(); ++i) {
      FragPool* pool = context.fragPools[i];

      if (pool == nullptr) {
        continue;
      }

      if (pool->flags & FragPool::UPDATED_BIT) {
        pool->flags &= ~FragPool::UPDATED_BIT;
      }
      else {
        delete pool;
        context.fragPools[i] = nullptr;
      }
    }
  }

  if (tick % BSP_CLEAR_INTERVAL == BSP_CLEAR_LAG) {
    for (int i = 0; i < liber.bsps.length(); ++i) {
      Context::Resource<BSPImago*>& bsp = context.bspImagines[i];

      if (bsp.nUsers != 0) {
        bsp.nUsers = 0;
      }
      else {
        delete bsp.handle;

        bsp.handle = nullptr;
        bsp.nUsers = -1;
      }
    }
  }

  if (tick % MODEL_CLEAR_INTERVAL == MODEL_CLEAR_LAG) {
    for (int i = 0; i < liber.models.length(); ++i) {
      Context::Resource<Model*>& model = context.models[i];

      if (model.nUsers == 0) {
        delete model.handle;

        model.handle = nullptr;
        model.nUsers = -1;
      }
    }
  }

  if (tick % PARTICLE_CLEAR_INTERVAL == PARTICLE_CLEAR_LAG) {
    for (int i = 0; i < liber.parts.length(); ++i) {
    }
  }

  OZ_GL_CHECK_ERROR();
}

void Loader::cleanupSound()
{
  OZ_AL_CHECK_ERROR();

  // Remove unused BSP audio objects.
  if (tick % BSPAUDIO_CLEAR_INTERVAL == BSPAUDIO_CLEAR_LAG) {
    for (int i = 0; i < liber.bsps.length(); ++i) {
      Context::Resource<BSPAudio*>& bspAudio = context.bspAudios[i];

      if (bspAudio.nUsers != 0) {
        bspAudio.nUsers = 0;
      }
      else {
        delete bspAudio.handle;

        bspAudio.handle = nullptr;
        bspAudio.nUsers = -1;
      }
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Loader::preloadRender()
{
  for (int i = 0; i < liber.bsps.length(); ++i) {
    BSPImago* bsp = context.bspImagines[i].handle;

    if (bsp != nullptr && !bsp->isLoaded() && !bsp->isPreloaded()) {
      bsp->preload();
    }
  }

  for (int i = 0; i < liber.models.length(); ++i) {
    Model* model = context.models[i].handle;

    if (model != nullptr && !model->isLoaded() && !model->isPreloaded()) {
      model->preload();
    }
  }
}

void Loader::uploadRender(bool isOneShot)
{
  for (int i = 0; i < liber.bsps.length(); ++i) {
    BSPImago* bsp = context.bspImagines[i].handle;

    if (bsp != nullptr && bsp->isPreloaded()) {
      bsp->load();

      if (isOneShot) {
        return;
      }
    }
  }

  for (int i = 0; i < liber.models.length(); ++i) {
    Model* model = context.models[i].handle;

    if (model != nullptr && model->isPreloaded()) {
      model->load();

      if (isOneShot) {
        return;
      }
    }
  }
}

void Loader::preloadRun()
{
  preloadAuxSemaphore.wait();

  while (isPreloadAlive) {
    preloadRender();

    preloadMainSemaphore.post();
    preloadAuxSemaphore.wait();
  }
}

void Loader::makeScreenshot()
{
  File picturesDir = config["dir.pictures"].get(File::PICTURES);
  File screenshot  = picturesDir / "OpenZone";

  picturesDir.mkdir();

  Log::println("Screenshot to '%s' scheduled in background thread", screenshot.c());
  Window::screenshot(screenshot);
}

void Loader::syncUpdate()
{
  MainCall() << [&]
  {
    updateEnvironment();

    if (context.dynamicLoading) {
      preloadRender();
      uploadRender(false);
    }
  };
}

void Loader::update()
{
  updateSound();

  if (context.dynamicLoading) {
    cleanupSound();
  }

  if (!preloadMainSemaphore.tryWait()) {
    return;
  }

  MainCall() << [&]
  {
    updateRender();
    updateEnvironment();

    if (context.dynamicLoading) {
      cleanupRender();
      uploadRender(true);
    }
  };

  tick = (tick + 1) % TICK_PERIOD;

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

  preloadThread = Thread("preload", preloadMain);
}

void Loader::destroy()
{
  isPreloadAlive = false;

  preloadAuxSemaphore.post();
  preloadThread.join();
}

Loader loader;

}
}
