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

#include <matrix/Synapse.hh>

#include <matrix/Liber.hh>
#include <matrix/Collider.hh>
#include <matrix/Bot.hh>

namespace oz
{

bool Synapse::use(Bot* user, Object* target)
{
  return target->use(user);
}

bool Synapse::trigger(Entity* target)
{
  return target->trigger();
}

bool Synapse::lock(Bot* user, Entity* target)
{
  return target->lock(user);
}

bool Synapse::transferItem(Object* source, Dynamic* item, Object* target)
{
  OZ_ASSERT(source->items.contains(item->index));

  if (target->items.size() == target->clazz->nItems) {
    return false;
  }

  item->parent = target->index;
  target->items.add(item->index);
  source->items.exclude(item->index);

  if (source->flags & Object::BOT_BIT) {
    Bot* bot = static_cast<Bot*>(source);

    bot->weapon = bot->weapon == item->index ? -1 : bot->weapon;
  }

  return true;
}

bool Synapse::takeItem(Object* container, Dynamic* item)
{
  OZ_ASSERT(item->cell != nullptr);

  if (container->items.size() == container->clazz->nItems) {
    return false;
  }

  item->parent = container->index;
  container->items.add(item->index);
  cut(item);

  return true;
}

bool Synapse::dropItem(Object* container, Dynamic* item, const Point& p, const Vec3& velocity)
{
  OZ_ASSERT(container->items.contains(item->index));

  item->p = p;

  if (collider.overlaps(item)) {
    return false;
  }

  item->parent = -1;
  container->items.exclude(item->index);
  put(item);

  if (container->flags & Object::BOT_BIT) {
    Bot* bot = static_cast<Bot*>(container);

    bot->weapon = bot->weapon == item->index ? -1 : bot->weapon;
  }

  item->velocity = velocity;
  item->momentum = velocity;

  return true;
}

void Synapse::put(Dynamic* obj)
{
  OZ_ASSERT(obj->index != -1 && obj->cell == nullptr && obj->parent == -1);

  orbis.position(obj);

  putObjects.add(obj->index);
}

void Synapse::cut(Dynamic* obj)
{
  OZ_ASSERT(obj->index != -1 && obj->cell != nullptr && obj->parent != -1);

  obj->flags   &= ~(Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK);
  obj->lower    = -1;
  obj->velocity = Vec3::ZERO;
  obj->momentum = Vec3::ZERO;

  orbis.unposition(obj);

  cutObjects.add(obj->index);
}

Struct* Synapse::add(const BSP* bsp, const Point& p, Heading heading, bool empty)
{
  Struct* str = orbis.add(bsp, p, heading);
  if (str == nullptr) {
    return nullptr;
  }

  if (!orbis.position(str)) {
    orbis.remove(str);
    delete str;
    return nullptr;
  }

  addedStructs.add(str->index);

  if (!empty) {
    for (int i = 0; i < bsp->nBoundObjects; ++i) {
      const BSP::BoundObject& boundObj = bsp->boundObjects[i];

      Point   objPos     = str->toAbsoluteCS(boundObj.pos);
      Heading objHeading = Heading((str->heading + boundObj.heading) % 4);

      Object* obj = orbis.add(boundObj.clazz, objPos, objHeading);
      if (obj == nullptr) {
        continue;
      }

      orbis.position(obj);
      str->boundObjects.add(obj->index);

      addedObjects.add(obj->index);
    }
  }

  return str;
}

Object* Synapse::add(const ObjectClass* clazz, const Point& p, Heading heading, bool empty)
{
  Object* obj = orbis.add(clazz, p, heading);
  if (obj == nullptr) {
    return nullptr;
  }

  orbis.position(obj);

  addedObjects.add(obj->index);

  if (!empty) {
    for (const ObjectClass* defaultItem : obj->clazz->defaultItems) {
      Heading itemHeading = Heading(Math::rand(4));
      Dynamic* item = static_cast<Dynamic*>(orbis.add(defaultItem, Point::ORIGIN, itemHeading));

      if (item == nullptr) {
        continue;
      }

      obj->items.add(item->index);
      item->parent = obj->index;

      addedObjects.add(item->index);
    }

    if (obj->flags & Object::BOT_BIT) {
      const BotClass* botClazz = static_cast<const BotClass*>(obj->clazz);
      Bot* bot = static_cast<Bot*>(obj);

      if (uint(botClazz->weaponItem) < uint(obj->items.size())) {
        bot->weapon = bot->items[botClazz->weaponItem];
      }
    }
  }

  return obj;
}

Frag* Synapse::add(const FragPool* pool, const Point& p, const Vec3& velocity)
{
  Frag* frag = orbis.add(pool, p, velocity);
  if (frag == nullptr) {
    return nullptr;
  }

  orbis.position(frag);
  addedFrags.add(frag->index);

  return frag;
}

void Synapse::gen(const FragPool* pool, int nFrags, const Bounds& bb, const Vec3& velocity)
{
  for (int i = 0; i < nFrags; ++i) {
    // spawn the frag somewhere in the upper half of the structure's bounding box
    Point fragPos = Point(bb.mins.x + Math::rand() * (bb.maxs.x - bb.mins.x),
                          bb.mins.y + Math::rand() * (bb.maxs.y - bb.mins.y),
                          bb.mins.z + Math::rand() * (bb.maxs.z - bb.mins.z));

    Frag*  frag = add(pool, fragPos, velocity);
    if (frag == nullptr) {
      continue;
    }

    frag->velocity += Vec3(Math::normalRand() * pool->velocitySpread,
                           Math::normalRand() * pool->velocitySpread,
                           Math::normalRand() * pool->velocitySpread);

    frag->life     += Math::centralRand() * pool->lifeSpread;
  }
}

Struct* Synapse::addStruct(const char* bspName, const Point& p, Heading heading, bool empty)
{
  return add(liber.bsp(bspName), p, heading, empty);
}

Object* Synapse::addObject(const char* className, const Point& p, Heading heading, bool empty)
{
  return add(liber.objClass(className), p, heading, empty);
}

Frag* Synapse::addFrag(const char* poolName, const Point& p, const Vec3& velocity)
{
  return add(liber.fragPool(poolName), p, velocity);
}

void Synapse::genFrags(const char* poolName, int nFrags, const Bounds& bb, const Vec3& velocity)
{
  gen(liber.fragPool(poolName), nFrags, bb, velocity);
}

void Synapse::remove(Struct* str)
{
  OZ_ASSERT(str->index != -1);

  for (int boundObjIndex : str->boundObjects) {
    Object* boundObj = orbis.obj(boundObjIndex);

    if (boundObj != nullptr) {
      remove(boundObj);
    }
  }

  removedStructs.add(str->index);

  List<Object*> overlappingObjs;
  collider.getOverlaps(str->toAABB(), nullptr, &overlappingObjs, 2.0f * EPSILON);

  for (Object* obj : overlappingObjs) {
    if (obj->flags & Object::DYNAMIC_BIT) {
      obj->flags &= ~Object::DISABLED_BIT;
      obj->flags |= Object::ENABLE_BIT;
    }
  }

  orbis.unposition(str);
  orbis.remove(str);
}

void Synapse::remove(Object* obj)
{
  OZ_ASSERT(obj->index != -1);

  for (int itemIndex : obj->items) {
    Object* item = orbis.obj(itemIndex);

    if (item != nullptr) {
      remove(item);
    }
  }

  removedObjects.add(obj->index);

  if (obj->cell != nullptr) {
    List<Object*> overlappingObjs;
    collider.getOverlaps(*obj, nullptr, &overlappingObjs, 2.0f * EPSILON);

    for (Object* sObj : overlappingObjs) {
      if (sObj->flags & Object::DYNAMIC_BIT) {
        sObj->flags &= ~Object::DISABLED_BIT;
        sObj->flags |= Object::ENABLE_BIT;
      }
    }

    orbis.unposition(obj);
  }
  orbis.remove(obj);
}

void Synapse::remove(Frag* frag)
{
  OZ_ASSERT(frag->index != -1);

  removedFrags.add(frag->index);

  orbis.unposition(frag);
  orbis.remove(frag);
}

void Synapse::removeStruct(int index)
{
  OZ_ASSERT(index != -1);

  Struct* str = orbis.str(index);

  if (str != nullptr) {
    remove(str);
  }
}

void Synapse::removeObject(int index)
{
  OZ_ASSERT(index != -1);

  Object* obj = orbis.obj(index);

  if (obj != nullptr) {
    remove(obj);
  }
}

void Synapse::removeFrag(int index)
{
  OZ_ASSERT(index != -1);

  Frag* frag = orbis.frag(index);

  if (frag != nullptr) {
    remove(frag);
  }
}

void Synapse::update()
{
  putObjects.clear();
  cutObjects.clear();

  addedStructs.clear();
  addedObjects.clear();
  addedFrags.clear();

  removedStructs.clear();
  removedObjects.clear();
  removedFrags.clear();
}

void Synapse::load()
{
  putObjects.reserve(32);
  cutObjects.reserve(32);

  addedStructs.reserve(4 * 16);
  addedObjects.reserve(64 * 16);
  addedFrags.reserve(128 * 8);

  removedStructs.reserve(4);
  removedObjects.reserve(64);
  removedFrags.reserve(128);
}

void Synapse::unload()
{
  putObjects.clear();
  putObjects.trim();
  cutObjects.clear();
  cutObjects.trim();

  addedStructs.clear();
  addedStructs.trim();
  addedObjects.clear();
  addedObjects.trim();
  addedFrags.clear();
  addedFrags.trim();

  removedStructs.clear();
  removedStructs.trim();
  removedObjects.clear();
  removedObjects.trim();
  removedFrags.clear();
  removedFrags.trim();
}

Synapse synapse;

}
