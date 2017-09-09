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

#include <client/Audio.hh>

#include <client/Camera.hh>
#include <client/Context.hh>

namespace oz
{
namespace client
{

const float Audio::ROLLOFF_FACTOR       = 0.50f;
const float Audio::COCKPIT_GAIN_FACTOR  = 0.35f;
const float Audio::COCKPIT_PITCH_FACTOR = 0.95f;

Collider Audio::collider;

void Audio::playSound(int sound, float volume, const Object* parent) const
{
  OZ_ASSERT(uint(sound) < uint(liber.sounds.size()));

  const Dynamic* dynParent = static_cast<const Dynamic*>(parent);

  Context::Source* source = context.addSource(sound);
  if (source == nullptr) {
    return;
  }

  uint srcId = source->id;

  alSourcef(srcId, AL_ROLLOFF_FACTOR, ROLLOFF_FACTOR);

  // If the object moves since source starts playing and source stands still, it's usually
  // not noticeable for short-time source. After all, sound source many times does't move
  // together with the object in many cases (e.g. the sound when an objects hits something).
  //
  // However, when the sound is generated by the player (e.g. cries, talk) it is often annoying
  // if the sound source doesn't move with the player. That's why we position such sounds
  // at the origin of the coordinate system relative to player.
  if (parent == camera.botObj || obj == camera.botObj ||
      (camera.botObj != nullptr && parent->index == camera.botObj->parent))
  {
    alSourcef(srcId, AL_GAIN, volume);
    alSourcei(srcId, AL_SOURCE_RELATIVE, AL_TRUE);
  }
  else {
    collider.translate(camera.p, parent->p - camera.p, parent);
    bool isObstructed = collider.hit.ratio != 1.0f;

    alSourcef(srcId, AL_GAIN, isObstructed ? volume / 2.0f : volume);
    alSourcefv(srcId, AL_POSITION, parent->p);
    if (parent->flags & Object::DYNAMIC_BIT) {
      alSourcefv(srcId, AL_VELOCITY, dynParent->velocity);
    }
  }

  alSourcePlay(srcId);

  OZ_AL_CHECK_ERROR();
}

void Audio::playContSound(int sound, float volume, const Object* parent) const
{
  OZ_ASSERT(uint(sound) < uint(liber.sounds.size()));

  int key = obj->index * ObjectClass::MAX_SOUNDS + sound;

  Context::ContSource* contSource = context.contSources.find(key);
  const Dynamic*       dynParent  = static_cast<const Dynamic*>(parent);
  uint                 srcId;

  if (contSource == nullptr) {
    contSource = context.addContSource(sound, key);
    if (contSource == nullptr) {
      return;
    }

    srcId = contSource->id;

    alSourcei(srcId, AL_LOOPING, AL_TRUE);
    alSourcef(srcId, AL_ROLLOFF_FACTOR, ROLLOFF_FACTOR);
  }
  else {
    srcId = contSource->id;
    contSource->isUpdated = true;
  }

  collider.translate(camera.p, parent->p - camera.p, parent);
  bool isObstructed = collider.hit.ratio != 1.0f;

  alSourcef(srcId, AL_GAIN, isObstructed ? volume / 2.0f : volume);
  alSourcefv(srcId, AL_POSITION, parent->p);
  if (parent->flags & Object::DYNAMIC_BIT) {
    alSourcefv(srcId, AL_VELOCITY, dynParent->velocity);
  }

  if (contSource == nullptr) {
    alSourcePlay(srcId);
  }

  OZ_AL_CHECK_ERROR();
}

bool Audio::playSpeak(const char* text, float volume, const Object* parent) const
{
  const Dynamic* dynParent = static_cast<const Dynamic*>(parent);
  int            owner = context.speakSource.owner.load<RELAXED>();
  uint           srcId;

  if (owner == -1) {
    if (text == nullptr) {
      return false;
    }

    Context::SpeakSource* speakSource = context.requestSpeakSource(text, obj->index);
    if (speakSource == nullptr) {
      return false;
    }

    srcId = speakSource->id;

    alSourcef(srcId, AL_ROLLOFF_FACTOR, ROLLOFF_FACTOR);
  }
  else if (owner == obj->index) {
    srcId = context.speakSource.id;
  }
  else {
    return false;
  }

  collider.translate(camera.p, parent->p - camera.p, parent);
  bool isObstructed = collider.hit.ratio != 1.0f;

  alSourcef(srcId, AL_GAIN, isObstructed ? volume / 2.0f : volume);
  alSourcefv(srcId, AL_POSITION, parent->p);
  if (parent->flags & Object::DYNAMIC_BIT) {
    alSourcefv(srcId, AL_VELOCITY, dynParent->velocity);
  }

  OZ_AL_CHECK_ERROR();

  return true;
}

void Audio::playEngineSound(int sound, float volume, float pitch, const Object* parent) const
{
  OZ_ASSERT(uint(sound) < uint(liber.sounds.size()));
  OZ_ASSERT(obj->flags & Object::VEHICLE_BIT);

  const Vehicle* veh = static_cast<const Vehicle*>(obj);

  if (!camera.isExternal && veh->pilot != -1 && veh->pilot == camera.bot) {
    volume *= COCKPIT_GAIN_FACTOR;
    pitch  *= COCKPIT_PITCH_FACTOR;
  }

  int key = veh->index * ObjectClass::MAX_SOUNDS + sound;

  Context::ContSource* contSource = context.contSources.find(key);
  uint                 srcId;

  if (contSource == nullptr) {
    contSource = context.addContSource(sound, key);
    if (contSource == nullptr) {
      return;
    }

    srcId = contSource->id;

    alSourcei(srcId, AL_LOOPING, AL_TRUE);
    alSourcef(srcId, AL_ROLLOFF_FACTOR, ROLLOFF_FACTOR);
  }
  else {
    srcId = contSource->id;
    contSource->isUpdated = true;
  }

  collider.translate(camera.p, parent->p - camera.p, parent);
  bool isObstructed = collider.hit.ratio != 1.0f;

  alSourcef(srcId, AL_PITCH, pitch);
  alSourcef(srcId, AL_GAIN, isObstructed ? volume / 2.0f : volume);
  alSourcefv(srcId, AL_POSITION, veh->p);
  alSourcefv(srcId, AL_VELOCITY, veh->velocity);

  if (contSource == nullptr) {
    alSourcePlay(srcId);
  }

  OZ_AL_CHECK_ERROR();
}

Audio::Audio(const Object* obj_)
  : obj(obj_), clazz(obj_->clazz), flags(0)
{
  const int* sounds = clazz->audioSounds;

  for (int i = 0; i < ObjectClass::MAX_SOUNDS; ++i) {
    context.requestSound(sounds[i]);
  }

  OZ_AL_CHECK_ERROR();
}

Audio::~Audio()
{
  const int* sounds = clazz->audioSounds;

  for (int i = 0; i < ObjectClass::MAX_SOUNDS; ++i) {
    context.releaseSound(sounds[i]);
  }

  OZ_AL_CHECK_ERROR();
}

}
}
