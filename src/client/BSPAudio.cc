/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/BSPAudio.cc
 */

#include <client/BSPAudio.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{

void BSPAudio::playDemolish(const Struct* str, int sound) const
{
  hard_assert(uint(sound) < uint(liber.sounds.length()));

  uint srcId = context.addSource(sound);
  if (srcId == Context::INVALID_SOURCE) {
    return;
  }

  alSourcef(srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE);
  alSourcef(srcId, AL_ROLLOFF_FACTOR, Audio::ROLLOFF_FACTOR);

  Audio::collider.translate(camera.p, str->p - camera.p);
  bool isObstructed = collider.hit.str != str;

  if (isObstructed) {
    alSourcef(srcId, AL_GAIN, 0.5f);
  }
  alSourcefv(srcId, AL_POSITION, str->p);

  alSourcePlay(srcId);

  OZ_AL_CHECK_ERROR();
}

void BSPAudio::playSound(const Entity* entity, int sound) const
{
  hard_assert(uint(sound) < uint(liber.sounds.length()));

  const Struct* str      = entity->str;
  Point         p        = str->toAbsoluteCS(entity->clazz->p() + entity->offset);
  Vec3          velocity = str->toAbsoluteCS(entity->velocity);

  uint srcId = context.addSource(sound);
  if (srcId == Context::INVALID_SOURCE) {
    return;
  }

  alSourcef(srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE);
  alSourcef(srcId, AL_ROLLOFF_FACTOR, Audio::ROLLOFF_FACTOR);

  Audio::collider.translate(camera.p, p - camera.p);
  bool isObstructed = collider.hit.entity != entity;

  if (isObstructed) {
    alSourcef(srcId, AL_GAIN, 0.5f);
  }
  alSourcefv(srcId, AL_POSITION, p);
  alSourcefv(srcId, AL_VELOCITY, velocity);

  alSourcePlay(srcId);

  OZ_AL_CHECK_ERROR();
}

void BSPAudio::playContSound(const Entity* entity, int sound) const
{
  hard_assert(uint(sound) < uint(liber.sounds.length()));

  const Struct* str      = entity->str;
  int           key      = entity->index();
  Point         p        = str->toAbsoluteCS(entity->clazz->p() + entity->offset);
  Vec3          velocity = str->toAbsoluteCS(entity->velocity);

  Context::ContSource* contSource = context.contSources.find(key);
  uint                 srcId;

  if (contSource == nullptr) {
    srcId = context.addContSource(sound, key);
    if (srcId == Context::INVALID_SOURCE) {
      return;
    }

    alSourcef(srcId, AL_REFERENCE_DISTANCE, Audio::REFERENCE_DISTANCE);
    alSourcef(srcId, AL_ROLLOFF_FACTOR, Audio::ROLLOFF_FACTOR);
    alSourcei(srcId, AL_LOOPING, AL_TRUE);
  }
  else {
    srcId = contSource->id;
    contSource->isUpdated = true;
  }

  Audio::collider.translate(camera.p, p - camera.p);
  bool isObstructed = collider.hit.entity != entity;

  if (isObstructed) {
    alSourcef(srcId, AL_GAIN, 0.5f);
  }
  alSourcefv(srcId, AL_POSITION, p);
  alSourcefv(srcId, AL_VELOCITY, velocity);

  if (contSource == nullptr) {
    alSourcePlay(srcId);
  }

  OZ_AL_CHECK_ERROR();
}

BSPAudio::BSPAudio(const BSP* bsp_) :
  bsp(bsp_)
{
  for (int i = 0; i < bsp->nEntities; ++i) {
    const EntityClass& entityClass = bsp->entities[i];

    context.requestSound(entityClass.openSound);
    context.requestSound(entityClass.closeSound);
    context.requestSound(entityClass.frictSound);
  }

  context.requestSound(bsp->demolishSound);
}

BSPAudio::~BSPAudio()
{
  for (int i = 0; i < bsp->nEntities; ++i) {
    const EntityClass& entityClass = bsp->entities[i];

    context.releaseSound(entityClass.openSound);
    context.releaseSound(entityClass.closeSound);
    context.releaseSound(entityClass.frictSound);
  }

  context.releaseSound(bsp->demolishSound);
}

void BSPAudio::play(const Struct* str) const
{
  if (str->life == 0.0f && str->demolishing == 0.0f) {
    int demolishSound = str->bsp->demolishSound;

    if (demolishSound >= 0) {
      playDemolish(str, demolishSound);
    }
  }

  for (const Entity& entity : str->entities) {
    if (entity.state == Entity::OPENING) {
      if (entity.time == 0.0f && entity.clazz->openSound >= 0) {
        playSound(&entity, entity.clazz->openSound);
      }
      if (entity.clazz->frictSound >= 0) {
        playContSound(&entity, entity.clazz->frictSound);
      }
    }
    else if (entity.state == Entity::CLOSING) {
      if (entity.time == 0.0f && entity.clazz->closeSound >= 0) {
        playSound(&entity, entity.clazz->closeSound);
      }
      if (entity.clazz->frictSound >= 0) {
        playContSound(&entity, entity.clazz->frictSound);
      }
    }
  }
}

}
}
