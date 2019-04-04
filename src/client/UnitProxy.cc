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

#include <client/UnitProxy.hh>

#include <client/Camera.hh>
#include <client/Input.hh>
#include <client/Context.hh>
#include <client/ui/HudArea.hh>
#include <client/ui/Inventory.hh>
#include <client/ui/InfoFrame.hh>
#include <client/ui/GalileoFrame.hh>
#include <client/ui/MusicPlayer.hh>
#include <client/ui/UI.hh>

namespace oz::client
{

void UnitProxy::begin()
{
  camera.setTaggedObj(nullptr);
  camera.setTaggedEnt(nullptr);
  camera.isExternal = isExternal;

  ui::mouse.isVisible = false;

  ui::ui.hudArea->enable(true);
  ui::ui.inventory->enable(true);
  ui::ui.infoFrame->enable(true);
  ui::ui.musicPlayer->show(false);

  baseRot = camera.rot;
  headRot = Quat::ID;

  const Bot* bot = camera.botObj;
  if (bot != nullptr && !(bot->state & Bot::DEAD_BIT) && isExternal && isFreelook) {
    headH = bot->h;
    headV = bot->v;
  }
  else {
    headH = 0.0f;
    headV = 0.0f;
  }

  botEye      = camera.p;
  bobTheta    = 0.0f;
  bobBias     = 0.0f;

  injuryRatio = 0.0f;

  oldBot      = -1;

  isFreelook  = false;
}

void UnitProxy::end()
{
  camera.setBot(nullptr);

  ui::mouse.isVisible = true;

  ui::ui.infoFrame->enable(false);
  ui::ui.inventory->enable(false);
  ui::ui.hudArea->enable(false);
}

void UnitProxy::prepare()
{
  Bot*     bot = camera.botObj;
  Vehicle* veh = camera.vehicleObj;

  if (bot != nullptr) {
    bot->actions = 0;
    camera.strategic.h = bot->h;
  }

  if (input.keys[Input::KEY_SWITCH_TO_UNIT] && !input.oldKeys[Input::KEY_SWITCH_TO_UNIT]) {
    if (camera.allowReincarnation) {
      camera.setState(Camera::STRATEGIC);
      return;
    }
  }

  if (input.keys[Input::KEY_CYCLE_UNITS] && !input.oldKeys[Input::KEY_CYCLE_UNITS]) {
    int nSwitchableunits = camera.switchableUnits.size();

    if (nSwitchableunits != 0) {
      int currUnit = -1;

      for (int i = 0; i < nSwitchableunits; ++i) {
        if (camera.bot == camera.switchableUnits[i]) {
          currUnit = i;
          break;
        }
      }

      if (currUnit != 0 || nSwitchableunits > 1) {
        currUnit = (currUnit + 1) % nSwitchableunits;

        Bot* unit = orbis.obj<Bot>(camera.switchableUnits[currUnit]);

        if (unit->state & Bot::DEAD_BIT) {
          botEye    = unit->p;
          botEye.z += unit->camZ;
        }
        camera.setBot(unit);
        return;
      }
    }
  }

  if (bot == nullptr || (bot->state & Bot::DEAD_BIT)) {
    return;
  }

  /*
   * Camera
   */

  if (!isFreelook || (veh == nullptr && !isExternal)) {
    bot->h += camera.relH;
    bot->v += camera.relV;
  }

  if (input.keys[Input::KEY_TURN_LEFT]) {
    bot->h += input.keySensX;
  }
  if (input.keys[Input::KEY_TURN_RIGHT]) {
    bot->h -= input.keySensY;
  }

  bot->h = angleWrap(bot->h);
  bot->v = clamp(bot->v, 0.0f, Math::TAU / 2.0f);

  /*
   * Mouse
   */

  if (!ui::mouse.isVisible) {
    if (input.buttons & Input::LEFT_BUTTON) {
      bot->actions |= Bot::ACTION_ATTACK;
    }

    if (input.leftPressed) {
      if (bot->cargo != -1) {
        bot->rotateCargo();
      }
    }
    if (input.rightPressed) {
      if (bot->parent != -1) {
        context.playSample(ui::style.sounds.nextWeapon);

        bot->actions |= Bot::ACTION_VEH_NEXT_WEAPON;
      }
      else if (camera.entityObj != nullptr) {
        bot->trigger(camera.entityObj);
      }
      else if (camera.objectObj != nullptr) {
        bot->use(camera.objectObj);
      }
    }
    else if (input.middlePressed) {
      if (bot->cargo != -1) {
        bot->grab();
      }
      else if (camera.entity != -1) {
        bot->lock(camera.entityObj);
      }
      else if (camera.object != -1) {
        Dynamic* dyn = static_cast<Dynamic*>(camera.objectObj);

        if (dyn->flags & Object::DYNAMIC_BIT) {
          bot->grab(dyn);
        }
      }
    }
    else if (input.wheelDown) {
      if (camera.objectObj != nullptr) {
        if (camera.objectObj->flags & Object::BROWSABLE_BIT) {
          ui::mouse.isVisible = true;
        }
        else {
          Dynamic* dyn = static_cast<Dynamic*>(camera.objectObj);

          if (dyn->flags & Object::DYNAMIC_BIT) {
            bot->take(dyn);
          }
        }
      }
    }
    else if (input.wheelUp) {
      if (bot->cargo != -1) {
        bot->throwCargo();
      }
    }
  }

  /*
   * Movement
   */

  if (input.moveY > 0.0f) {
    bot->actions |= Bot::ACTION_FORWARD;
  }
  if (input.moveY < 0.0f) {
    bot->actions |= Bot::ACTION_BACKWARD;
  }
  if (input.moveX > 0.0f) {
    bot->actions |= Bot::ACTION_RIGHT;
  }
  if (input.moveX < 0.0f) {
    bot->actions |= Bot::ACTION_LEFT;
  }

  /*
   * Actions
   */

  if (input.keys[Input::KEY_JUMP]) {
    bot->actions |= Bot::ACTION_JUMP;
  }
  if (input.keys[Input::KEY_CROUCH_TOGGLE] && !input.oldKeys[Input::KEY_CROUCH_TOGGLE]) {
    bot->actions |= Bot::ACTION_CROUCH;
  }
  if (input.keys[Input::KEY_SPEED_TOGGLE] && !input.oldKeys[Input::KEY_SPEED_TOGGLE]) {
    bot->actions |= Bot::ACTION_WALK;
  }

  if (input.keys[Input::KEY_MOVE_UP]) {
    bot->actions |= Bot::ACTION_VEH_UP;
  }
  if (input.keys[Input::KEY_MOVE_DOWN]) {
    bot->actions |= Bot::ACTION_VEH_DOWN;
  }

  if (input.keys[Input::KEY_EXIT] && !input.oldKeys[Input::KEY_EXIT]) {
    bot->actions |= Bot::ACTION_EXIT;
  }
  if (input.keys[Input::KEY_EJECT] && !input.oldKeys[Input::KEY_EJECT]) {
    bot->actions |= Bot::ACTION_EJECT;
  }
  if (input.keys[Input::KEY_SUICIDE] && !input.oldKeys[Input::KEY_SUICIDE]) {
    bot->actions |= Bot::ACTION_SUICIDE;
  }

  int weaponIndex = -1;

  if (input.keys[Input::KEY_WEAPON_1] && !input.oldKeys[Input::KEY_WEAPON_1]) {
    weaponIndex = 0;
  }
  if (input.keys[Input::KEY_WEAPON_2] && !input.oldKeys[Input::KEY_WEAPON_2]) {
    weaponIndex = 1;
  }
  if (input.keys[Input::KEY_WEAPON_3] && !input.oldKeys[Input::KEY_WEAPON_3]) {
    weaponIndex = 2;
  }
  if (input.keys[Input::KEY_WEAPON_4] && !input.oldKeys[Input::KEY_WEAPON_4]) {
    weaponIndex = 3;
  }

  if (weaponIndex != -1) {
    for (int i = 0, index = 0; i < bot->items.size(); ++i) {
      const Weapon* weapon = orbis.obj<const Weapon>(bot->items[i]);

      if (weapon != nullptr && weapon->flags & Object::WEAPON_BIT) {
        if (index == weaponIndex) {
          bot->invUse(weapon, bot);
          break;
        }
        else {
          ++index;
        }
      }
    }
  }

  if (input.keys[Input::KEY_GESTURE_POINT]) {
    bot->actions |= Bot::ACTION_POINT;
  }
  if (input.keys[Input::KEY_GESTURE_BACK]) {
    bot->actions |= Bot::ACTION_BACK;
  }
  if (input.keys[Input::KEY_GESTURE_SALUTE]) {
    bot->actions |= Bot::ACTION_SALUTE;
  }
  if (input.keys[Input::KEY_GESTURE_WAVE]) {
    bot->actions |= Bot::ACTION_WAVE;
  }
  if (input.keys[Input::KEY_GESTURE_FLIP]) {
    bot->actions |= Bot::ACTION_FLIP;
  }

  /*
   * View
   */

  if (input.keys[Input::KEY_NV_TOGGLE] && !input.oldKeys[Input::KEY_NV_TOGGLE]) {
    camera.nightVision = !camera.nightVision;
  }

  if (input.keys[Input::KEY_BINOCULARS_TOGGLE] && !input.oldKeys[Input::KEY_BINOCULARS_TOGGLE]) {
    if (camera.desiredMag == 1.0f) {
      camera.smoothMagnify(BINOCULARS_MAGNIFICATION);
    }
    else {
      camera.smoothMagnify(1.0f);
    }
  }

  if (input.keys[Input::KEY_MAP_TOGGLE] && !input.oldKeys[Input::KEY_MAP_TOGGLE]) {
    ui::ui.galileoFrame->setMaximised(!ui::ui.galileoFrame->isMaximised);
  }

  if (camera.nightVision && !bot->hasAttribute(ObjectClass::NIGHT_VISION_BIT)) {
    camera.nightVision = false;
  }

  if (camera.desiredMag != 1.0f && !bot->hasAttribute(ObjectClass::BINOCULARS_BIT)) {
    camera.desiredMag = 1.0f;
  }

  if (input.keys[Input::KEY_CAMERA_TOGGLE] && !input.oldKeys[Input::KEY_CAMERA_TOGGLE]) {
    isExternal = !isExternal;
    isFreelook = false;

    headH = 0.0f;
    headV = 0.0f;

    camera.isExternal = isExternal;
  }
  if (input.keys[Input::KEY_FREELOOK_TOGGLE] && !input.oldKeys[Input::KEY_FREELOOK_TOGGLE]) {
    isFreelook = !isFreelook;

    if (isExternal && isFreelook) {
      headH = bot->h;
      headV = bot->v;
    }
  }

  if (input.keys[Input::KEY_CHEAT_SKY_FORWARD]) {
    orbis.caelum.time += 0.1f * Timer::TICK_TIME * orbis.caelum.period;
  }
  if (input.keys[Input::KEY_CHEAT_SKY_BACKWARD]) {
    orbis.caelum.time -= 0.1f * Timer::TICK_TIME * orbis.caelum.period;
  }

  if (input.keys[Input::KEY_UI_TOGGLE] && !input.oldKeys[Input::KEY_UI_TOGGLE]) {
    ui::mouse.isVisible = !ui::mouse.isVisible;
  }
}

void UnitProxy::update()
{
  if (camera.botObj == nullptr || (camera.botObj->state & Bot::DEAD_BIT)) {
    ui::ui.galileoFrame->setMaximised(false);

    camera.rotateTo(Quat::ID);
    camera.magnify(1.0f);
    camera.nightVision = false;
    camera.align();

    if (camera.botObj != nullptr) {
      botEye    = camera.botObj->p;
      botEye.z += camera.botObj->camZ;
    }

    bobTheta = 0.0f;
    bobBias  = 0.0f;

    Vec3 offset = Vec3(0.0f, 0.0f, DEATH_CAM_DIST);

    collider.translate(botEye, offset);
    camDist = Math::mix(collider.hit.ratio, camDist, CAMERA_DIST_SMOOTHING);
    offset *= camDist;

    float dist = !offset;
    if (dist > EXTERNAL_CAM_CLIP_DIST) {
      offset *= (dist - EXTERNAL_CAM_CLIP_DIST) / dist;
    }
    else {
      offset = Vec3::ZERO;
    }

    camera.moveTo(botEye + offset);

    camera.colour = Math::mix(camera.baseColour, camera.colour, min(injuryRatio, 1.0f));
    injuryRatio  *= INJURY_SUPPRESSION_COEF;

    oldBot = camera.bot;
    return;
  }

  const Bot*          bot      = camera.botObj;
  const BotClass*     botClazz = static_cast<const BotClass*>(bot->clazz);
  const Vehicle*      veh      = camera.vehicleObj;
  const VehicleClass* vehClazz = nullptr;

  if (veh != nullptr) {
    vehClazz = static_cast<const VehicleClass*>(veh->clazz);

    botEye = veh->p + veh->rot * vehClazz->pilotPos;
  }
  else {
    float actualZ = bot->p.z + bot->camZ;

    botEye.x = bot->p.x;
    botEye.y = bot->p.y;

    if (oldBot != camera.bot) {
      botEye.z = actualZ;
    }
    else {
      botEye.z = Math::mix(botEye.z, actualZ, CAMERA_Z_SMOOTHING);
      botEye.z = clamp(botEye.z, actualZ - CAMERA_Z_TOLERANCE, actualZ + CAMERA_Z_TOLERANCE);
    }
  }

  // external
  if (isExternal) {
    if (isFreelook) {
      headH = angleWrap(headH + camera.relH);
      headV = clamp(headV + camera.relV, 0.0f, +Math::TAU / 2.0f);

      Quat completeRot = Quat::rotationZXZ(headH, headV, 0.0f);

      baseRot = Quat::rotationZXZ(bot->h, bot->v, 0.0f);
      headRot = *baseRot * completeRot;

      camera.smoothRotateTo(completeRot);
    }
    else {
      headH = 0.0f;
      headV = 0.0f;

      baseRot = Quat::rotationZXZ(bot->h, bot->v, 0.0f);
      headRot = Quat::ID;

      camera.smoothRotateTo(baseRot);
    }

    bobTheta = 0.0f;
    bobBias  = 0.0f;

    camera.align();

    Vec3 offset;

    if (veh != nullptr) {
      float dist = veh->dim.fastN() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3(0.0f, VEHICLE_CAM_UP_FACTOR * dist, dist);
    }
    else {
      float dist = bot->dim.fastN() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3(SHOULDER_CAM_RIGHT, SHOULDER_CAM_UP, dist);
    }

    collider.translate(botEye, offset, bot);
    camDist = Math::mix(collider.hit.ratio, camDist, CAMERA_DIST_SMOOTHING);
    offset *= camDist;

    float dist = offset.fastN();
    if (dist > EXTERNAL_CAM_CLIP_DIST) {
      offset *= (dist - EXTERNAL_CAM_CLIP_DIST) / dist;
    }
    else {
      offset = Vec3::ZERO;
    }

    camera.moveTo(botEye + offset);
  }
  else {
    // internal, vehicle
    if (veh != nullptr) {
      if (isFreelook) {
        headH = clamp(headH + camera.relH, vehClazz->lookHMin, vehClazz->lookHMax);
        headV = clamp(headV + camera.relV, vehClazz->lookVMin, vehClazz->lookVMax);
      }
      else {
        headH *= Camera::ROT_SMOOTHING_COEF;
        headV *= Camera::ROT_SMOOTHING_COEF;
      }

      bobTheta = 0.0f;
      bobBias  = 0.0f;

      baseRot = Quat::rotationZXZ(veh->h, veh->v, veh->w);
      headRot = Quat::rotationY(headH) * Quat::rotationX(headV);

      camera.rotateTo(baseRot * headRot);
      camera.align();
      camera.moveTo(botEye);
    }
    // internal, bot
    else {
      camDist = 0.0f;

      if ((bot->state & (Bot::MOVING_BIT | Bot::SWIMMING_BIT)) == Bot::MOVING_BIT) {
        float phase = bot->step * Math::TAU;
        float sine  = Math::sin(phase);

        bobTheta = sine * botClazz->bobRotation;
        bobBias  = sine*sine * botClazz->bobAmplitude;
      }
      else if ((bot->state & (Bot::MOVING_BIT | Bot::SWIMMING_BIT)) ==
               (Bot::MOVING_BIT | Bot::SWIMMING_BIT))
      {
        float sine = Math::sin(bot->step * Math::TAU / 2.0f);

        bobTheta = 0.0f;
        bobBias  = sine*sine * botClazz->bobSwimAmplitude;
      }
      else {
        bobTheta *= BOB_SUPPRESSION_COEF;
        bobBias  *= BOB_SUPPRESSION_COEF;
      }

      baseRot = Quat::rotationZXZ(bot->h, bot->v, bobTheta);
      headRot = Quat::ID;
      headH   = 0.0f;
      headV   = 0.0f;

      camera.rotateTo(baseRot);
      camera.align();
      camera.moveTo(Point(botEye.x, botEye.y, botEye.z + bobBias));
    }
  }

  if (bot->parent != -1) {
    camera.setTaggedObj(nullptr);
  }
  else if (bot->cargo != -1) {
    camera.setTaggedObj(orbis.obj(bot->cargo));
  }
  else {
    // {hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine}
    float hvsc[6];

    Math::sincos(bot->h, &hvsc[0], &hvsc[1]);
    Math::sincos(bot->v, &hvsc[2], &hvsc[3]);

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    Vec3  at    = Vec3(-hvsc[4], +hvsc[5], -hvsc[3]);
    Point eye   = bot->p + Vec3(0.0f, 0.0f, bot->camZ);
    Vec3  reach = at * botClazz->reachDist;

    collider.mask = ~0;
    collider.translate(eye, reach, bot);
    collider.mask = Object::SOLID_BIT;

    camera.setTaggedObj(collider.hit.obj);
    camera.setTaggedEnt(collider.hit.entity);
  }

  if (camera.bot != oldBot) {
    injuryRatio = 0.0f;
  }
  else {
    for (const Object::Event& event : bot->events) {
      if (event.id == Object::EVENT_DAMAGE) {
        injuryRatio += event.intensity;
      }
    }
  }

  injuryRatio   = min(injuryRatio, INJURY_CLAMP);
  camera.colour = Math::mix(Mat4::ID, botClazz->injuryColour, min(injuryRatio, 1.0f)) *
                  (camera.nightVision ? botClazz->nvColour : botClazz->baseColour);
  injuryRatio  *= INJURY_SUPPRESSION_COEF;

  oldBot = camera.bot;
}

void UnitProxy::reset()
{
  baseRot     = Quat::ID;
  headRot     = Quat::ID;
  headH       = 0.0f;
  headV       = 0.0f;

  botEye      = Point::ORIGIN;
  bobTheta    = 0.0f;
  bobBias     = 0.0f;

  injuryRatio = 0.0f;
  camDist     = 0.0f;

  oldBot      = -1;

  isExternal  = false;
  isFreelook  = false;
}

void UnitProxy::read(Stream* is)
{
  isExternal = is->readBool();
}

void UnitProxy::read(const Json& json)
{
  isExternal = json["isExternal"].get(false);
}

void UnitProxy::write(Stream* os) const
{
  os->writeBool(isExternal);
}

Json UnitProxy::write() const
{
  Json json(Json::OBJECT);

  json.add("isExternal", isExternal);

  return json;
}

}
