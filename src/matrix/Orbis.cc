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
 * @file matrix/Orbis.cc
 */

#include <matrix/Orbis.hh>

#include <matrix/Liber.hh>
#include <matrix/LuaMatrix.hh>
#include <matrix/Vehicle.hh>

namespace oz
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

static int freeing = 0;
static int waiting = 1;

static int lastStructIndex = -1;
static int lastObjectIndex = -1;
static int lastFragIndex   = -1;

static SBitset<Orbis::MAX_STRUCTS> pendingStructs[2];
static SBitset<Orbis::MAX_OBJECTS> pendingObjects[2];
static SBitset<Orbis::MAX_FRAGS>   pendingFrags[2];

int Orbis::allocStrIndex() const
{
  int index = lastStructIndex + 1;

  while (structs[1 + index] != nullptr || pendingStructs[0].get(index) ||
         pendingStructs[1].get(index))
  {
    if (index == lastStructIndex) {
      // We have wrapped around => no slots available.
      soft_assert(false);
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

  while (objects[1 + index] != nullptr || pendingObjects[0].get(index) ||
         pendingObjects[1].get(index))
  {
    if (index == lastObjectIndex) {
      // We have wrapped around => no slots available.
      soft_assert(false);
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

  while (frags[1 + index] != nullptr || pendingFrags[0].get(index) ||
         pendingFrags[1].get(index))
  {
    if (index == lastFragIndex) {
      // We have wrapped around => no slots available.
      soft_assert(false);
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
      if (cells[x][y].structs.length() == cells[x][y].structs.capacity()) {
        soft_assert(false);
        return false;
      }
    }
  }

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      hard_assert(!cells[x][y].structs.contains(short(str->index)));

      cells[x][y].structs.add(short(str->index));
    }
  }

  return true;
}

void Orbis::unposition(Struct* str)
{
  Span span = getInters(*str, EPSILON);

  for (int x = span.minX; x <= span.maxX; ++x) {
    for (int y = span.minY; y <= span.maxY; ++y) {
      hard_assert(cells[x][y].structs.contains(short(str->index)));

      cells[x][y].structs.excludeUnordered(short(str->index));
    }
  }
}

void Orbis::position(Object* obj)
{
  hard_assert(obj->cell == nullptr);

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
  hard_assert(obj->cell != nullptr);

  Cell* cell = obj->cell;

  obj->cell = nullptr;

  if (obj->next[0] != nullptr) {
    obj->next[0]->prev[0] = obj->prev[0];
  }

  cell->objects.erase(obj, obj->prev[0]);
}

void Orbis::position(Frag* frag)
{
  hard_assert(frag->cell == nullptr);

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
  hard_assert(frag->cell != nullptr);

  Cell* cell = frag->cell;

  frag->cell = nullptr;

  if (frag->next[0] != nullptr) {
    frag->next[0]->prev[0] = frag->prev[0];
  }

  cell->frags.erase(frag, frag->prev[0]);
}

Struct* Orbis::add(const BSP* bsp, const Point& p, Heading heading)
{
  int index = allocStrIndex();
  if (index < 0) {
    return nullptr;
  }

  Struct* str = new Struct(bsp, index, p, heading);
  structs[1 + index] = str;

  return str;
}

Object* Orbis::add(const ObjectClass* clazz, const Point& p, Heading heading)
{
  int index = allocObjIndex();
  if (index < 0) {
    return nullptr;
  }

  Object* obj = clazz->create(index, p, heading);
  objects[1 + index] = obj;

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.registerObject(index);
  }

  return obj;
}

Frag* Orbis::add(const FragPool* pool, const Point& p, const Vec3& velocity)
{
  int index = allocFragIndex();
  if (index < 0) {
    return nullptr;
  }

  Frag* frag = new Frag(pool, index, p, velocity);
  frags[1 + index] = frag;

  return frag;
}

void Orbis::remove(Struct* str)
{
  hard_assert(str->index >= 0);

  pendingStructs[freeing].set(str->index);
  structs[1 + str->index] = nullptr;
  delete str;
}

void Orbis::remove(Object* obj)
{
  hard_assert(obj->index >= 0 && obj->cell == nullptr);

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.unregisterObject(obj->index);
  }

  pendingObjects[freeing].set(obj->index);
  objects[1 + obj->index] = nullptr;
  delete obj;
}

void Orbis::remove(Frag* frag)
{
  hard_assert(frag->index >= 0 && frag->cell == nullptr);

  pendingFrags[freeing].set(frag->index);
  frags[1 + frag->index] = nullptr;
  delete frag;
}

