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
 * @file client/StrategicProxy.cc
 */

#include <client/StrategicProxy.hh>

#include <client/Camera.hh>
#include <client/Input.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/GalileoFrame.hh>
#include <client/ui/BuildFrame.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

const float StrategicProxy::MIN_HEIGHT      = 2.00f;
const float StrategicProxy::MAX_HEIGHT      = 50.0f;
const float StrategicProxy::DEFAULT_HEIGHT  = 15.0f;
const float StrategicProxy::DEFAULT_ANGLE   = 45.0f;
const float StrategicProxy::FREE_LOW_SPEED  = 2.40f;
const float StrategicProxy::FREE_HIGH_SPEED = 30.0f;
const float StrategicProxy::RTS_LOW_SPEED   = 15.0f;
const float StrategicProxy::RTS_HIGH_SPEED  = 45.0f;
const float StrategicProxy::ZOOM_FACTOR     = 0.10f;

void StrategicProxy::begin()
{
  camera.setTaggedObj(nullptr);
  camera.setTaggedEnt(nullptr);
  camera.smoothMagnify(1.0f);
  camera.isExternal = true;

  ui::mouse.isVisible = true;

  ui::ui.strategicArea->enable(true);
  ui::ui.buildFrame->enable(hasBuildFrame);

  desiredPos = camera.p;
}

void StrategicProxy::end()
{
  camera.colour = camera.baseColour;

  ui::mouse.isVisible = true;

  ui::ui.buildFrame->enable(false);
  ui::ui.strategicArea->enable(false);
}

