/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * @file matrix/Bot.hh
 */

#pragma once

#include <matrix/Struct.hh>
#include <matrix/Weapon.hh>
#include <matrix/BotClass.hh>

namespace oz
{

class Bot : public Dynamic
{
public:

  /**
   * EVENTS
   */

  // Step and swim events are not in use but merely reserve sound slots.
  static constexpr int EVENT_STEP             = 9;
  static constexpr int EVENT_WATER_STEP       = 10;
  static constexpr int EVENT_SWIM_SURFACE     = 11;
  static constexpr int EVENT_SWIM_SUBMERGED   = 12;

  static constexpr int EVENT_DEATH            = 13;
  static constexpr int EVENT_JUMP             = 14;
  static constexpr int EVENT_MELEE            = 15;
  static constexpr int EVENT_TAKE             = 16;

  // Gestures.
  static constexpr int EVENT_POINT            = 17;
  static constexpr int EVENT_FALL_BACK        = 18;
  static constexpr int EVENT_SALUTE           = 19;
  static constexpr int EVENT_WAVE             = 20;
  static constexpr int EVENT_FLIP             = 21;

  /*
   * ACTIONS
   */

  static constexpr int ACTION_FORWARD         = 0x00000001;
  static constexpr int ACTION_BACKWARD        = 0x00000002;
  static constexpr int ACTION_RIGHT           = 0x00000004;
  static constexpr int ACTION_LEFT            = 0x00000008;
  static constexpr int ACTION_JUMP            = 0x00000010;
  static constexpr int ACTION_CROUCH          = 0x00000020;
  static constexpr int ACTION_WALK            = 0x00000040;

  static constexpr int ACTION_ATTACK          = 0x00000080;
  static constexpr int ACTION_EXIT            = 0x00000100;
  static constexpr int ACTION_EJECT           = 0x00000200;
  static constexpr int ACTION_SUICIDE         = 0x00000400;

  static constexpr int ACTION_VEH_UP          = 0x00000800;
  static constexpr int ACTION_VEH_DOWN        = 0x00001000;
  static constexpr int ACTION_VEH_NEXT_WEAPON = 0x00002000;

  static constexpr int ACTION_POINT           = 0x00004000;
  static constexpr int ACTION_BACK            = 0x00008000;
  static constexpr int ACTION_SALUTE          = 0x00010000;
  static constexpr int ACTION_WAVE            = 0x00020000;
  static constexpr int ACTION_FLIP            = 0x00040000;

  static constexpr int ACTION_TRIGGER         = 0x00080000;
  static constexpr int ACTION_LOCK            = 0x00100000;
  static constexpr int ACTION_USE             = 0x00200000;
  static constexpr int ACTION_TAKE            = 0x00400000;
  static constexpr int ACTION_GRAB            = 0x00800000;
  static constexpr int ACTION_ROTATE          = 0x01000000;
  static constexpr int ACTION_THROW           = 0x02000000;

  static constexpr int ACTION_INV_USE         = 0x04000000;
  static constexpr int ACTION_INV_TAKE        = 0x08000000;
  static constexpr int ACTION_INV_GIVE        = 0x10000000;
  static constexpr int ACTION_INV_DROP        = 0x20000000;
  static constexpr int ACTION_INV_GRAB        = 0x40000000;

  static constexpr int ACTION_GESTURE_MASK    = ACTION_POINT | ACTION_BACK | ACTION_SALUTE |
                                                ACTION_WAVE | ACTION_FLIP;

  static constexpr int INSTRUMENT_ACTIONS     = ACTION_TRIGGER | ACTION_LOCK | ACTION_USE |
                                                ACTION_TAKE | ACTION_GRAB | ACTION_ROTATE |
                                                ACTION_THROW | ACTION_INV_USE | ACTION_INV_TAKE |
                                                ACTION_INV_GIVE | ACTION_INV_DROP | ACTION_INV_GRAB;

  /*
   * STATE
   */

  static constexpr int DEAD_BIT               = 0x00000001;
  static constexpr int MECHANICAL_BIT         = 0x00000002;
  static constexpr int INCARNATABLE_BIT       = 0x00000004;

