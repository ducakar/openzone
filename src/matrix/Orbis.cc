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

#include <matrix/Orbis.hh>

#include <matrix/Liber.hh>
#include <matrix/LuaMatrix.hh>
#include <matrix/Vehicle.hh>

namespace oz
{

namespace
{

static_assert(Orbis::CELLS * Cell::SIZE == Terra::QUADS * Terra::Quad::SIZE,
              "oz::Orbis and terrain size mismatch");

/*
 * Index reusing: when an entity (structure, object or fragment) is removed, there may still be
 * references to it from other entities or from render or sound subsystems; that's why every cycle
 * all references must be checked if the target still exists. To make sure all references can be
 * checked, one full world update must pass before a slot is reused. Otherwise an entity may be
 * removed and immediately after that another added into it's slot which might end in an inexpected
 * behaviour or even a crash if the new entity doesn't have the same type.
 *
 * To ensure that one full update passes between the updates we tag slots as 'freeing' when they are
 * freed and 'waiting' in the next world update. In the update after 'waiting' state they can be
 * reused again.
 */

int freeing = 0;
int waiting = 1;

int lastStructIndex = -1;
int lastObjectIndex = -1;
int lastFragIndex   = -1;

SBitset<Orbis::MAX_STRUCTS> pendingStructs[2];
SBitset<Orbis::MAX_OBJECTS> pendingObjects[2];
SBitset<Orbis::MAX_FRAGS>   pendingFrags[2];

}

int Orbis::allocStrIndex() const
{
  int index = lastStructIndex + 1;

  while (structs[index] != nullptr || pendingStructs[0].get(index) || pendingStructs[1].get(index))
  {
    if (index == lastStructIndex) {
      // We have wrapped around => no slots available.
      OZ_ASSERT(false);
      return -1;
    }

    index = (index + 1) % MAX_STRUCTS;
  }

  lastStructIndex = index;
  return index;
}

int Orbis::allocObjIndex() const
{
  int index = lastObjectIndex + 1;

  while (objects[index] != nullptr || pendingObjects[0].get(index) || pendingObjects[1].get(index))
  {
    if (index == lastObjectIndex) {
      // We have wrapped around => no slots available.
      OZ_ASSERT(false);
      return -1;
    }

    index = (index + 1) % MAX_OBJECTS;
  }

  lastObjectIndex = index;
  return index;
}

int Orbis::allocFragIndex() const
{
  int index = lastFragIndex + 1;

  while (frags[index] != nullptr || pendingFrags[0].get(index) || pendingFrags[1].get(index)) {
    if (index == lastFragIndex) {
      // We have wrapped around => no slots available.
      OZ_ASSERT(false);
      return -1;
    }

    index = (index + 1) % MAX_FRAGS;
  }

  lastFragIndex = index;
  return index;
}

bool Orbis::position(Struct* str)
{
  Span span = getInters(*str, EPSILON);

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      if (cells[x][y].structs.size() == cells[x][y].structs.capacity()) {
        OZ_ASSERT(false);
        return false;
      }
    }
  }

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      OZ_ASSERT(!cells[x][y].structs.contains(int16(str->index)));

      cells[x][y].structs.add(int16(str->index));
    }
  }

  return true;
}

void Orbis::unposition(Struct* str)
{
  Span span = getInters(*str, EPSILON);

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      OZ_ASSERT(cells[x][y].structs.contains(int16(str->index)));

      cells[x][y].structs.excludeUnordered(int16(str->index));
    }
  }
}

void Orbis::position(Object* obj)
{
  OZ_ASSERT(obj->cell == nullptr);

  Cell* cell = getCell(obj->p);

  obj->cell = cell;
  obj->prev[0] = nullptr;

  if (!cell->objects.isEmpty()) {
    cell->objects.first()->prev[0] = obj;
  }

  cell->objects.add(obj);
}

void Orbis::unposition(Object* obj)
{
  OZ_ASSERT(obj->cell != nullptr);

  Cell* cell = obj->cell;

  obj->cell = nullptr;

  if (obj->next[0] != nullptr) {
    obj->next[0]->prev[0] = obj->prev[0];
  }

  cell->objects.eraseAfter(obj, obj->prev[0]);
}