void Orbis::reposition(Object* obj)
{
  hard_assert(obj->cell != nullptr);

  Cell* oldCell = obj->cell;
  Cell* newCell = getCell(obj->p);

  if (newCell != oldCell) {
    if (obj->next[0] != nullptr) {
      obj->next[0]->prev[0] = obj->prev[0];
    }

    oldCell->objects.erase(obj, obj->prev[0]);

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
  hard_assert(frag->cell != nullptr);

  Cell* oldCell = frag->cell;
  Cell* newCell = getCell(frag->p);

  if (newCell != oldCell) {
    if (frag->next[0] != nullptr) {
      frag->next[0]->prev[0] = frag->prev[0];
    }

    oldCell->frags.erase(frag, frag->prev[0]);

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
  pendingStructs[waiting].clearAll();
  pendingObjects[waiting].clearAll();
  pendingFrags[waiting].clearAll();

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
    structs[1 + str->index] = str;
  }

  for (int i = 0; i < nObjects; ++i) {
    const char*        name  = is->readString();
    const ObjectClass* clazz = liber.objClass(name);
    Object*            obj   = clazz->create(is);
    const Dynamic*     dyn   = static_cast<const Dynamic*>(obj);

    // No need to register objects since Lua state is being deserialised.

    if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent < 0) {
      position(obj);
    }
    objects[1 + obj->index] = obj;
  }

  for (int i = 0; i < nFrags; ++i) {
    const char*     name = is->readString();
    const FragPool* pool = liber.fragPool(name);
    Frag*           frag = new Frag(pool, is);

    position(frag);
    frags[1 + frag->index] = frag;
  }

  lastStructIndex = is->readInt();
  lastObjectIndex = is->readInt();
  lastFragIndex   = is->readInt();

  is->readBitset(pendingStructs[freeing], pendingStructs[freeing].length());
  is->readBitset(pendingStructs[waiting], pendingStructs[waiting].length());
  is->readBitset(pendingObjects[freeing], pendingObjects[freeing].length());
  is->readBitset(pendingObjects[waiting], pendingObjects[waiting].length());
  is->readBitset(pendingFrags[freeing], pendingFrags[freeing].length());
  is->readBitset(pendingFrags[waiting], pendingFrags[waiting].length());
}

void Orbis::read(const Json& json)
{
  caelum.read(json["caelum"]);
  terra.read(json["terra"]);

  for (const Json& strJson : json["structs"].arrayCIter()) {
    String name    = strJson["bsp"].get("");
    const BSP* bsp = liber.bsp(name);

    int index = allocStrIndex();
    if (index >= 0) {
      Struct* str = new Struct(bsp, index, strJson);
      position(str);
      structs[1 + index] = str;
    }
  }

  for (const Json& objJson : json["objects"].arrayCIter()) {
    String             name  = objJson["class"].get("");
    const ObjectClass* clazz = liber.objClass(name);

    int index = allocObjIndex();
    if (index >= 0) {
      Object*  obj = clazz->create(index, objJson);
      Dynamic* dyn = static_cast<Dynamic*>(obj);

      if (obj->flags & Object::LUA_BIT) {
        luaMatrix.registerObject(obj->index);
      }

      if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent < 0) {
        position(obj);
      }
      objects[1 + obj->index] = obj;

      for (const Json& itemJson : objJson["items"].arrayCIter()) {
        String              itemName  = itemJson["class"].get("");
        const DynamicClass* itemClazz = static_cast<const DynamicClass*>(liber.objClass(itemName));

        if (!(itemClazz->flags & Object::ITEM_BIT)) {
          OZ_ERROR("Inventory object '%s' is not an item", itemClazz->name.c());
        }
        if (obj->items.length() >= obj->clazz->nItems) {
          OZ_ERROR("Too many inventory items for '%s'", itemClazz->name.c());
        }

        int itemIndex = allocObjIndex();
        if (itemIndex >= 0) {
          Dynamic* item = static_cast<Dynamic*>(itemClazz->create(itemIndex, itemJson));

          item->parent = obj->index;
          obj->items.add(item->index);

          if (item->flags & Object::LUA_BIT) {
            luaMatrix.registerObject(item->index);
          }

          objects[1 + item->index] = item;
        }
      }
    }
  }
}

