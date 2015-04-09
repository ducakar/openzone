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
 * @file matrix/BotClass.cc
 */

#include <matrix/BotClass.hh>

#include <matrix/Liber.hh>
#include <matrix/Bot.hh>

namespace oz
{

const Mat4 BotClass::INJURY_COLOUR = Mat4(1.00f, 0.00f, 0.00f, 0.00f,
                                          1.00f, 0.00f, 0.00f, 0.00f,
                                          1.00f, 0.00f, 0.00f, 0.00f,
                                          0.20f, 0.05f, 0.05f, 1.00f);

const Mat4 BotClass::NV_COLOUR     = Mat4(0.25f, 2.00f, 0.25f, 0.00f,
                                          0.25f, 2.00f, 0.25f, 0.00f,
                                          0.25f, 2.00f, 0.25f, 0.00f,
                                          0.00f, 0.00f, 0.00f, 1.00f);

ObjectClass* BotClass::createClass()
{
  return new BotClass();
}

void BotClass::init(const Json& config, const char* name_)
{
  DynamicClass::init(config, name_);

  flags |= Object::BOT_BIT | Object::CYLINDER_BIT | Object::UPDATE_FUNC_BIT |
           Object::STATUS_FUNC_BIT;
  // We don't allow browsing a bot's inventory as long as one is alive.
  flags &= ~Object::BROWSABLE_BIT;
  life  *= 2.0f;

  if (audioType >= 0) {
    const Json& soundsConfig = config["audioSounds"];

    const char* sEventStep          = soundsConfig["step"         ].get("");
    const char* sEventWaterStep     = soundsConfig["waterStep"    ].get("");
    const char* sEventSwimSurface   = soundsConfig["swimSurface"  ].get("");
    const char* sEventSwimSubmerged = soundsConfig["swimSubmerged"].get("");

    const char* sEventDeath         = soundsConfig["death"        ].get("");
    const char* sEventJump          = soundsConfig["jump"         ].get("");
    const char* sEventMelee         = soundsConfig["melee"        ].get("");
    const char* sEventTake          = soundsConfig["take"         ].get("");

    const char* sEventPoint         = soundsConfig["point"        ].get("");
    const char* sEventFallBack      = soundsConfig["fallBack"     ].get("");
    const char* sEventSalute        = soundsConfig["salute"       ].get("");
    const char* sEventWave          = soundsConfig["wave"         ].get("");
    const char* sEventFlip          = soundsConfig["flip"         ].get("");

    audioSounds[Bot::EVENT_STEP]           = liber.soundIndex(sEventStep         );
    audioSounds[Bot::EVENT_WATER_STEP]     = liber.soundIndex(sEventWaterStep    );
    audioSounds[Bot::EVENT_SWIM_SURFACE]   = liber.soundIndex(sEventSwimSurface  );
    audioSounds[Bot::EVENT_SWIM_SUBMERGED] = liber.soundIndex(sEventSwimSubmerged);

    audioSounds[Bot::EVENT_DEATH]          = liber.soundIndex(sEventDeath        );
    audioSounds[Bot::EVENT_JUMP]           = liber.soundIndex(sEventJump         );
    audioSounds[Bot::EVENT_MELEE]          = liber.soundIndex(sEventMelee        );
    audioSounds[Bot::EVENT_TAKE]           = liber.soundIndex(sEventTake         );

    audioSounds[Bot::EVENT_POINT]          = liber.soundIndex(sEventPoint        );
    audioSounds[Bot::EVENT_FALL_BACK]      = liber.soundIndex(sEventFallBack     );
    audioSounds[Bot::EVENT_SALUTE]         = liber.soundIndex(sEventSalute       );
    audioSounds[Bot::EVENT_WAVE]           = liber.soundIndex(sEventWave         );
    audioSounds[Bot::EVENT_FLIP]           = liber.soundIndex(sEventFlip         );
  }

  state = 0;

  OZ_CLASS_STATE(Bot::MECHANICAL_BIT, "state.mechanical", false);

  crouchDim.x = dim.x;
  crouchDim.y = dim.y;
  crouchDim.z = config["crouchDimZ"].get(-1.0f);

  if (crouchDim.z < 0.0f) {
    OZ_ERROR("%s: Invalid bot crouch dimensions. Should be >= 0.", name_);
  }

  corpseDim.x = config["corpseDim.x"].get(2.0f * dim.x);
  corpseDim.y = config["corpseDim.y"].get(2.0f * dim.y);
  corpseDim.z = config["corpseDim.z"].get(0.20f);

  if (corpseDim.x < 0.0f || corpseDim.y < 0.0f || corpseDim.z < 0.0f) {
    OZ_ERROR("%s: Invalid bot corpse dimensions. Should be >= 0.", name_);
  }

  camZ              = config["camZ"].get(0.0f);
  crouchCamZ        = config["crouchCamZ"].get(0.0f);

  walkMomentum      = config["walkMomentum"].get(1.0f);
  runMomentum       = config["runMomentum"].get(2.0f);
  jumpMomentum      = config["jumpMomentum"].get(5.0f);

  airControl        = config["airControl"].get(0.025f);
  ladderControl     = config["ladderControl"].get(0.20f);
  waterControl      = config["waterControl"].get(0.07f);
  slickControl      = config["slickControl"].get(0.07f);

  stepWalkInc       = config["stepWalkInc"].get(6.0f / 6.0f) * Timer::TICK_TIME;
  stepRunInc        = config["stepRunInc"].get(10.0f / 6.0f) * Timer::TICK_TIME;

  stairInc          = config["stairInc"].get(11.0f / 64.0f);
  stairMax          = config["stairMax"].get(22.0f / 64.0f);
  stairRateLimit    = config["stairRateLimit"].get(0.15f);
  stairRateSupp     = config["stairRateSupp"].get(0.80f);

  climbInc          = config["climbInc"].get(0.25f);
  climbMax          = config["climbMax"].get(2.25f);
  climbMomentum     = config["climbMomentum"].get(2.0f);

  stamina           = config["stamina"].get(100.0f);

  if (stamina <= 0.0f || !Math::isFinite(stamina)) {
    OZ_ERROR("%s: Invalid stamina value. Should be > 0 and finite. If you want infinite stamina"
             " rather set stamina*Drain variables to zero.", name_);
  }

  staminaGain       = config["staminaGain"].get(2.25f) * Timer::TICK_TIME;
  staminaRunDrain   = config["staminaRunDrain"].get(4.0f) * Timer::TICK_TIME;
  staminaWaterDrain = config["staminaWaterDrain"].get(4.0f) * Timer::TICK_TIME;
  staminaClimbDrain = config["staminaClimbDrain"].get(10.0f) * Timer::TICK_TIME;
  staminaJumpDrain  = config["staminaJumpDrain"].get(5.0f);
  staminaThrowDrain = config["staminaThrowDrain"].get(8.0f);

  regeneration      = config["regeneration"].get(0.0f) * Timer::TICK_TIME;

  reachDist         = config["reachDist"].get(2.5f);
  grabWeight        = config["grabWeight"].get(500.0f);
  throwMomentum     = config["throwMomentum"].get(6.0f);

  weaponItem        = config["weaponItem"].get(-1);
  meleeInterval     = config["meleeInterval"].get(0.5f);
  onMelee           = config["onMelee"].get("");

  if (!String::isEmpty(onMelee)) {
    flags |= Object::LUA_BIT;
  }

  nameFunc          = config["nameFunc"].get("");
  mind              = config["mind"].get("");

  bobRotation       = Math::rad(config["bobRotation"].get(0.35f));
  bobAmplitude      = config["bobAmplitude"].get(0.07f);
  bobSwimAmplitude  = config["bobSwimAmplitude"].get(0.07f);

  baseColour        = config["baseColour"].get(Mat4::ID);
  injuryColour      = config["injuryColour"].get(INJURY_COLOUR);
  nvColour          = config["nvColour"].get(NV_COLOUR);
}

Object* BotClass::create(int index, const Point& pos, Heading heading) const
{
  return new Bot(this, index, pos, heading);
}

Object* BotClass::create(int index, const Json& json) const
{
  return new Bot(this, index, json);
}

Object* BotClass::create(InputStream* is) const
{
  return new Bot(this, is);
}

}