void Orbis::position(Frag* frag)
{
  OZ_ASSERT(frag->cell == nullptr);

  Cell* cell = getCell(frag->p);

  frag->cell = cell;
  frag->prev[0] = nullptr;

  if (!cell->frags.isEmpty()) {
    cell->frags.first()->prev[0] = frag;
  }

  cell->frags.add(frag);
}

void Orbis::unposition(Frag* frag)
{
  OZ_ASSERT(frag->cell != nullptr);

  Cell* cell = frag->cell;

  frag->cell = nullptr;

  if (frag->next[0] != nullptr) {
    frag->next[0]->prev[0] = frag->prev[0];
  }

  cell->frags.eraseAfter(frag, frag->prev[0]);
}

Struct* Orbis::add(const BSP* bsp, const Point& p, Heading heading)
{
  int index = allocStrIndex();
  if (index == -1) {
    return nullptr;
  }

  Struct* str = new Struct(bsp, index, p, heading);
  structs[index] = str;

  return str;
}

Object* Orbis::add(const ObjectClass* clazz, const Point& p, Heading heading)
{
  int index = allocObjIndex();
  if (index == -1) {
    return nullptr;
  }

  Object* obj = clazz->create(index, p, heading);
  objects[index] = obj;

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.registerObject(index);
  }

  return obj;
}

Frag* Orbis::add(const FragPool* pool, const Point& p, const Vec3& velocity)
{
  int index = allocFragIndex();
  if (index == -1) {
    return nullptr;
  }

  Frag* frag = new Frag(pool, index, p, velocity);
  frags[index] = frag;

  return frag;
}

void Orbis::remove(Struct* str)
{
  OZ_ASSERT(str->index != -1);

  pendingStructs[freeing].set(str->index);
  structs[str->index] = nullptr;
  delete str;
}

void Orbis::remove(Object* obj)
{
  OZ_ASSERT(obj->index != -1 && obj->cell == nullptr);

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.unregisterObject(obj->index);
  }

  pendingObjects[freeing].set(obj->index);
  objects[obj->index] = nullptr;
  delete obj;
}

void Orbis::remove(Frag* frag)
{
  OZ_ASSERT(frag->index != -1 && frag->cell == nullptr);

  pendingFrags[freeing].set(frag->index);
  frags[frag->index] = nullptr;
  delete frag;
}

void Orbis::reposition(Object* obj)
{
  OZ_ASSERT(obj->cell != nullptr);

  Cell* oldCell = obj->cell;
  Cell* newCell = getCell(obj->p);

  if (newCell != oldCell) {
    if (obj->next[0] != nullptr) {
      obj->next[0]->prev[0] = obj->prev[0];
    }

    oldCell->objects.eraseAfter(obj, obj->prev[0]);

    obj->cell = newCell;
    obj->prev[0] = nullptr;

    if (!newCell->objects.isEmpty()) {
      newCell->objects.first()->prev[0] = obj;
    }

    newCell->objects.add(obj);
  }
}

void Orbis::reposition(Frag* frag)
{
  OZ_ASSERT(frag->cell != nullptr);

  Cell* oldCell = frag->cell;
  Cell* newCell = getCell(frag->p);

  if (newCell != oldCell) {
    if (frag->next[0] != nullptr) {
      frag->next[0]->prev[0] = frag->prev[0];
    }

    oldCell->frags.eraseAfter(frag, frag->prev[0]);

    frag->cell = newCell;
    frag->prev[0] = nullptr;

    if (!newCell->frags.isEmpty()) {
      newCell->frags.first()->prev[0] = frag;
    }

    newCell->frags.add(frag);
  }
}

void Orbis::resetLastIndices()
{
  lastStructIndex = -1;
  lastObjectIndex = -1;
  lastFragIndex   = -1;
}

void Orbis::update()
{
  pendingStructs[waiting].clear();
  pendingObjects[waiting].clear();
  pendingFrags[waiting].clear();

  swap(freeing, waiting);

  caelum.update();
}

