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
 * @file client/SMMVehicleImago.hh
 */

#pragma once

#include <client/Imago.hh>
#include <client/Model.hh>

namespace oz::client
{

class SMMVehicleImago : public Imago
{
private:

  Model* model;

private:

  using Imago::Imago;

public:

  static Pool<SMMVehicleImago> pool;

  static Imago* create(const Object* obj_);

  ~SMMVehicleImago() override;

  OZ_NO_COPY(SMMVehicleImago)
  OZ_NO_MOVE(SMMVehicleImago)

  void draw(const Imago* parent) override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