  static constexpr int MOVING_BIT             = 0x00000010;
  static constexpr int CROUCHING_BIT          = 0x00000020;
  static constexpr int WALKING_BIT            = 0x00000040;
  static constexpr int ATTACKING_BIT          = 0x00000080;

  // current state bits
  static constexpr int JUMP_SCHED_BIT         = 0x00000100;
  // on floor or an another object
  static constexpr int GROUNDED_BIT           = 0x00000200;
  // climbing a ladder
  static constexpr int LADDER_BIT             = 0x00000800;
  // climbing a ledge
  static constexpr int LEDGE_BIT              = 0x00001000;
  static constexpr int SWIMMING_BIT           = 0x00002000;
  static constexpr int SUBMERGED_BIT          = 0x00004000;

  static constexpr int GESTURE_POINT_BIT      = 0x01000000;
  static constexpr int GESTURE_FALL_BACK_BIT  = 0x02000000;
  static constexpr int GESTURE_SALUTE_BIT     = 0x04000000;
  static constexpr int GESTURE_WAVE_BIT       = 0x08000000;
  static constexpr int GESTURE_FLIP_BIT       = 0x10000000;

  static constexpr int GESTURE_MASK           = GESTURE_POINT_BIT | GESTURE_FALL_BACK_BIT |
                                                GESTURE_SALUTE_BIT | GESTURE_WAVE_BIT |
                                                GESTURE_FLIP_BIT;

  static constexpr float AIR_FRICTION         =  0.01f;
  static constexpr float LADDER_SLIP_MOMENTUM =  16.0f;

  static constexpr float WOUNDED_THRESHOLD    =  0.70f;
  static constexpr float DROWNING_RATIO       =  8.00f;
  static constexpr float CORPSE_FADE_FACTOR   =  0.50f / 100.0f;

  static constexpr float INSTRUMENT_DIST      =  2.00f;
  static constexpr float INSTRUMENT_DOT_MIN   =  0.80f;

  static constexpr float GRAB_EPSILON         =  0.20f;
  static constexpr float GRAB_STRING_RATIO    =  10.0f;
  static constexpr float GRAB_HANDLE_TOL      =  1.60f;
  static constexpr float GRAB_MOM_RATIO       =  0.30f;
  static constexpr float GRAB_MOM_MAX         =  1.00f; // must be < abs(Physics::HIT_THRESHOLD)
  static constexpr float GRAB_MOM_MAX_SQ      =  1.00f;

  static constexpr float STEP_MOVE_AHEAD      =  0.20f;
  static constexpr float CLIMB_MOVE_AHEAD     =  0.40f;

public:

  static Pool<Bot> pool;

  float  h, v;
  int    actions, oldActions;
  int    instrument;
  int    container;

  int    state, oldState;
  float  stamina;
  float  step;
  float  stairRate;

  int    cargo;
  int    weapon;
  float  grabHandle;
  float  meleeTime;

  float  camZ;

  String name;
  String mind;

public:

  bool hasAttribute(int attribute) const;

  bool canReach(const Entity* ent) const;
  bool canReach(const Object* obj) const;
  bool canEquip(const Weapon* weaponObj) const;

  bool trigger(const Entity* entity);
  bool lock(const Entity* entity);
  bool use(const Object* object);
  bool take(const Dynamic* item);
  bool grab(const Dynamic* dynamic = nullptr);
  bool rotateCargo();
  bool throwCargo();
  bool invUse(const Dynamic* item, const Object* source);
  bool invTake(const Dynamic* item, const Object* source);
  bool invGive(const Dynamic* item, const Object* target);
  bool invDrop(const Dynamic* item);
  bool invGrab(const Dynamic* item);

  void grabCargo(Dynamic* dyn);
  void releaseCargo();
  void heal();
  void rearm();
  void kill();

  void enter(int vehicle_);
  void exit();

protected:

  void onDestroy() override;
  void onUpdate() override;
  String getTitle() const override;
  float getStatus() const override;

public:

  explicit Bot(const BotClass* clazz_, int index_, const Point& p_, Heading heading);
  explicit Bot(const BotClass* clazz_, int index_, const Json& json);
  explicit Bot(const BotClass* clazz_, Stream* is);

  Json write() const override;
  void write(Stream* os) const override;

  void readUpdate(Stream* is) override;
  void writeUpdate(Stream* os) const override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
