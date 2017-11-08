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

#include <matrix/Physics.hh>

namespace oz
{

const float Physics::FLOOR_NORMAL_Z          =  0.60f;
const float Physics::MOVE_BOUNCE             =  EPSILON;
const float Physics::ENTITY_BOND_G_RATIO     =  0.10f;
const float Physics::SIDE_PUSH_RATIO         =  0.40f;

const float Physics::HIT_MAX_MASS            =  100.0f;
const float Physics::HIT_THRESHOLD           = -3.0f;
const float Physics::HIT_INTENSITY_COEF      =  0.02f;
const float Physics::HIT_ENERGY_COEF         =  0.01f;
const float Physics::SPLASH_THRESHOLD        = -2.0f;
const float Physics::SPLASH_INTENSITY_COEF   =  0.02f;

const float Physics::SLIDE_DAMAGE_THRESHOLD  =  50.0f;
const float Physics::SLIDE_DAMAGE_COEF       = -2.5f;

const float Physics::STICK_VELOCITY          =  0.03f;
const float Physics::SLICK_STICK_VELOCITY    =  0.003f;
const float Physics::FLOATING_STICK_VELOCITY =  0.0005f;
const float Physics::WATER_FRICTION          =  0.10f;
const float Physics::LADDER_FRICTION         =  0.15f;
const float Physics::FLOOR_FRICTION_COEF     =  0.30f;
const float Physics::SLICK_FRICTION_COEF     =  0.03f;

const float Physics::LAVA_LIFT               =  1.2f;
const float Physics::LAVA_DAMAGE_ABSOLUTE    =  175.0f;
const float Physics::LAVA_DAMAGE_RATIO       =  0.25f;
const int   Physics::LAVA_DAMAGE_INTERVAL    =  Timer::TICKS_PER_SEC / 2;

const float Physics::FRAG_HIT_VELOCITY2      =  100.0f;
const float Physics::FRAG_DESTROY_VELOCITY2  =  300.0f;
const float Physics::FRAG_DAMAGE_COEF        =  0.05f;
const float Physics::FRAG_FIXED_DAMAGE       =  0.75f;

//***********************************
//*    OBJECT COLLISION HANDLING    *
//***********************************

bool Physics::handleObjFriction()
{
  float systemMom = gravity * Timer::TICK_TIME;

  if (dyn->flags & Object::IN_LIQUID_BIT) {
    float lift = dyn->flags & Object::IN_LAVA_BIT ? LAVA_LIFT : dyn->lift;
    float frictionFactor = 0.5f * dyn->depth / dyn->dim.z;

    dyn->momentum *= 1.0f - frictionFactor * WATER_FRICTION;
    systemMom -= frictionFactor * lift * gravity * Timer::TICK_TIME;
  }

  if (dyn->flags & Object::ON_LADDER_BIT) {
    float momentum2 = dyn->momentum.sqN();

    if (momentum2 <= STICK_VELOCITY) {
      dyn->momentum = Vec3::ZERO;

      return dyn->flags & Object::ENABLE_BIT;
    }
    else {
      dyn->momentum *= 1.0f - LADDER_FRICTION;
    }
  }
  else {
    bool isLowerStill = true;

    float deltaVelX = dyn->momentum.x;
    float deltaVelY = dyn->momentum.y;

    if (dyn->lower != -1) {
      if (dyn->flags & Object::ON_FLOOR_BIT) {
        const Entity* ent = orbis.ent(dyn->lower);

        if (ent->velocity != Vec3::ZERO) {
          Vec3 entVelocity = ent->str->toAbsoluteCS(ent->velocity);

          isLowerStill = false;

          deltaVelX -= entVelocity.x;
          deltaVelY -= entVelocity.y;

          // Push a little into entity if e.g. on an elevator going down.
          if (entVelocity.z < 0.0f && !(dyn->flags & Object::IN_LIQUID_BIT)) {
            systemMom += ENTITY_BOND_G_RATIO * gravity;
          }
        }
      }
      else {
        const Dynamic* sDyn = orbis.obj<const Dynamic>(dyn->lower);

        if (sDyn->velocity != Vec3::ZERO) {
          isLowerStill = false;

          deltaVelX -= sDyn->velocity.x;
          deltaVelY -= sDyn->velocity.y;
        }
      }
    }

    // On floor or on a still object.
    if ((dyn->flags & Object::ON_FLOOR_BIT) || dyn->lower != -1) {
      float deltaVel2 = deltaVelX*deltaVelX + deltaVelY*deltaVelY;
      float friction  = FLOOR_FRICTION_COEF;
      float stickVel  = STICK_VELOCITY;

      if (dyn->flags & Object::ON_SLICK_BIT) {
        friction = SLICK_FRICTION_COEF;
        stickVel = SLICK_STICK_VELOCITY;
      }

      dyn->momentum   += (systemMom * dyn->floor.z) * dyn->floor;
      dyn->momentum.x -= deltaVelX * friction;
      dyn->momentum.y -= deltaVelY * friction;
      dyn->momentum.z *= 1.0f - friction;

      // Push into floor just enough that collision occurs continuously each tick.
      dyn->momentum.z -= EPSILON / Timer::TICK_TIME;

      if (deltaVel2 > stickVel) {
        dyn->flags |= Object::FRICTING_BIT;

        if (deltaVel2 > SLIDE_DAMAGE_THRESHOLD) {
          dyn->damage(SLIDE_DAMAGE_COEF * Math::fastSqrt(deltaVel2) * friction * gravity);
        }
      }
      else if (isLowerStill) {
        dyn->momentum.x = 0.0f;
        dyn->momentum.y = 0.0f;

        if (dyn->momentum.z <= 0.0f && !(dyn->flags & Object::ENABLE_BIT)) {
          dyn->momentum.z = 0.0f;

          if (systemMom <= 0.0f) {
            return dyn->flags & Object::ENABLE_BIT;
          }
        }
      }
    }
    // In air or swimming.
    else {
      dyn->momentum.z += systemMom;

      if (abs(systemMom) <= FLOATING_STICK_VELOCITY &&
          dyn->momentum.sqN() <= FLOATING_STICK_VELOCITY)
      {
        dyn->momentum = Vec3::ZERO;

        return dyn->flags & Object::ENABLE_BIT;
      }
    }
  }

  return true;
}

void Physics::handleObjHit()
{
  const Hit& hit = collider.hit;

  if (hit.obj != nullptr && (hit.obj->flags & Object::DYNAMIC_BIT)) {
    Dynamic* sDyn = static_cast<Dynamic*>(hit.obj);

    float massSum     = dyn->mass + sDyn->mass;
    Vec3  momentum    = (dyn->momentum * dyn->mass + sDyn->momentum * sDyn->mass) / massSum;
    float hitMomentum = (dyn->momentum - sDyn->momentum) * hit.normal;
    float hitVelocity = dyn->velocity * hit.normal;

    if (hitMomentum < HIT_THRESHOLD && hitVelocity < HIT_THRESHOLD) {
      float momentum2 = hitMomentum*hitMomentum;
      float energy    = min(dyn->mass, HIT_MAX_MASS) * momentum2;
      float intensity = momentum2 * HIT_INTENSITY_COEF;
      float damage    = energy * HIT_ENERGY_COEF;
      bool  hasLanded = hit.normal.z > 0.0f; // Just test for sign, it can only be +1 or -1.

      dyn->addEvent(Object::EVENT_HIT + hasLanded, intensity);
      dyn->damage(damage);

      sDyn->addEvent(Object::EVENT_HIT, intensity);
      sDyn->damage(damage);
    }

    if (hit.normal.z == 0.0f) {
      float momProj     =       momentum.x * hit.normal.x +       momentum.y * hit.normal.y;
      float dynMomProj  =  dyn->momentum.x * hit.normal.x +  dyn->momentum.y * hit.normal.y;
      float sDynMomProj = sDyn->momentum.x * hit.normal.x + sDyn->momentum.y * hit.normal.y;
      float sDynVelProj = sDyn->velocity.x * hit.normal.x + sDyn->velocity.y * hit.normal.y;
      float directPushX = (momProj - sDynMomProj) * hit.normal.x;
      float directPushY = (momProj - sDynMomProj) * hit.normal.y;

      dyn->momentum.x -= (dynMomProj - sDynVelProj) * hit.normal.x;
      dyn->momentum.y -= (dynMomProj - sDynVelProj) * hit.normal.y;

      sDyn->flags      &= ~Object::DISABLED_BIT;
      sDyn->momentum.x += directPushX;
      sDyn->momentum.y += directPushY;

      if (dyn->flags & Object::BOT_BIT) {
        float pushX = momentum.x - sDyn->momentum.x;
        float pushY = momentum.y - sDyn->momentum.y;

        sDyn->momentum.x += SIDE_PUSH_RATIO * (pushX - directPushX);
        sDyn->momentum.y += SIDE_PUSH_RATIO * (pushY - directPushY);

        // Allow side-pushing downwards in water.
        if ((sDyn->flags & Object::IN_LIQUID_BIT) && momentum.z < 0.0f) {
          sDyn->momentum.z += SIDE_PUSH_RATIO * (momentum.z - sDyn->momentum.z);
        }
      }
    }
    else if (hit.normal.z == -1.0f) {
      dyn->flags      |= Object::BELOW_BIT;
      dyn->momentum.z  = sDyn->velocity.z;

      sDyn->flags     &= ~(Object::DISABLED_BIT | Object::ON_FLOOR_BIT);
      sDyn->lower      = dyn->index;
      sDyn->floor      = Vec3(0.0f, 0.0f, 1.0f);
      sDyn->momentum.z = momentum.z;
    }
    else { // hit.normal.z == 1.0f
      OZ_ASSERT(hit.normal.z == 1.0f);

      dyn->flags     &= ~Object::ON_FLOOR_BIT;
      dyn->lower      = sDyn->index;
      dyn->floor      = Vec3(0.0f, 0.0f, 1.0f);
      dyn->momentum.z = sDyn->velocity.z;

      sDyn->flags    |= Object::BELOW_BIT;

      if (!(sDyn->flags & Object::ON_FLOOR_BIT) && sDyn->lower == -1) {
        sDyn->flags     &= ~Object::DISABLED_BIT;
        sDyn->momentum.z = momentum.z;
      }
    }
  }
  else {
    float hitMomentum = dyn->momentum * hit.normal;
    float hitVelocity = dyn->velocity * hit.normal;

    if (hitMomentum < HIT_THRESHOLD && hitVelocity < HIT_THRESHOLD) {
      float momentum2 = hitMomentum*hitMomentum;
      float energy    = min(dyn->mass, HIT_MAX_MASS) * momentum2;
      float intensity = momentum2 * HIT_INTENSITY_COEF;
      float damage    = energy * HIT_ENERGY_COEF;
      bool  hasLanded = hit.normal.z >= FLOOR_NORMAL_Z;

      dyn->addEvent(Object::EVENT_HIT + hasLanded, intensity);
      dyn->damage(damage);

      if (hit.obj != nullptr) {
        hit.obj->addEvent(Object::EVENT_HIT, intensity);
        hit.obj->damage(damage);
      }
      else if (hit.str != nullptr) {
        hit.str->damage(damage);
      }
    }

    dyn->momentum -= (dyn->momentum * hit.normal) * hit.normal;

    if (hit.normal.z >= FLOOR_NORMAL_Z) {
      dyn->flags |= Object::ON_FLOOR_BIT;
      dyn->flags |= hit.material & Material::SLICK_BIT ? Object::ON_SLICK_BIT : 0;
      dyn->floor  = hit.normal;
      dyn->lower  = hit.entity == nullptr ? -1 : hit.entity->index();
    }
  }
}

Vec3 Physics::handleObjMove()
{
  // It's much more accurate to sum partial moves than calculate it from a position difference;
  // floating and sliding may never come to a halt at large world coordinates in the latter case.
  Vec3 realisedMove = Vec3::ZERO;

  move = dyn->momentum * Timer::TICK_TIME;

  float moveLen = move.fastN();
  if (moveLen == 0.0f) {
    return realisedMove;
  }

  Vec3  originalDir = move / moveLen;
  float leftRatio   = 1.0f;

  int traceSplits = 0;
  do {
    collider.translate(dyn, move);

    Vec3 partialMove = collider.hit.ratio * move;

    realisedMove += partialMove;
    dyn->p       += partialMove;
    leftRatio    -= leftRatio * collider.hit.ratio;

    if (collider.hit.ratio == 1.0f) {
      break;
    }
    // Collision response.
    handleObjHit();

    if (traceSplits >= 3 || dyn->life == 0.0f) {
      break;
    }
    ++traceSplits;

    move *= 1.0f - collider.hit.ratio;
    move -= (move * collider.hit.normal - MOVE_BOUNCE) * collider.hit.normal;

    // In obtuse corners (> 90°) we prevent oscillations by preventing move in the opposite
    // direction form the original one.
    move -= min<float>(move * originalDir + MOVE_BOUNCE, 0.0f) * originalDir;

    // In acute (< 90°) corners we move the object a little out of it to prevent it getting stuck.
    if (traceSplits == 1) {
      lastNormals[0] = collider.hit.normal;
    }
    else {
      float dot = lastNormals[0] * collider.hit.normal;

      if (dot < 0.0f) {
        Vec3  cross    = collider.hit.normal ^ lastNormals[0];
        float crossSqN = cross.sqN();

        if (crossSqN == 0.0f) {
          move = Vec3::ZERO;
        }
        else {
          float length_1 = Math::fastInvSqrt(crossSqN);

          cross *= length_1;
          move   = (move * cross) * cross;
          move  += 3.0f * MOVE_BOUNCE * length_1 * (collider.hit.normal + lastNormals[0]);
        }
      }

      if (traceSplits == 2) {
        lastNormals[1] = lastNormals[0];
        lastNormals[0] = collider.hit.normal;
      }
      else {
        dot = lastNormals[1] * collider.hit.normal;

        if (dot < 0.0f) {
          Vec3  cross    = collider.hit.normal ^ lastNormals[1];
          float crossSqN = cross.sqN();

          if (crossSqN == 0.0f) {
            move = Vec3::ZERO;
          }
          else {
            float length_1 = Math::fastInvSqrt(crossSqN);

            cross /= length_1;
            move   = (move * cross) * cross;
            move  += 3.0f * MOVE_BOUNCE * length_1 * (collider.hit.normal + lastNormals[1]);
          }
        }
      }
    }
  }
  while (true);

  orbis.reposition(dyn);

  return realisedMove;
}

//***********************************
//*   FRAGMENT COLLISION HANDLING   *
//***********************************

void Physics::handleFragHit()
{
  Vec3  fragVelocity = frag->velocity;
  float velocity2    = frag->velocity.sqN();

  frag->velocity *= frag->elasticity;
  frag->velocity -= (2.0f * (frag->velocity * collider.hit.normal)) * collider.hit.normal;

  if (velocity2 > FRAG_HIT_VELOCITY2) {
    if (frag->mass != 0.0f) {
      if (collider.hit.str != nullptr) {
        Struct* str = collider.hit.str;
        float damage = FRAG_DAMAGE_COEF * velocity2 * frag->mass;

        if (damage > str->resistance) {
          damage *= FRAG_FIXED_DAMAGE + (1.0f - FRAG_FIXED_DAMAGE) * Math::rand();
          str->damage(damage);
        }
      }
      else if (collider.hit.obj != nullptr) {
        Object* obj = collider.hit.obj;
        float damage = FRAG_DAMAGE_COEF * velocity2 * frag->mass;

        if (damage > obj->resistance) {
          damage *= FRAG_FIXED_DAMAGE + (1.0f - FRAG_FIXED_DAMAGE) * Math::rand();
          obj->damage(damage);
        }

        if (obj->flags & Object::DYNAMIC_BIT) {
          Dynamic* dynObj = static_cast<Dynamic*>(obj);

          float fragMass = frag->mass * 10.0f;
          float massSum  = fragMass + dynObj->mass;

          dynObj->flags   &= ~Object::DISABLED_BIT;
          dynObj->momentum = (fragVelocity * fragMass + dynObj->momentum * dynObj->mass) / massSum;
        }
      }
    }

    if (velocity2 > FRAG_DESTROY_VELOCITY2) {
      // We abuse velocity to hold the normal of the fatal hit, needed for positioning decals.
      frag->velocity = collider.hit.normal;
      frag->life     = -Math::INF;
    }
  }
}

void Physics::handleFragMove()
{
  move = frag->velocity * Timer::TICK_TIME;

  float leftRatio = 1.0f;

  int traceSplits = 0;
  do {
    collider.translate(frag->p, move);
    frag->p += collider.hit.ratio * move;
    leftRatio -= leftRatio * collider.hit.ratio;

    if (collider.hit.ratio == 1.0f) {
      break;
    }
    // Collision response.
    handleFragHit();

    // We must check lifeTime <= 0.0f to prevent an already destroyed fragment to bounce off a
    // surface and hit something and to position decal properly.
    if (traceSplits >= 3 || frag->life <= 0.0f) {
      break;
    }
    ++traceSplits;

    move *= 1.0f - collider.hit.ratio;
    move -= (move * collider.hit.normal - MOVE_BOUNCE) * collider.hit.normal;
  }
  while (true);

  orbis.reposition(frag);
}

//***********************************
//*             PUBLIC              *
//***********************************

void Physics::updateEnt(Entity* ent, const Vec3& localMove)
{
  const EntityClass* clazz = ent->clazz;

  if (clazz->flags & EntityClass::IGNORANT) {
    ent->offset  += localMove;
    ent->velocity = localMove / Timer::TICK_TIME;
  }
  else {
    collider.translate(ent, localMove);

    if (collider.hit.obj != nullptr) {
      Object* sObj = collider.hit.obj;

      if (sObj->flags & Object::DYNAMIC_BIT) {
        Dynamic* sDyn = static_cast<Dynamic*>(sObj);

        sDyn->momentum -= 8.0f * collider.hit.normal;
        sDyn->flags    &= ~Object::DISABLED_BIT;
      }
    }

    Vec3 realisedMove = collider.hit.ratio * localMove;

    ent->offset  += realisedMove;
    ent->velocity = realisedMove / Timer::TICK_TIME;
  }
}

void Physics::updateObj(Dynamic* dyn_)
{
  OZ_ASSERT(dyn_->cell != nullptr);

  dyn         = dyn_;
  dyn->flags &= ~Object::TICK_CLEAR_MASK;

  if (dyn->lower != -1) {
    if (dyn->flags & Object::ON_FLOOR_BIT) {
      const Entity* ent = orbis.ent(dyn->lower);

      if (ent == nullptr) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower  = -1;
      }
      else if (ent->state == Entity::OPENING || ent->state == Entity::CLOSING) {
        dyn->flags &= ~Object::DISABLED_BIT;
      }
    }
    else {
      const Object* sObj = orbis.obj(dyn->lower);

      // Clear the lower object if it doesn't exist any more.
      if (sObj == nullptr || sObj->cell == nullptr) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower  = -1;
      }
      else {
        dyn->flags &= sObj->flags | ~Object::DISABLED_BIT;
      }
    }
  }

  // Handle physics.
  if (!(dyn->flags & Object::DISABLED_BIT)) {
    if (handleObjFriction()) {
      int oldFlags = dyn->flags;

      dyn->flags &= ~(Object::MOVE_CLEAR_MASK | Object::ENABLE_BIT);
      dyn->lower  = -1;

      collider.mask = dyn->flags & Object::SOLID_BIT;
      Vec3 realisedMove = handleObjMove();
      collider.mask = Object::SOLID_BIT;

      if (collider.hit.medium & Medium::LADDER_BIT) {
        dyn->flags |= Object::ON_LADDER_BIT;
      }
      if (collider.hit.medium & Medium::WATER_BIT) {
        dyn->flags |= Object::IN_LIQUID_BIT;

        if (!(oldFlags & Object::IN_LIQUID_BIT) && dyn->velocity.z <= SPLASH_THRESHOLD) {
          float momentum2 = dyn->velocity.z*dyn->velocity.z;
          float intensity = momentum2 * SPLASH_INTENSITY_COEF;

          dyn->addEvent(Object::EVENT_SPLASH, intensity);
        }
      }
      if (collider.hit.medium & Medium::LAVA_BIT) {
        dyn->flags |= Object::IN_LIQUID_BIT | Object::IN_LAVA_BIT;

        if (dyn->resistance <= LAVA_DAMAGE_ABSOLUTE) {
          dyn->flags |= Object::ENABLE_BIT;

          if ((timer.ticks + uint64(dyn->index) * 1025u) % LAVA_DAMAGE_INTERVAL == 0) {
            dyn->damage(max(LAVA_DAMAGE_ABSOLUTE, dyn->clazz->life * LAVA_DAMAGE_RATIO));
          }
        }
      }

      dyn->velocity = realisedMove / Timer::TICK_TIME;
      dyn->momentum = dyn->velocity;
      dyn->depth    = min(collider.hit.depth, 2.0f * dyn->dim.z);
    }
    else {
      OZ_ASSERT(dyn->momentum == Vec3::ZERO);

      dyn->flags   |= Object::DISABLED_BIT;
      dyn->velocity = Vec3::ZERO;
    }
  }
}

void Physics::updateFrag(Frag* frag_)
{
  frag = frag_;

  OZ_ASSERT(frag->cell != nullptr);

  frag->velocity.z += gravity * Timer::TICK_TIME;

  handleFragMove();
}

Physics physics;

}
