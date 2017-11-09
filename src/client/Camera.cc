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

#include <client/Camera.hh>

#include <client/Input.hh>
#include <client/ui/Area.hh>
#include <client/ui/UI.hh>

namespace oz::client
{

StrategicProxy Camera::strategic;
UnitProxy      Camera::unit;
CinematicProxy Camera::cinematic;

Proxy* const PROXIES[] = {
  nullptr,
  &Camera::strategic,
  &Camera::unit,
  &Camera::cinematic
};

void Camera::flash(float intensity)
{
  flashColour = Math::mix(Mat4::ID, FLASH_COLOUR, intensity);
}

void Camera::shake(float intensity)
{
  float heading = intensity * Math::rand() * Math::TAU;
  float pitch   = intensity * Math::rand() * Math::TAU / 2.0f;
  float cPitch  = Math::cos(pitch);
  Vec3  axis    = Vec3(cPitch * Math::sin(heading), cPitch * Math::cos(heading), Math::sin(pitch));

  shakeRot = Quat::rotationAxis(axis, intensity);
}

void Camera::updateReferences()
{
  objectObj  = orbis.obj(object);
  object     = objectObj == nullptr ? -1 : object;

  entityObj  = orbis.ent(entity);
  entity     = entityObj == nullptr ? -1 : entity;

  botObj     = orbis.obj<Bot>(bot);
  bot        = botObj == nullptr ? -1 : bot;

  vehicleObj = botObj == nullptr ? nullptr : orbis.obj<Vehicle>(botObj->parent);
  vehicle    = vehicleObj == nullptr ? -1 : botObj->parent;

  for (int i = 0; i < switchableUnits.size();) {
    const Bot* switchableUnit = orbis.obj<const Bot>(switchableUnits[i]);

    if (switchableUnit == nullptr) {
      switchableUnits.erase(i);
    }
    else {
      ++i;
    }
  }
}

void Camera::align()
{
  rot      = ~Quat::fastSlerp(rot, desiredRot * shakeRot, ROT_SMOOTHING_COEF);
  mag      = Math::mix(mag, desiredMag, SMOOTHING_COEF);
  p        = Math::mix(p, desiredPos, SMOOTHING_COEF);
  velocity = (p - oldPos) / Timer::TICK_TIME;
  oldPos   = p;

  rotMat   = Mat4::rotation(rot);
  rotTMat  = ~rotMat;

  right    = Vec3(+rotMat.x);
  up       = Vec3(+rotMat.y);
  at       = Vec3(-rotMat.z);
}

void Camera::prepare()
{
  updateReferences();

  ui::mouse.update();

  relH = clamp(input.lookX * mag, -ROT_LIMIT, +ROT_LIMIT);
  relV = clamp(input.lookY * mag, -ROT_LIMIT, +ROT_LIMIT);

  if (newState != state) {
    if (proxy != nullptr) {
      proxy->end();
    }

    proxy = PROXIES[newState];

    if (proxy != nullptr) {
      proxy->begin();
    }
    else {
      bot       = -1;
      botObj    = nullptr;

      object    = -1;
      objectObj = nullptr;

      entity    = -1;
      entityObj = nullptr;
    }

    state = newState;
  }

  if (proxy != nullptr) {
    proxy->prepare();
  }

  ui::ui.update();

  if (Window::width() != width || Window::height() != height) {
    width   = Window::width();
    height  = Window::height();

    centreX = Window::width() / 2;
    centreY = Window::height() / 2;

    aspect  = isFixedAspect ? aspect : float(width) / float(height);

    ui::ui.root->width  = camera.width;
    ui::ui.root->height = camera.height;
    ui::ui.root->realign();
  }
}

void Camera::update()
{
  updateReferences();

  if (proxy != nullptr) {
    proxy->update();
  }

  horizPlane  = coeff * mag * MIN_DISTANCE;
  vertPlane   = aspect * horizPlane;
  shakeRot    = Quat::fastSlerp(Quat::ID, shakeRot, SHAKE_SUPPRESSION);
  flashColour = Math::mix(Mat4::ID, flashColour, FLASH_SUPPRESSION);
}

void Camera::reset()
{
  rot         = Quat::ID;
  mag         = 1.0f;
  p           = Point::ORIGIN;
  velocity    = Vec3::ZERO;

  desiredRot  = Quat::ID;
  shakeRot    = Quat::ID;
  desiredMag  = 1.0f;
  desiredPos  = Point::ORIGIN;
  oldPos      = Point::ORIGIN;

  relH        = 0.0f;
  relV        = 0.0f;

  rotMat      = Mat4::rotation(rot);
  rotTMat     = ~rotTMat;

  colour      = Mat4::ID;
  baseColour  = Mat4::ID;
  nvColour    = BotClass::NV_COLOUR;
  flashColour = Mat4::ID;

  right       = Vec3(+rotMat.x);
  up          = Vec3(+rotMat.y);
  at          = Vec3(-rotMat.z);

  object      = -1;
  objectObj   = nullptr;
  entity      = -1;
  entityObj   = nullptr;
  bot         = -1;
  botObj      = nullptr;
  vehicle     = -1;
  vehicleObj  = nullptr;

  switchableUnits.clear();
  switchableUnits.trim();

  allowReincarnation = true;
  nightVision        = false;
  isExternal         = true;

  state    = NONE;
  newState = NONE;

  strategic.reset();
  unit.reset();
  cinematic.reset();

  if (proxy != nullptr) {
    proxy->end();
    proxy = nullptr;
  }
}

void Camera::read(const Json& json)
{
  p          = json["position"].get(Point::ORIGIN);
  rot        = json["rotation"].get(Quat::ID);

  desiredRot = rot;
  desiredPos = p;
  oldPos     = p;

  newState   = State(json["state"].get(STRATEGIC));

  strategic.read(json["strategic"]);
}

void Camera::read(Stream* is)
{
  rot        = is->read<Quat>();
  mag        = is->readFloat();
  p          = is->read<Point>();
  velocity   = Vec3::ZERO;

  desiredRot = rot;
  desiredMag = mag;
  desiredPos = p;
  oldPos     = p;

  relH       = is->readFloat();
  relV       = is->readFloat();

  rotMat     = Mat4::rotation(rot);
  rotTMat    = ~rotMat;

  colour     = is->read<Mat4>();
  baseColour = is->read<Mat4>();
  nvColour   = is->read<Mat4>();

  right      = Vec3(+rotMat.x);
  up         = Vec3(+rotMat.y);
  at         = Vec3(-rotMat.z);

  object     = -1;
  objectObj  = nullptr;
  entity     = -1;
  entityObj  = nullptr;
  bot        = is->readInt();
  botObj     = orbis.obj<Bot>(bot);
  vehicle    = is->readInt();
  vehicleObj = orbis.obj<Vehicle>(vehicle);

  OZ_ASSERT(switchableUnits.isEmpty());

  int nSwitchableUnits = is->readInt();
  for (int i = 0; i < nSwitchableUnits; ++i) {
    switchableUnits.add(is->readInt());
  }

  allowReincarnation = is->readBool();
  nightVision        = is->readBool();
  isExternal         = is->readBool();

  state     = NONE;
  newState  = State(is->readInt());

  strategic.read(is);
  unit.read(is);
  cinematic.read(is);
}

Json Camera::write() const
{
  Json json(Json::OBJECT);

  json.add("position", p);
  json.add("rotation", rot);
  json.add("state", state);

  json.add("strategic", strategic.write());

  return json;
}

void Camera::write(Stream* os) const
{
  os->write<Quat>(desiredRot);
  os->writeFloat(desiredMag);
  os->write<Point>(desiredPos);

  os->writeFloat(relH);
  os->writeFloat(relV);

  os->write<Mat4>(colour);
  os->write<Mat4>(baseColour);
  os->write<Mat4>(nvColour);

  os->writeInt(bot);
  os->writeInt(vehicle);

  os->writeInt(switchableUnits.size());
  for (int switchableUnit : switchableUnits) {
    os->writeInt(switchableUnit);
  }

  os->writeBool(allowReincarnation);
  os->writeBool(nightVision);
  os->writeBool(isExternal);

  os->writeInt(state);

  strategic.write(os);
  unit.write(os);
  cinematic.write(os);
}

void Camera::init()
{
  width         = Window::width();
  height        = Window::height();
  centreX       = Window::width() / 2;
  centreY       = Window::height() / 2;

  float angle   = Math::rad(appConfig.include("camera.angle", 80.0f).get(0.0f));
  aspect        = appConfig.include("camera.aspect", 0.0f).get(0.0f);
  isFixedAspect = aspect != 0.0f;
  aspect        = isFixedAspect ? aspect : float(width) / float(height);
  coeff         = Math::tan(angle / 2.0f);

  reset();
}

void Camera::destroy()
{}

Camera camera;

}