void Orbis::read(Stream* is)
{
  luaMatrix.read(is);

  caelum.read(is);
  terra.read(is);

  int nStructs = is->readInt();
  int nObjects = is->readInt();
  int nFrags   = is->readInt();

  for (int i = 0; i < nStructs; ++i) {
    const char* name = is->readString();
    const BSP*  bsp  = liber.bsp(name);

    Struct* str = new Struct(bsp, is);

    position(str);
    structs[str->index] = str;
  }

  for (int i = 0; i < nObjects; ++i) {
    const char*        name  = is->readString();
    const ObjectClass* clazz = liber.objClass(name);
    Object*            obj   = clazz->create(is);
    const Dynamic*     dyn   = static_cast<const Dynamic*>(obj);

    // No need to register objects since Lua state is being deserialised.

    if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent == -1) {
      position(obj);
    }
    objects[obj->index] = obj;
  }

  for (int i = 0; i < nFrags; ++i) {
    const char*     name = is->readString();
    const FragPool* pool = liber.fragPool(name);
    Frag*           frag = new Frag(pool, is);

    position(frag);
    frags[frag->index] = frag;
  }

  lastStructIndex = is->readInt();
  lastObjectIndex = is->readInt();
  lastFragIndex   = is->readInt();

  is->readBitset(pendingStructs[freeing]);
  is->readBitset(pendingStructs[waiting]);
  is->readBitset(pendingObjects[freeing]);
  is->readBitset(pendingObjects[waiting]);
  is->readBitset(pendingFrags[freeing]);
  is->readBitset(pendingFrags[waiting]);
}

void Orbis::read(const Json& json)
{
  caelum.read(json["caelum"]);
  terra.read(json["terra"]);

  for (const Json& strJson : json["structs"].arrayCRange()) {
    String name    = strJson["bsp"].get("");
    const BSP* bsp = liber.bsp(name);

    int index = allocStrIndex();
    if (index != -1) {
      Struct* str = new Struct(bsp, index, strJson);
      position(str);
      structs[index] = str;
    }
  }

  for (const Json& objJson : json["objects"].arrayCRange()) {
    String             name  = objJson["class"].get("");
    const ObjectClass* clazz = liber.objClass(name);

    int index = allocObjIndex();
    if (index != -1) {
      Object*  obj = clazz->create(index, objJson);
      Dynamic* dyn = static_cast<Dynamic*>(obj);

      if (obj->flags & Object::LUA_BIT) {
        luaMatrix.registerObject(obj->index);
      }

      if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent == -1) {
        position(obj);
      }
      objects[obj->index] = obj;

      for (const Json& itemJson : objJson["items"].arrayCRange()) {
        String              itemName  = itemJson["class"].get("");
        const DynamicClass* itemClazz = static_cast<const DynamicClass*>(liber.objClass(itemName));

        if (!(itemClazz->flags & Object::ITEM_BIT)) {
          OZ_ERROR("Inventory object '%s' is not an item", itemClazz->name.c());
        }
        if (obj->items.size() >= obj->clazz->nItems) {
          OZ_ERROR("Too many inventory items for '%s'", itemClazz->name.c());
        }

        int itemIndex = allocObjIndex();
        if (itemIndex != -1) {
          Dynamic* item = static_cast<Dynamic*>(itemClazz->create(itemIndex, itemJson));

          item->parent = obj->index;
          obj->items.add(item->index);

          if (item->flags & Object::LUA_BIT) {
            luaMatrix.registerObject(item->index);
          }

          objects[item->index] = item;
        }
      }
    }
  }
}

int Orbis::readObject(const Json& json)
{
  int index = allocObjIndex();
  if (index == -1) {
    return index;
  }

  String             name  = json["class"].get("");
  const ObjectClass* clazz = liber.objClass(name);
  Object*            obj   = clazz->create(index, json);
  Dynamic*           dyn   = static_cast<Dynamic*>(obj);

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.registerObject(obj->index);
  }

  if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent == -1) {
    position(obj);
  }
  objects[obj->index] = obj;

  return index;
}

