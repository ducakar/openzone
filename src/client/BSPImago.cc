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
 * @file client/BSPImago.cc
 */

#include <client/BSPImago.hh>

#include <client/Context.hh>

namespace oz
{
namespace client
{

BSPImago::BSPImago(const BSP* bsp) :
  model("@bsp/" + bsp->name + ".ozcModel")
{}

BSPImago::~BSPImago()
{
  model.unload();
}

void BSPImago::schedule(const Struct* str, Model::QueueType queue)
{
  if (str != nullptr) {
    tf.model = Mat4::translation(str->p - Point::ORIGIN);
    tf.model.rotateZ(float(str->heading) * Math::TAU / 4.0f);

    for (int i = 0; i < str->entities.length(); ++i) {
      const Entity& entity = str->entities[i];

      tf.push();
      tf.model.translate(entity.offset);

      model.schedule(i + 1, queue);

      if (entity.clazz->model != -1) {
        Model* entityModel = context.requestModel(entity.clazz->model);;

        if (entityModel != nullptr && entityModel->isLoaded()) {
          tf.model = tf.model ^ entity.clazz->modelTransf;

          entityModel->schedule(0, queue);
        }

        context.releaseModel(entity.clazz->model);
      }

      tf.pop();
    }
  }

  model.schedule(0, queue);
}

void BSPImago::preload()
{
  const File* file = model.preload();
  Stream      is   = file->read(Endian::LITTLE);

  is.seek(is.available() - 2 * sizeof(float[4]));
  waterFogColour = is.readVec4();
  lavaFogColour  = is.readVec4();
}

void BSPImago::load()
{
  model.load();
}

}
}
