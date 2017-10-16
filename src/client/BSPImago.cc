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

#include <client/BSPImago.hh>

#include <client/Context.hh>

namespace oz::client
{

BSPImago::BSPImago(const BSP* bsp)
  : model("@bsp/" + bsp->name + ".ozcModel")
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

    for (int i = 0; i < str->entities.size(); ++i) {
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

  int modelEnd = is.readInt();

  is.seek(modelEnd);

  int nLeaves = is.readInt();

  leafClusters.resize(nLeaves);

  for (int i = 0; i < leafClusters.size(); ++i) {
    leafClusters[i] = is.readInt();
  }

  nClusters      = is.readInt();
  nClusterBits   = is.readInt();

  clusters.resize(nClusters * nClusterBits);
  is.readBitset(clusters);

  waterFogColour = is.read<Vec4>();
  lavaFogColour  = is.read<Vec4>();
}

void BSPImago::load()
{
  model.load();
}

}