void StrategicProxy::prepare()
{
  if (input.keys[Input::KEY_CAMERA_TOGGLE] && !input.oldKeys[Input::KEY_CAMERA_TOGGLE]) {
    isFree     = !isFree;
    isRTSFast  = false;
    isFreeFast = true;
  }

  if (input.keys[Input::KEY_SWITCH_TO_UNIT] && !input.oldKeys[Input::KEY_SWITCH_TO_UNIT] &&
      camera.allowReincarnation)
  {
    if (ui::ui.strategicArea->taggedObjs.length() == 1) {
      const Object* tagged = orbis.obj(ui::ui.strategicArea->taggedObjs.first());
      const Bot*    me     = nullptr;

      if (tagged != nullptr) {
        if (tagged->flags & Object::BOT_BIT) {
          me = static_cast<const Bot*>(tagged);
        }
        else if (tagged->flags & Object::VEHICLE_BIT) {
          const Vehicle* veh = static_cast<const Vehicle*>(tagged);

          me = orbis.obj<const Bot>(veh->pilot);
        }
      }

      if (me != nullptr) {
        camera.setBot(const_cast<Bot*>(me));
        camera.setState(Camera::UNIT);
      }
    }
  }

  if (input.keys[Input::KEY_CYCLE_UNITS] && !input.oldKeys[Input::KEY_CYCLE_UNITS]) {
    if (!camera.switchableUnits.isEmpty()) {
      Bot* bot = orbis.obj<Bot>(camera.switchableUnits.first());

      camera.setBot(bot);
      camera.setState(Camera::UNIT);
    }
  }

  if (input.keys[Input::KEY_NV_TOGGLE] && !input.oldKeys[Input::KEY_NV_TOGGLE]) {
    camera.nightVision = !camera.nightVision;
  }

  if (input.keys[Input::KEY_MAP_TOGGLE] && !input.oldKeys[Input::KEY_MAP_TOGGLE]) {
    ui::ui.galileoFrame->setMaximised(!ui::ui.galileoFrame->isMaximised);
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

void StrategicProxy::update()
{
  h = angleWrap(h + camera.relH);
  v = clamp(v + camera.relV, 0.0f, Math::TAU / 2.0f);

  camera.rotateTo(Quat::rotationZXZ(h, v, 0.0f));
  camera.align();

  if (isFree) {
    // free camera mode
    if (input.keys[Input::KEY_SPEED_TOGGLE] && !input.oldKeys[Input::KEY_SPEED_TOGGLE]) {
      isFreeFast = !isFreeFast;
    }

    float speed = (isFreeFast ? FREE_HIGH_SPEED : FREE_LOW_SPEED) * Timer::TICK_TIME;

    if (!Math::isNaN(ui::ui.galileoFrame->clickX)) {
      desiredPos.x = ui::ui.galileoFrame->clickX;
      desiredPos.y = ui::ui.galileoFrame->clickY;
    }
    else {
      desiredPos += input.moveY * camera.at    * speed;
      desiredPos += input.moveX * camera.right * speed;
    }

    if (input.keys[Input::KEY_MOVE_UP]) {
      desiredPos.z += speed;
    }
    if (input.keys[Input::KEY_MOVE_DOWN]) {
      desiredPos.z -= speed;
    }

    desiredPos.x = clamp<float>(desiredPos.x, -Orbis::DIM, +Orbis::DIM);
    desiredPos.y = clamp<float>(desiredPos.y, -Orbis::DIM, +Orbis::DIM);
    desiredPos.z = clamp<float>(desiredPos.z, -Orbis::DIM, +Orbis::DIM);
  }
  else {
    // RTS camera mode
    if (input.keys[Input::KEY_SPEED_TOGGLE] && !input.oldKeys[Input::KEY_SPEED_TOGGLE]) {
      isRTSFast = !isRTSFast;
    }

    Vec3  up = Vec3(-camera.right.y, camera.right.x, 0.0f);
    float logHeight = Math::log(height);
    float speed = (isRTSFast ? RTS_HIGH_SPEED : RTS_LOW_SPEED) * Timer::TICK_TIME * logHeight;

    if (!Math::isNaN(ui::ui.galileoFrame->clickX)) {
      desiredPos.x = ui::ui.galileoFrame->clickX;
      desiredPos.y = ui::ui.galileoFrame->clickY;
    }
    else {
      desiredPos += input.moveY * up           * speed;
      desiredPos += input.moveX * camera.right * speed;
    }

    if (input.wheelDown || input.wheelUp) {
      float wheelFactor = float(ui::ui.strategicArea->mouseW);

      height = clamp(height + logHeight * ZOOM_FACTOR * wheelFactor, MIN_HEIGHT, MAX_HEIGHT);

      ui::ui.strategicArea->mouseW = 0.0f;
    }

    desiredPos.x = clamp<float>(desiredPos.x, -Orbis::DIM, +Orbis::DIM);
    desiredPos.y = clamp<float>(desiredPos.y, -Orbis::DIM, +Orbis::DIM);
    desiredPos.z = max(0.0f, orbis.terra.getHeight(desiredPos.x, desiredPos.y)) + height;
  }

  camera.smoothMoveTo(desiredPos);

  camera.colour = camera.nightVision ? camera.nvColour : camera.baseColour;
}

void StrategicProxy::reset()
{
  h             = 0.0f;
  v             = 0.0f;
  desiredPos    = Point::ORIGIN;
  height        = DEFAULT_HEIGHT;

  isFree        = false;
  isFreeFast    = true;
  isRTSFast     = false;

  hasBuildFrame = false;
}

void StrategicProxy::read(Stream* is)
{
  h             = is->readFloat();
  v             = is->readFloat();
  height        = is->readFloat();

  isFree        = is->readBool();
  isFreeFast    = is->readBool();
  isRTSFast     = is->readBool();

  hasBuildFrame = is->readBool();
}

void StrategicProxy::read(const Json& json)
{
  h      = json["h"].get(0.0f);
  v      = json["v"].get(0.0f);
  height = json["height"].get(DEFAULT_HEIGHT);

  isFree = json["isFree"].get(false);
}

void StrategicProxy::write(Stream* os) const
{
  os->writeFloat(h);
  os->writeFloat(v);
  os->writeFloat(height);

  os->writeBool(isFree);
  os->writeBool(isFreeFast);
  os->writeBool(isRTSFast);

  os->writeBool(hasBuildFrame);
}

Json StrategicProxy::write() const
{
  Json json(Json::OBJECT);

  json.add("h", h);
  json.add("v", v);
  json.add("height", height);

  json.add("isFree", isFree);

  return json;
}

}
}
