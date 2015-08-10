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
 * @file matrix/Struct.cc
 */

#include <matrix/Struct.hh>

#include <matrix/Physics.hh>
#include <matrix/Synapse.hh>
#include <matrix/Bot.hh>

namespace oz
{

static const Mat4 ROTATIONS[] =
{
  Mat4( 1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f),
  Mat4( 0.0f,  1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f),
  Mat4(-1.0f,  0.0f,  0.0f,  0.0f,
        0.0f, -1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f),
  Mat4( 0.0f, -1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f),
  Mat4( 1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f),
};

static const Entity::State OPPOSITE_STATES[] = { Entity::CLOSING, Entity::OPENING };
static const Entity::State END_STATES[]      = { Entity::OPEN, Entity::CLOSED };

const Vec3  Struct::DESTRUCT_FRAG_VELOCITY   = Vec3(0.0f, 0.0f, 2.0f);
const float Struct::DEMOLISH_SPEED           = 8.0f;

const Entity::Handler Entity::HANDLERS[] = {
  &Entity::staticHandler,
  &Entity::moverHandler,
  &Entity::doorHandler,
  &Entity::portalHandler
};

List<Object*> Struct::overlappingObjs;
Pool<Struct>  Struct::pool;

bool Entity::trigger()
{
  if (clazz->target < 0 || key < 0) {
    return false;
  }

  if (clazz->type == EntityClass::STATIC) {
    OZ_ASSERT(time == 0.0f);

    state = OPENING;
  }

  int strIndex = clazz->target >> Struct::MAX_ENT_SHIFT;
  int entIndex = clazz->target & (Struct::MAX_ENTITIES - 1);

  Struct* targetStr = orbis.str(strIndex);
  if (targetStr == nullptr) {
    return false;
  }

  Entity& target = targetStr->entities[entIndex];

  if (target.state == OPEN || target.state == OPENING) {
    target.state = CLOSING;
    target.time  = 0.0f;
  }
  else {
    target.state = OPENING;
    target.time  = 0.0f;
  }

  return true;
}

bool Entity::lock(Bot* user)
{
  if (key == 0) {
    return true;
  }

  if (user->clazz->key == key || user->clazz->key == ~key) {
    key = ~key;
    return true;
  }

  for (int i : user->items) {
    Object* obj = orbis.obj(i);

    if (obj->clazz->key == key || obj->clazz->key == ~key) {
      key = ~key;
      return true;
    }
  }
  return false;
}

void Entity::staticHandler()
{
  state = CLOSED;
}

void Entity::moverHandler()
{
  time += Timer::TICK_TIME;

  float timeout[] { clazz->openTimeout, clazz->closeTimeout };

  switch (state) {
    case CLOSED:
    case OPEN: {
      bool isClosed = state == CLOSED;

      if (timeout[isClosed] != 0.0f && time > timeout[isClosed]) {
        if (time > clazz->openTimeout) {
          state = OPPOSITE_STATES[isClosed];
          time  = 0.0f;
        }
      }
      break;
    }
    case OPENING:
    case CLOSING: {
      Vec3 move      = destination - offset;
      bool finishing = move.sqN() <= clazz->moveStep;

      physics.updateEnt(this, move);

      if (finishing && collider.hit.ratio == 1.0f) {
        bool isClosing = state == CLOSING;

        state    = END_STATES[isClosing];
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      break;
    }
  }
}

void Entity::doorHandler()
{
  time += Timer::TICK_TIME;

  switch (state) {
    case CLOSED: {
      if (clazz->flags & EntityClass::AUTO_OPEN) {
        bool checkTick = (timer.ticks + uint(str->index * 1025)) % (Timer::TICKS_PER_SEC / 6) == 0;

        if (checkTick && collider.overlaps(this, clazz->margin)) {
          state = OPENING;
          time  = 0.0f;
        }
      }
      break;
    }
    case OPENING: {
      Vec3 origOffset = offset;

      moveDist += clazz->moveStep;

      if (moveDist >= clazz->moveLength) {
        state    = OPEN;
        time     = 0.0f;
        moveDist = clazz->moveLength;
        offset   = moveDist * clazz->moveDir;
        velocity = Vec3::ZERO;
      }
      else {
        offset   = moveDist * clazz->moveDir;
        velocity = (offset - origOffset) / Timer::TICK_TIME;
      }
      break;
    }
    case OPEN: {
      if (clazz->closeTimeout != 0.0f && time > clazz->closeTimeout) {
        Vec3 origOffset = offset;

        time   = 0.0f;
        offset = Vec3::ZERO;

        if (!collider.overlaps(this, clazz->margin)) {
          state = CLOSING;
        }

        offset = origOffset;
      }
      break;
    }
    case CLOSING: {
      float origMove   = moveDist;
      Vec3  origOffset = offset;

      moveDist -= clazz->moveStep;
      offset    = Vec3::ZERO;

      if (collider.overlaps(this, clazz->margin)) {
        state    = OPENING;
        time     = 0.0f;
        moveDist = origMove;
        offset   = origOffset;
        velocity = Vec3::ZERO;
      }
      else if (moveDist <= 0.0f) {
        state    = CLOSED;
        time     = 0.0f;
        moveDist = 0.0f;
        velocity = Vec3::ZERO;
      }
      else {
        offset   = moveDist * clazz->moveDir;
        velocity = (offset - origOffset) / Timer::TICK_TIME;
      }
      break;
    }
  }
}

void Entity::portalHandler()
{
  state = CLOSED;
}

void Struct::onDemolish()
{
  collider.mask = ~0;
  collider.getOverlaps(toAABB(), nullptr, &overlappingObjs, 2.0f * EPSILON);
  collider.mask = Object::SOLID_BIT;

  for (int i = 0; i < overlappingObjs.length(); ++i) {
    Dynamic* dyn = static_cast<Dynamic*>(overlappingObjs[i]);

    if ((dyn->flags & Object::SOLID_BIT) &&
        collider.overlaps(AABB(*dyn, -2.0f * EPSILON), dyn))
    {
      dyn->destroy();
    }
    else if (dyn->flags & Object::DYNAMIC_BIT) {
      dyn->flags &= ~Object::DISABLED_BIT;
      dyn->flags |= Object::ENABLE_BIT;
    }
  }

  overlappingObjs.clear();

  float deltaHeight = DEMOLISH_SPEED * Timer::TICK_TIME;
  demolishing += deltaHeight / (maxs.z - mins.z);
  p.z -= deltaHeight;

  transf.w = Vec4(p);

  invTransf = ROTATIONS[4 - heading];
  invTransf.translate(Point::ORIGIN - p);

  Bounds bb = toAbsoluteCS(*bsp);
  mins = bb.mins;
  maxs = bb.maxs;
}

void Struct::onUpdate()
{
  for (int i = 0; i < boundObjects.length();) {
    if (orbis.obj(boundObjects[i]) == nullptr) {
      boundObjects.eraseUnordered(i);
    }
    else {
      ++i;
    }
  }

  if (life == 0.0f) {
    onDemolish();
  }
  else {
    for (int i = 0; i < entities.length(); ++i) {
      Entity& entity = entities[i];

      (entity.*Entity::HANDLERS[entity.clazz->type])();
    }
  }
}

Bounds Struct::toStructCS(const Bounds& bb) const
{
  switch (heading) {
    case NORTH: {
      return Bounds(Point(+bb.mins.x - p.x, +bb.mins.y - p.y, +bb.mins.z - p.z),
                    Point(+bb.maxs.x - p.x, +bb.maxs.y - p.y, +bb.maxs.z - p.z));
    }
    case WEST: {
      return Bounds(Point(+bb.mins.y - p.y, -bb.maxs.x + p.x, +bb.mins.z - p.z),
                    Point(+bb.maxs.y - p.y, -bb.mins.x + p.x, +bb.maxs.z - p.z));
    }
    case SOUTH: {
      return Bounds(Point(-bb.maxs.x + p.x, -bb.maxs.y + p.y, +bb.mins.z - p.z),
                    Point(-bb.mins.x + p.x, -bb.mins.y + p.y, +bb.maxs.z - p.z));
    }
    case EAST: {
      return Bounds(Point(-bb.maxs.y + p.y, +bb.mins.x - p.x, +bb.mins.z - p.z),
                    Point(-bb.mins.y + p.y, +bb.maxs.x - p.x, +bb.maxs.z - p.z));
    }
  }
}

Bounds Struct::toAbsoluteCS(const Bounds& bb) const
{
  switch (heading) {
    case NORTH: {
      return Bounds(p + Vec3(+bb.mins.x, +bb.mins.y, +bb.mins.z),
                    p + Vec3(+bb.maxs.x, +bb.maxs.y, +bb.maxs.z));
    }
    case WEST: {
      return Bounds(p + Vec3(-bb.maxs.y, +bb.mins.x, +bb.mins.z),
                    p + Vec3(-bb.mins.y, +bb.maxs.x, +bb.maxs.z));
    }
    case SOUTH: {
      return Bounds(p + Vec3(-bb.maxs.x, -bb.maxs.y, +bb.mins.z),
                    p + Vec3(-bb.mins.x, -bb.mins.y, +bb.maxs.z));
    }
    case EAST: {
      return Bounds(p + Vec3(+bb.mins.y, -bb.maxs.x, +bb.mins.z),
                    p + Vec3(+bb.maxs.y, -bb.mins.x, +bb.maxs.z));
    }
  }
}

void Struct::destroy()
{
  for (int i = 0; i < boundObjects.length(); ++i) {
    Object* obj = orbis.obj(boundObjects[i]);

    if (obj != nullptr) {
      obj->destroy();
    }
  }

  onDemolish();

  if (bsp->fragPool != nullptr) {
    synapse.gen(bsp->fragPool,
                bsp->nFrags,
                Bounds(Point(mins.x, mins.y, 0.5f * (mins.z + maxs.z)), maxs),
                DESTRUCT_FRAG_VELOCITY);
  }
}

Struct::Struct(const BSP* bsp_, int index_, const Point& p_, Heading heading_)
{
  bsp         = bsp_;

  p           = p_;
  heading     = heading_;

  index       = index_;

  life        = bsp->life;
  resistance  = bsp->resistance;
  demolishing = 0.0f;

  transf      = Mat4::translation(p - Point::ORIGIN) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat4::translation(Point::ORIGIN - p);

  OZ_ASSERT(transf.det() != 0.0f);

  Bounds bb   = toAbsoluteCS(*bsp);
  mins        = bb.mins;
  maxs        = bb.maxs;

  if (bsp->nEntities != 0) {
    entities.resize(bsp->nEntities, true);

    for (int i = 0; i < entities.length(); ++i) {
      Entity& entity = entities[i];

      entity.clazz    = &bsp->entities[i];
      entity.str      = this;
      entity.key      = bsp->entities[i].key;
      entity.state    = Entity::CLOSED;
      entity.moveDist = 0.0f;
      entity.time     = 0.0f;
      entity.offset   = Vec3::ZERO;
      entity.velocity = Vec3::ZERO;
    }
  }

  if (bsp->nBoundObjects != 0) {
    boundObjects.reserve(bsp->nBoundObjects, true);
  }
}

Struct::Struct(const BSP* bsp_, int index_, const Json& json)
{
  bsp         = bsp_;

  p           = json["p"].get(Point::ORIGIN);
  heading     = Heading(json["heading"].get(Heading::NORTH));

  index       = index_;

  life        = json["life"].get(0.0f);
  resistance  = bsp->resistance;
  demolishing = json["demolishing"].get(0.0f);

  transf      = Mat4::translation(p - Point::ORIGIN) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat4::translation(Point::ORIGIN - p);

  Bounds bb   = toAbsoluteCS(*bsp);
  mins        = bb.mins;
  maxs        = bb.maxs;

  if (index < 0) {
    OZ_ERROR("Invalid struct index");
  }

  if (bsp->nEntities != 0) {
    entities.resize(bsp->nEntities, true);

    const Json& entitiesJson = json["entities"];

    for (int i = 0; i < entities.length(); ++i) {
      const Json& entityJson = entitiesJson[i];
      Entity&     entity     = entities[i];

      entity.clazz    = &bsp->entities[i];
      entity.str      = this;
      entity.key      = entityJson["key"].get(0);
      entity.state    = Entity::State(entityJson["state"].get(Entity::CLOSED));
      entity.moveDist = entityJson["move"].get(0.0f);
      entity.time     = entityJson["time"].get(0.0f);
      entity.offset   = entityJson["offset"].get(Vec3::ZERO);
      entity.velocity = entityJson["velocity"].get(Vec3::ZERO);
    }
  }

  if (bsp->nBoundObjects != 0) {
    boundObjects.reserve(bsp->nBoundObjects, true);
  }
}

Struct::Struct(const BSP* bsp_, Stream* is)
{
  bsp         = bsp_;

  p           = is->read<Point>();
  heading     = Heading(is->readInt());

  index       = is->readInt();

  life        = is->readFloat();
  resistance  = bsp->resistance;
  demolishing = is->readFloat();

  transf      = Mat4::translation(p - Point::ORIGIN) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat4::translation(Point::ORIGIN - p);

  Bounds bb   = toAbsoluteCS(*bsp);
  mins        = bb.mins;
  maxs        = bb.maxs;

  if (bsp->nEntities != 0) {
    entities.resize(bsp->nEntities, true);

    for (int i = 0; i < entities.length(); ++i) {
      Entity& entity = entities[i];

      entity.clazz    = &bsp->entities[i];
      entity.str      = this;
      entity.key      = is->readInt();
      entity.state    = Entity::State(is->readInt());
      entity.moveDist = is->readFloat();
      entity.time     = is->readFloat();
      entity.offset   = is->read<Vec3>();
      entity.velocity = is->read<Vec3>();
    }
  }

  int nBoundObjects = is->readInt();
  OZ_ASSERT(nBoundObjects <= bsp->nBoundObjects);

  if (bsp->nBoundObjects != 0) {
    boundObjects.reserve(bsp->nBoundObjects, true);

    for (int i = 0; i < nBoundObjects; ++i) {
      boundObjects.add(is->readInt());
    }
  }
}

Json Struct::write() const
{
  Json json(Json::OBJECT);

  json.add("bsp", bsp->name);

  json.add("p", p);
  json.add("heading", heading);

  json.add("life", life);
  json.add("demolishing", demolishing);

  Json& entitiesJson = json.add("entities", Json::ARRAY);

  for (int i = 0; i < entities.length(); ++i) {
    Json& entityJson = entitiesJson.add(Json::OBJECT);

    entityJson.add("key", entities[i].key);
    entityJson.add("state", entities[i].state);
    entityJson.add("move", entities[i].moveDist);
    entityJson.add("time", entities[i].time);
    entityJson.add("offset", entities[i].offset);
    entityJson.add("velocity", entities[i].velocity);
  }

  Json& boundObjectsJson = json.add("boundObjects", Json::ARRAY);

  for (int i : boundObjects) {
    const Object* obj = orbis.obj(i);

    if (obj != nullptr) {
      boundObjectsJson.add(obj->write());
    }
  }

  return json;
}

void Struct::write(Stream* os) const
{
  os->write<Point>(p);
  os->writeInt(heading);

  os->writeInt(index);

  os->writeFloat(life);
  os->writeFloat(demolishing);

  for (int i = 0; i < entities.length(); ++i) {
    os->writeInt(entities[i].key);
    os->writeInt(entities[i].state);
    os->writeFloat(entities[i].moveDist);
    os->writeFloat(entities[i].time);
    os->write<Vec3>(entities[i].offset);
    os->write<Vec3>(entities[i].velocity);
  }

  os->writeInt(boundObjects.length());
  for (int i = 0; i < boundObjects.length(); ++i) {
    os->writeInt(boundObjects[i]);
  }
}

}