void Orbis::write(Stream* os) const
{
  luaMatrix.write(os);

  caelum.write(os);
  terra.write(os);

  int nStructs = Struct::pool.size();
  int nObjects = Object::pool.size() + Dynamic::pool.size() + Weapon::pool.size() +
                 Bot::pool.size() + Vehicle::pool.size();
  int nFrags   = Frag::mpool.size();

  os->writeInt(nStructs);
  os->writeInt(nObjects);
  os->writeInt(nFrags);

  for (const Struct* str : structs) {
    if (str != nullptr) {
      os->writeString(str->bsp->name);
      str->write(os);
    }
  }
  for (const Object* obj : objects) {
    if (obj != nullptr) {
      os->writeString(obj->clazz->name);
      obj->write(os);
    }
  }
  for (const Frag* frag : frags) {
    if (frag != nullptr) {
      os->writeString(frag->pool->name);
      frag->write(os);
    }
  }

  os->writeInt(lastStructIndex);
  os->writeInt(lastObjectIndex);
  os->writeInt(lastFragIndex);

  os->writeBitset(pendingStructs[freeing]);
  os->writeBitset(pendingStructs[waiting]);
  os->writeBitset(pendingObjects[freeing]);
  os->writeBitset(pendingObjects[waiting]);
  os->writeBitset(pendingFrags[freeing]);
  os->writeBitset(pendingFrags[waiting]);
}

Json Orbis::write() const
{
  Json json(Json::OBJECT);

  json.add("caelum", caelum.write());
  json.add("terra", terra.write());

  Set<int> boundObjects;

  Json structsJson = Json::ARRAY;
  Json objectsJson = Json::ARRAY;

  for (const Struct* str : structs) {
    if (str != nullptr) {
      structsJson.add(str->write());

      for (int j : str->boundObjects) {
        if (objects[j] != nullptr) {
          boundObjects.add(j);
        }
      }
    }
  }

  for (const Object* obj : objects) {
    if (obj != nullptr && obj->cell != nullptr && !boundObjects.contains(obj->index)) {
      objectsJson.add(obj->write());
    }
  }

  json.add("structs", static_cast<Json&&>(structsJson));
  json.add("objects", static_cast<Json&&>(objectsJson));

  return json;
}

void Orbis::load()
{}

void Orbis::unload()
{
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (objects[i] != nullptr && (objects[i]->flags & Object::LUA_BIT)) {
      luaMatrix.unregisterObject(i);
    }
  }

  for (auto& column : cells) {
    for (auto& cell : column) {
      cell.structs.clear();
      cell.objects.clear();
      cell.frags.clear();
    }
  }

  Arrays::free(frags, MAX_FRAGS);
  Arrays::free(objects, MAX_OBJECTS);
  Arrays::free(structs, MAX_STRUCTS);

  terra.reset();
  caelum.reset();

  Frag::mpool.free();

  Object::Event::pool.free();
  Object::pool.free();
  Dynamic::pool.free();
  Weapon::pool.free();
  Bot::pool.free();
  Vehicle::pool.free();

  OZ_ASSERT(Struct::overlappingObjs.isEmpty());

  Struct::overlappingObjs.trim();
  Struct::pool.free();

  lastStructIndex = -1;
  lastObjectIndex = -1;
  lastFragIndex   = -1;

  pendingStructs[0].clear();
  pendingStructs[1].clear();
  pendingObjects[0].clear();
  pendingObjects[1].clear();
  pendingFrags[0].clear();
  pendingFrags[1].clear();
}

void Orbis::init()
{
  Log::print("Initialising Orbis ...");

  mins = Point(-Orbis::DIM, -Orbis::DIM, -Orbis::DIM);
  maxs = Point(+Orbis::DIM, +Orbis::DIM, +Orbis::DIM);

  caelum.reset();
  terra.init();
  terra.reset();

  Log::printEnd(" OK");
}

void Orbis::destroy()
{
  Log::println("Destroying Orbis ... OK");
}

Orbis orbis;

}
