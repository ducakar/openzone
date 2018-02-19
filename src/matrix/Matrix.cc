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

#include <matrix/Matrix.hh>

#include <matrix/LuaMatrix.hh>
#include <matrix/Physics.hh>
#include <matrix/Synapse.hh>
#include <matrix/Vehicle.hh>

namespace oz
{

void Matrix::update()
{
  maxStructs  = max(maxStructs,  Struct::pool.size());
  maxEvents   = max(maxEvents,   Object::Event::pool.size());
  maxObjects  = max(maxObjects,  Object::pool.size());
  maxDynamics = max(maxDynamics, Dynamic::pool.size());
  maxWeapons  = max(maxWeapons,  Weapon::pool.size());
  maxBots     = max(maxBots,     Bot::pool.size());
  maxVehicles = max(maxVehicles, Vehicle::pool.size());
  maxFrags    = max(maxFrags,    Frag::mpool.size());

  for (int i = 0; i < Orbis::MAX_OBJECTS; ++i) {
    Object* obj = orbis.obj(i);

    if (obj != nullptr) {
      // If this is cleared on the object's update, we may also remove effects added by other
      // objects updated before it.
      obj->events.free();

      // We don't remove objects as they get destroyed but on the next update, so the destruction
      // sound and other effects can be played on an object's destruction.
      if (obj->flags & Object::DESTROYED_BIT) {
        synapse.remove(obj);
      }
    }
  }

  for (int i = 0; i < Orbis::MAX_STRUCTS; ++i) {
    Struct* str = orbis.str(i);

    if (str == nullptr) {
      continue;
    }

    OZ_ASSERT(str->life >= 0.0f);

    if (str->demolishing >= 1.0f) {
      synapse.remove(str);
    }
    else if (str->life == 0.0f && str->demolishing == 0.0f) {
      str->destroy();
    }
    else {
      str->update();
    }
  }

  for (int i = 0; i < Orbis::MAX_OBJECTS; ++i) {
    Object* obj = orbis.obj(i);

    if (obj == nullptr) {
      continue;
    }

    OZ_ASSERT(obj->life >= 0.0f);

    if (obj->life == 0.0f) {
      obj->destroy();
    }
    else {
      // clear inventory of invalid references
      if (!obj->items.isEmpty()) {
        for (int j = 0; j < obj->items.size();) {
          if (orbis.obj(obj->items[j]) == nullptr) {
            obj->items.erase(j);
          }
          else {
            ++j;
          }
        }
      }

      obj->update();

      // objects should not remove themselves within onUpdate()
      OZ_ASSERT(orbis.obj(i) != nullptr);

      if (obj->flags & Object::DYNAMIC_BIT) {
        Dynamic* dyn = static_cast<Dynamic*>(obj);

        OZ_ASSERT((dyn->parent != -1) == (dyn->cell == nullptr));

        if (dyn->cell == nullptr) {
          if (orbis.obj(dyn->parent) == nullptr) {
            synapse.remove(dyn);
          }
        }
        else {
          physics.updateObj(dyn);

          // remove on velocity overflow
          if (dyn->velocity.sqN() > MAX_VELOCITY2) {
            synapse.remove(dyn);
          }
        }
      }
    }
  }

  for (int i = 0; i < Orbis::MAX_FRAGS; ++i) {
    Frag* frag = orbis.frag(i);

    if (frag == nullptr) {
      continue;
    }

    if (frag->life <= 0.0f || frag->velocity.sqN() > MAX_VELOCITY2) {
      synapse.remove(frag);
    }
    else {
      frag->life -= Timer::TICK_TIME;
      physics.updateFrag(frag);
    }
  }

  // rotate freeing/waiting/available indices
  orbis.update();
}

void Matrix::read(Stream* is)
{
  Log::println("Reading Matrix {");
  Log::indent();

  timer.nTicks = is->readUInt64();
  timer.duration = Duration(is->readInt64());
  orbis.read(is);
  physics.gravity = is->readFloat();

  Log::unindent();
  Log::println("}");
}

void Matrix::read(const Json& json)
{
  Log::println("Reading Matrix {");
  Log::indent();

  orbis.read(json);

  Log::unindent();
  Log::println("}");
}

void Matrix::write(Stream* os) const
{
  os->writeUInt64(timer.nTicks);
  os->writeInt64(timer.duration.ns());
  orbis.write(os);
  os->writeFloat(physics.gravity);
}

Json Matrix::write() const
{
  return orbis.write();
}

void Matrix::load()
{
  Log::print("Loading Matrix ...");

  maxStructs  = 0;
  maxEvents   = 0;
  maxObjects  = 0;
  maxDynamics = 0;
  maxWeapons  = 0;
  maxBots     = 0;
  maxVehicles = 0;
  maxFrags    = 0;

  orbis.load();
  synapse.load();

  physics.gravity = -9.81f;

  Log::printEnd(" OK");
}

void Matrix::unload()
{
  Log::println("Unloading Matrix {");
  Log::indent();

  Log::println("Static memory usage  %.2f MiB", float(sizeof(orbis)) / (1024.0f * 1024.0f));

  Log::println("Peak instances {");
  Log::indent();
  Log::println("%6d  structures",      maxStructs);
  Log::println("%6d  object events",   maxEvents);
  Log::println("%6d  static objects",  maxObjects);
  Log::println("%6d  dynamic objects", maxDynamics);
  Log::println("%6d  weapon objects",  maxWeapons);
  Log::println("%6d  bot objects",     maxBots);
  Log::println("%6d  vehicle objects", maxVehicles);
  Log::println("%6d  fragments",       maxFrags);
  Log::unindent();
  Log::println("}");

  synapse.unload();
  orbis.unload();

  Log::unindent();
  Log::println("}");
}

void Matrix::init()
{
  Log::println("Initialising Matrix {");
  Log::indent();

  luaMatrix.init();
  orbis.init();

  Log::unindent();
  Log::println("}");
}

void Matrix::destroy()
{
  Log::println("Destroying Matrix {");
  Log::indent();

  orbis.destroy();
  luaMatrix.destroy();

  Log::unindent();
  Log::println("}");
}

Matrix matrix;

}
