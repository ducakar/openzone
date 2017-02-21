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

#include <client/ui/HudArea.hh>

#include <matrix/Physics.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{
namespace ui
{

const float HudArea::VEHICLE_DIM      = VEHICLE_SIZE / 2.0f;
const float HudArea::CROSS_FADE_COEFF = 8.0f;

void HudArea::drawBotCrosshair()
{
  const Bot*      me      = camera.botObj;
  const BotClass* myClazz = static_cast<const BotClass*>(camera.botObj->clazz);

  float delta  = max(1.0f - abs(camera.unit.headRot.w), 0.0f);
  float alpha  = 1.0f - CROSS_FADE_COEFF * Math::sqrt(delta);
  float life   = max(0.0f, 2.0f * me->life / myClazz->life - 1.0f);
  Vec4  colour = Math::mix(Vec4(1.00f, 0.50f, 0.25f, alpha),
                           Vec4(1.00f, 1.00f, 1.00f, alpha),
                           life);

  shape.colour(colour);
  glBindTexture(GL_TEXTURE_2D, style.images.crosshair);
  shape.fill(crossIconX, crossIconY, ICON_SIZE, ICON_SIZE);
  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

  if (me->parent == -1 && (camera.object != -1 || camera.entity != -1)) {
    const Object*      obj      = camera.objectObj;
    const ObjectClass* objClazz = obj == nullptr ? nullptr : obj->clazz;
    const Dynamic*     dyn      = static_cast<const Dynamic*>(obj);
    const Bot*         bot      = static_cast<const Bot*>(obj);
    const Entity*      ent      = camera.entityObj;
    const EntityClass* entClazz = ent == nullptr ? nullptr : ent->clazz;

    // it might happen that bot itself is tagged object for a frame when switching from freecam
    // into a bot
    if (obj == camera.botObj) {
      return;
    }

    if (ent != nullptr) {
      title.setText("%s", entClazz->title.c());
      title.draw(this);

      shape.colour(1.0f, 1.0f, 1.0f, 1.0f);

      if (entClazz->target != -1 && ent->key >= 0) {
        glBindTexture(GL_TEXTURE_2D, style.images.use);
        shape.fill(rightIconX, rightIconY, ICON_SIZE, ICON_SIZE);
      }

      if (ent->key < 0) {
        glBindTexture(GL_TEXTURE_2D, style.images.locked);
        shape.fill(bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE);
      }
      else if (ent->key > 0) {
        glBindTexture(GL_TEXTURE_2D, style.images.unlocked);
        shape.fill(bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE);
      }
    }
    else {
      if (obj->flags & Object::BOT_BIT) {
        life = max(0.0f, 2.0f * obj->life / objClazz->life - 1.0f);
      }
      else {
        life = obj->life / objClazz->life;
      }

      float status = obj->status();

      taggedLife.draw(this, healthBarX, healthBarY + 14, ICON_SIZE + 16, 10, life);
      if (status >= 0.0f) {
        taggedStatus.draw(this, healthBarX, healthBarY + 7, ICON_SIZE + 16, 8, status);
      }

      title.setText("%s", obj->title().c());
      title.draw(this);

      shape.colour(1.0f, 1.0f, 1.0f, 1.0f);

      if (obj->flags & Object::BROWSABLE_BIT) {
        glBindTexture(GL_TEXTURE_2D, style.images.browse);
        shape.fill(leftIconX, leftIconY, ICON_SIZE, ICON_SIZE);
      }
      if ((obj->flags & Object::USE_FUNC_BIT) &&
          !(obj->flags & (Object::WEAPON_BIT | Object::VEHICLE_BIT)))
      {

        glBindTexture(GL_TEXTURE_2D,
                      obj->flags & Object::USE_FUNC_BIT ? style.images.use : style.images.device);
        shape.fill(rightIconX, rightIconY, ICON_SIZE, ICON_SIZE);
      }

      if (!(obj->flags & Object::SOLID_BIT)) {
        glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
        return;
      }

      if (obj->flags & Object::VEHICLE_BIT) {
        const Vehicle* vehicle = static_cast<const Vehicle*>(obj);

        if (vehicle->pilot == -1) {
          glBindTexture(GL_TEXTURE_2D, style.images.mount);
          shape.fill(rightIconX, rightIconY, ICON_SIZE, ICON_SIZE);
        }
      }
      else if (obj->flags & Object::WEAPON_BIT) {
        if (me->canEquip(static_cast<const Weapon*>(obj))) {
          glBindTexture(GL_TEXTURE_2D, style.images.equip);
          shape.fill(rightIconX, rightIconY, ICON_SIZE, ICON_SIZE);
        }
      }

      if (obj->flags & Object::ITEM_BIT) {
        glBindTexture(GL_TEXTURE_2D, style.images.take);
        shape.fill(leftIconX, leftIconY, ICON_SIZE, ICON_SIZE);
      }

      if (me->cargo == -1 && me->weapon == -1 &&
          (obj->flags & Object::DYNAMIC_BIT) &&
          abs(dyn->mass * physics.gravity) <= myClazz->grabWeight &&
          // not climbing or on a ladder
          !(me->state & (Bot::LADDER_BIT | Bot::LEDGE_BIT)) &&
          // if it is not a bot that is holding something
          (!(dyn->flags & Object::BOT_BIT) || bot->cargo == -1))
      {
        float dimX = dyn->dim.x + dyn->dim.x;
        float dimY = dyn->dim.y + dyn->dim.y;
        float dist = Math::sqrt(dimX*dimX + dimY*dimY) + Bot::GRAB_EPSILON;

        if (dist <= myClazz->reachDist) {
          glBindTexture(GL_TEXTURE_2D, style.images.lift);
          shape.fill(bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE);
        }
      }
      if (camera.botObj->cargo != -1) {
        glBindTexture(GL_TEXTURE_2D, style.images.grab);
        shape.fill(bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE);
      }
    }
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
  }
}

void HudArea::drawBotStatus()
{
  const Bot*      bot      = camera.botObj;
  const BotClass* botClazz = static_cast<const BotClass*>(camera.botObj->clazz);

  float life    = max(0.0f, 2.0f * bot->life / botClazz->life - 1.0f);
  float stamina = bot->stamina / botClazz->stamina;

  botLife.draw(this, life);
  botStamina.draw(this, stamina);

  const Weapon* weaponObj = orbis.obj<const Weapon>(bot->weapon);

  if (weaponObj != nullptr) {
    Pos2 pos = alignChild(style.botWeapon.x, style.botWeapon.y,
                          style.botWeapon.w, style.botWeapon.h);

    shape.colour(style.colours.frame);
    shape.fill(pos.x, pos.y, style.botWeapon.w, style.botWeapon.h);

    weaponName.setPosition(pos.x + 4, pos.y + 2);
    weaponRounds.setPosition(pos.x + style.botWeapon.w - 4, pos.y + 2);

    weaponName.setText("%s", weaponObj->clazz->title.c());

    if (weaponObj->nRounds == -1) {
      weaponRounds.setText("∞");
    }
    else {
      weaponRounds.setText("%d", weaponObj->nRounds);
    }

    weaponName.draw(this);
    weaponRounds.draw(this);
  }
}

void HudArea::drawVehicleStatus()
{
  if (orbis.obj(camera.botObj->parent) == nullptr) {
    vehicleModel->setModel(-1);
    return;
  }

  const Bot*          bot      = camera.botObj;
  const Vehicle*      vehicle  = orbis.obj<const Vehicle>(bot->parent);
  const VehicleClass* vehClazz = static_cast<const VehicleClass*>(vehicle->clazz);

  // HACK close gap between vehicle model and weapons.
  int modelBiasY = (4 - vehClazz->nWeapons) * (style.vehicleWeapon[0].h + 4);

  vehicleModel->setModel(vehClazz->imagoModel);
  vehicleModel->realign();
  vehicleModel->y -= modelBiasY;

  float hull = vehicle->life / vehClazz->life;
  float fuel = vehicle->fuel / vehClazz->fuel;

  vehicleHull.draw(this, hull);
  vehicleFuel.draw(this, fuel);

  for (int i = 0; i < vehClazz->nWeapons; ++i) {
    int   labelIndex  = vehClazz->nWeapons - i - 1;
    Text& nameLabel   = vehicleWeaponNames[labelIndex];
    Text& roundsLabel = vehicleWeaponRounds[labelIndex];
    const Style::Area& areaStyle = style.vehicleWeapon[labelIndex];

    Pos2 pos = alignChild(areaStyle.x, areaStyle.y, areaStyle.w, areaStyle.h);

    if (i == vehicle->weapon) {
      shape.colour(style.colours.frame);
      shape.fill(pos.x, pos.y, areaStyle.w, areaStyle.h);
    }

    nameLabel.setPosition(pos.x + 2, pos.y + 2);
    roundsLabel.setPosition(pos.x + areaStyle.w - 4, pos.y + 2);

    nameLabel.setText("%s", vehClazz->weaponTitles[i].c());

    if (vehicle->nRounds[i] == -1) {
      roundsLabel.setText("∞");
    }
    else {
      roundsLabel.setText("%d", vehicle->nRounds[i]);
    }

    nameLabel.draw(this);
    roundsLabel.draw(this);
  }
}

void HudArea::onRealign()
{
  width        = camera.width;
  height       = camera.height;

  crossIconX   = (width - ICON_SIZE) / 2;
  crossIconY   = (height - ICON_SIZE) / 2;
  leftIconX    = crossIconX - ICON_SIZE;
  leftIconY    = crossIconY;
  rightIconX   = crossIconX + ICON_SIZE;
  rightIconY   = crossIconY;
  bottomIconX  = crossIconX;
  bottomIconY  = crossIconY - ICON_SIZE;
  healthBarX   = crossIconX - 8;
  healthBarY   = crossIconY + ICON_SIZE;
  descTextX    = width / 2;
  descTextY    = crossIconY + ICON_SIZE + 36;

  title.setPosition(descTextX, descTextY);
}

bool HudArea::onMouseEvent()
{
  return Area::passMouseEvents();
}

void HudArea::onDraw()
{
  if (camera.botObj == nullptr || (camera.botObj->state & Bot::DEAD_BIT)) {
    return;
  }

  drawBotCrosshair();
  drawBotStatus();
  drawVehicleStatus();
  drawChildren();
}

HudArea::HudArea()
  : Area(camera.width, camera.height),
    title(0, 0, 0, ALIGN_CENTRE, &style.largeFont, ""),
    weaponName(0, 0, 0, ALIGN_LEFT, &style.largeFont, ""),
    weaponRounds(0, 0, 0, ALIGN_RIGHT, &style.largeFont, "∞"),
    taggedLife(&style.taggedLife),
    taggedStatus(&style.taggedStatus),
    botLife(&style.botLife),
    botStamina(&style.botStamina),
    vehicleHull(&style.vehicleHull),
    vehicleFuel(&style.vehicleFuel),
    vehicleModel(nullptr)
{
  flags |= UPDATE_BIT | PINNED_BIT;

  for (int i = 0; i < Vehicle::MAX_WEAPONS; ++i) {
    vehicleWeaponNames[i]  = Text(0, 0, 0, ALIGN_LEFT, &style.largeFont, "");
    vehicleWeaponRounds[i] = Text(0, 0, 0, ALIGN_RIGHT, &style.largeFont, "∞");
  }

  vehicleModel = new ModelField(nullptr, style.vehicleField.h);
  add(vehicleModel, style.vehicleField.x, style.vehicleField.y);
}

}
}
}