int Orbis::readObject(const Json& json)
{
  int index = allocObjIndex();
  if (index < 0) {
    return index;
  }

  String             name  = json["class"].get("");
  const ObjectClass* clazz = liber.objClass(name);
  Object*            obj   = clazz->create(index, json);
  Dynamic*           dyn   = static_cast<Dynamic*>(obj);

  if (obj->flags & Object::LUA_BIT) {
    luaMatrix.registerObject(obj->index);
  }

  if (!(obj->flags & Object::DYNAMIC_BIT) || dyn->parent < 0) {
    position(obj);
  }
  objects[1 + obj->index] = obj;

  return index;
}

void Orbis::write(Stream* os) const
{
  luaMatrix.write(os);

  caelum.write(os);
  terra.write(os);

  int nStructs = Struct::pool.length();
  int nObjects = Object::pool.length() + Dynamic::pool.length() + Weapon::pool.length() +
                 Bot::pool.length() + Vehicle::pool.length();
  int nFrags   = Frag::mpool.length();

  os->writeInt(nStructs);
  os->writeInt(nObjects);
  os->writeInt(nFrags);

  for (int i = 0; i < MAX_STRUCTS; ++i) {
    Struct* str = structs[1 + i];

    if (str != nullptr) {
      os->writeString(str->bsp->name);
      str->write(os);
    }
  }
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    Object* obj = objects[1 + i];

    if (obj != nullptr) {
      os->writeString(obj->clazz->name);
      obj->write(os);
    }
  }
  for (int i = 0; i < MAX_FRAGS; ++i) {
    Frag* frag = frags[1 + i];

    if (frag != nullptr) {
      os->writeString(frag->pool->name);
      frag->write(os);
    }
  }

  os->writeInt(lastStructIndex);
  os->writeInt(lastObjectIndex);
  os->writeInt(lastFragIndex);

  os->writeBitset(pendingStructs[freeing], pendingStructs[freeing].length());
  os->writeBitset(pendingStructs[waiting], pendingStructs[waiting].length());
  os->writeBitset(pendingObjects[freeing], pendingObjects[freeing].length());
  os->writeBitset(pendingObjects[waiting], pendingObjects[waiting].length());
  os->writeBitset(pendingFrags[freeing], pendingFrags[freeing].length());
  os->writeBitset(pendingFrags[waiting], pendingFrags[waiting].length());
}

Json Orbis::write() const
{
  Json json(Json::OBJECT);

  json.add("caelum", caelum.write());
  json.add("terra", terra.write());

  Set<int> boundObjects;

  Json structsJson = Json::ARRAY;
  Json objectsJson = Json::ARRAY;

  for (int i = 0; i < MAX_STRUCTS; ++i) {
    const Struct* str = structs[1 + i];

    if (str != nullptr) {
      structsJson.add(str->write());

      for (int j : str->boundObjects) {
        if (objects[1 + j] != nullptr) {
          boundObjects.add(j);
        }
      }
    }
  }

  for (int i = 0; i < MAX_OBJECTS; ++i) {
    const Object* obj = objects[1 + i];

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
    if (objects[1 + i] != nullptr && (objects[1 + i]->flags & Object::LUA_BIT)) {
      luaMatrix.unregisterObject(i);
    }
  }

  for (int i = 0; i < Orbis::CELLS; ++i) {
    for (int j = 0; j < Orbis::CELLS; ++j) {
      cells[i][j].structs.clear();
      cells[i][j].objects.clear();
      cells[i][j].frags.clear();
    }
  }

  hard_assert(structs[0] == nullptr && objects[0] == nullptr && frags[0] == nullptr);

  Arrays::free(&frags[1], MAX_FRAGS);
  Arrays::fill(&frags[1], MAX_FRAGS, nullptr);
  Arrays::free(&objects[1], MAX_OBJECTS);
  Arrays::fill(&objects[1], MAX_OBJECTS, nullptr);
  Arrays::free(&structs[1], MAX_STRUCTS);
  Arrays::fill(&structs[1], MAX_STRUCTS, nullptr);

  terra.reset();
  caelum.reset();

  Frag::mpool.free();

  Object::Event::pool.free();
  Object::pool.free();
  Dynamic::pool.free();
  Weapon::pool.free();
  Bot::pool.free();
  Vehicle::pool.free();

  hard_assert(Struct::overlappingObjs.isEmpty());

  Struct::overlappingObjs.trim();
  Struct::pool.free();

  lastStructIndex = -1;
  lastObjectIndex = -1;
  lastFragIndex   = -1;

  pendingStructs[0].clearAll();
  pendingStructs[1].clearAll();
  pendingObjects[0].clearAll();
  pendingObjects[1].clearAll();
  pendingFrags[0].clearAll();
  pendingFrags[1].clearAll();
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
