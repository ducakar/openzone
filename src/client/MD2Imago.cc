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

#include <client/MD2Imago.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz::client
{

Pool<MD2Imago> MD2Imago::pool(256);

MD2Imago::~MD2Imago()
{
  context.releaseModel(clazz->imagoModel);
}

Imago* MD2Imago::create(const Object* obj_)
{
  OZ_ASSERT(obj_->flags & (Object::BOT_BIT | Object::VEHICLE_BIT));

  if (obj_->flags & Object::VEHICLE_BIT) {
    const Vehicle* veh   = static_cast<const Vehicle*>(obj_);
    MD2Imago*      imago = new MD2Imago(veh);

    imago->flags = Imago::MD2MODEL_BIT;
    imago->model = context.requestModel(obj_->clazz->imagoModel);
    imago->h     = veh->h;

    return imago;
  }
  else {
    const Bot*     bot   = static_cast<const Bot*>(obj_);
    MD2Imago*      imago = new MD2Imago(bot);

    imago->flags = Imago::MD2MODEL_BIT;
    imago->model = context.requestModel(obj_->clazz->imagoModel);
    imago->h     = bot->h;

    return imago;
  }
}

void MD2Imago::draw(const Imago* parent)
{
  if (!model->isLoaded()) {
    return;
  }

  if (obj->flags & Object::VEHICLE_BIT) {
    const Vehicle* veh = static_cast<const Vehicle*>(obj);

    anim.advance();

    if (veh->index == camera.vehicle && !camera.isExternal) {
      h = veh->h;
    }
    else {
      h = angleWrap(h + TURN_SMOOTHING_COEF * angleDiff(veh->h, h));

      tf.model = Mat4::translation(obj->p - Point::ORIGIN);
      tf.model.rotateZ(h);

      model->scheduleMD2Anim(&anim, Model::SCENE_QUEUE);
    }
  }
  else {
    const Bot*      bot    = static_cast<const Bot*>(obj);
    const BotClass* clazz  = static_cast<const BotClass*>(bot->clazz);
    const Weapon*   weapon = orbis.obj<const Weapon>(bot->weapon);

    anim.advance();

    if (bot->state & Bot::DEAD_BIT) {
      if (parent == nullptr) {
        Vec3 t = Vec3(obj->p.x, obj->p.y, obj->p.z + clazz->dim.z - clazz->corpseDim.z);

        tf.model = Mat4::translation(t);
        tf.model.rotateZ(h);

        tf.colour.w.w = min(bot->life * 8.0f / clazz->life, 1.0f);
      }

      model->scheduleMD2Anim(&anim, Model::SCENE_QUEUE);
    }
    else if (bot->index == camera.bot && !camera.isExternal) {
      h = bot->h;

      if (parent == nullptr && weapon != nullptr) {
        tf.model = Mat4::translation(obj->p - Point::ORIGIN);
        tf.model.rotateZ(bot->h);

        tf.model.translate(Vec3(0.0f, 0.0f, +bot->camZ));
        tf.model.rotateX(bot->v - Math::TAU / 4.0f);
        tf.model.translate(Vec3(0.0f, 0.0f, -bot->camZ));

        glDepthFunc(GL_ALWAYS);

        context.drawImago(weapon, this);

        glDepthFunc(GL_LEQUAL);
      }
    }
    else {
      if (parent == nullptr) {
        h = angleWrap(h + TURN_SMOOTHING_COEF * angleDiff(bot->h, h));

        tf.model = Mat4::translation(obj->p - Point::ORIGIN);
        tf.model.rotateZ(h);

        if (bot->state & Bot::CROUCHING_BIT) {
          tf.model.translate(Vec3(0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z));
        }
      }

      model->scheduleMD2Anim(&anim, Model::SCENE_QUEUE);

      if (parent == nullptr && weapon != nullptr) {
        context.drawImago(weapon, this);
      }
    }

    tf.colour.w.w = 1.0f;
  }
}

}
