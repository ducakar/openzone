/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <common/Timer.hh>
#include <matrix/Collider.hh>
#include <matrix/Synapse.hh>
#include <matrix/Bot.hh>

namespace oz
{

static const Mat44 ROTATIONS[] =
{
  Mat44(  1.0f,  0.0f,  0.0f,  0.0f,
          0.0f,  1.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f,  0.0f,
          0.0f,  0.0f,  0.0f,  1.0f ),
  Mat44(  0.0f,  1.0f,  0.0f,  0.0f,
         -1.0f,  0.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f,  0.0f,
          0.0f,  0.0f,  0.0f,  1.0f ),
  Mat44( -1.0f,  0.0f,  0.0f,  0.0f,
          0.0f, -1.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f,  0.0f,
          0.0f,  0.0f,  0.0f,  1.0f ),
  Mat44(  0.0f, -1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f,  0.0f,
          0.0f,  0.0f,  0.0f,  1.0f ),
  Mat44(  1.0f,  0.0f,  0.0f,  0.0f,
          0.0f,  1.0f,  0.0f,  0.0f,
          0.0f,  0.0f,  1.0f,  0.0f,
          0.0f,  0.0f,  0.0f,  1.0f ),
};

const Vec3  Struct::DESTRUCT_FRAG_VELOCITY = Vec3( 0.0f, 0.0f, 2.0f );
const float Struct::DEMOLISH_SPEED         = 8.0f;

const Entity::Handler Entity::HANDLERS[] = {
  &Entity::staticHandler,
  &Entity::manualDoorHandler,
  &Entity::autoDoorHandler,
  &Entity::ignoringBlockHandler,
  &Entity::crushingBlockHandler,
  &Entity::elevatorHandler
};

List<Object*> Struct::overlappingObjs;
Pool<Struct>  Struct::pool;

void Entity::trigger()
{
  if( clazz->target < 0 || key < 0 ) {
    return;
  }

  if( clazz->type == EntityClass::STATIC ) {
    state = OPENING;
    hard_assert( time == 0.0f );
  }

  int strIndex = clazz->target >> Struct::MAX_ENT_SHIFT;
  int entIndex = clazz->target & ( Struct::MAX_ENTITIES - 1 );

  Struct* targetStr = orbis.str( strIndex );

  if( targetStr != nullptr ) {
    Entity& target = targetStr->entities[entIndex];

    if( target.state == OPENED || target.state == OPENING ) {
      target.state = CLOSING;
      target.time = 0.0f;
      target.velocity = -target.clazz->move * target.clazz->ratioInc / Timer::TICK_TIME;
    }
    else {
      target.state = OPENING;
      target.time = 0.0f;
      target.velocity = target.clazz->move * target.clazz->ratioInc / Timer::TICK_TIME;
    }
  }
}

void Entity::lock( Bot* user )
{
  if( key == 0 ) {
    return;
  }

  if( user->clazz->key == key || user->clazz->key == ~key ) {
    key = ~key;
    return;
  }

  foreach( i, user->items.citer() ) {
    Object* obj = orbis.obj( *i );

    if( obj->clazz->key == key || obj->clazz->key == ~key ) {
      key = ~key;
      return;
    }
  }
}

void Entity::staticHandler()
{
  state = CLOSED;
}

void Entity::manualDoorHandler()
{
  switch( state ) {
    case CLOSED: {
      return;
    }
    case OPENING: {
      ratio  = min( ratio + clazz->ratioInc, 1.0f );
      time  += Timer::TICK_TIME;
      offset = ratio * clazz->move;

      if( ratio == 1.0f ) {
        state    = OPENED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      return;
    }
    case CLOSING: {
      time  += Timer::TICK_TIME;
      offset = Vec3::ZERO;

      if( collider.overlaps( this ) ) {
        offset = ratio * clazz->move;

        if( ratio == 1.0f ) {
          state = OPENED;
          time  = 0.0f;
        }
        else {
          state    = OPENING;
          time     = 0.0f;
          velocity = clazz->move * clazz->ratioInc / Timer::TICK_TIME;
        }
        return;
      }

      ratio  = max( ratio - clazz->ratioInc, 0.0f );
      offset = ratio * clazz->move;

      if( ratio == 0.0f ) {
        state    = CLOSED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::autoDoorHandler()
{
  switch( state ) {
    case CLOSED: {
      if( ( timer.ticks + uint( str->index * 1025 ) ) % ( Timer::TICKS_PER_SEC / 6 ) == 0 &&
          collider.overlaps( this, clazz->margin ) )
      {
        state    = OPENING;
        velocity = clazz->move * clazz->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      ratio  = min( ratio + clazz->ratioInc, 1.0f );
      time  += Timer::TICK_TIME;
      offset = ratio * clazz->move;

      if( ratio == 1.0f ) {
        state    = OPENED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      time += Timer::TICK_TIME;

      if( time > clazz->timeout ) {
        offset = Vec3::ZERO;
        time   = 0.0f;

        if( !collider.overlaps( this, clazz->margin ) ) {
          state    = CLOSING;
          velocity = -clazz->move * clazz->ratioInc / Timer::TICK_TIME;
        }

        offset = clazz->move;
      }
      return;
    }
    case CLOSING: {
      time  += Timer::TICK_TIME;
      offset = Vec3::ZERO;

      if( collider.overlaps( this, clazz->margin ) ) {
        offset = ratio * clazz->move;

        if( ratio == 1.0f ) {
          state = OPENED;
          time  = 0.0f;
        }
        else {
          state    = OPENING;
          time     = 0.0f;
          velocity = clazz->move * clazz->ratioInc / Timer::TICK_TIME;
        }
        return;
      }

      ratio  = max( ratio - clazz->ratioInc, 0.0f );
      offset = ratio * clazz->move;

      if( ratio == 0.0f ) {
        state    = CLOSED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::ignoringBlockHandler()
{
  time += Timer::TICK_TIME;

  switch( state ) {
    case CLOSED: {
      if( time > clazz->timeout ) {
        state    = OPENING;
        time     = 0.0f;
        velocity = clazz->move * clazz->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      ratio  = min( ratio + clazz->ratioInc, 1.0f );
      offset = ratio * clazz->move;

      if( ratio == 1.0f ) {
        state    = OPENED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      if( time > clazz->timeout ) {
        state    = CLOSING;
        time     = 0.0f;
        velocity = -clazz->move * clazz->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case CLOSING: {
      ratio  = max( ratio - clazz->ratioInc, 0.0f );
      offset = ratio * clazz->move;

      if( ratio == 0.0f ) {
        state    = CLOSED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::crushingBlockHandler()
{
  time += Timer::TICK_TIME;

  switch( state ) {
    case CLOSED: {
      if( time > clazz->timeout ) {
        state    = OPENING;
        time     = 0.0f;
        velocity = clazz->move * clazz->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      Vec3 move = offset;

      ratio  = min( ratio + clazz->ratioInc, 1.0f );
      offset = ratio * clazz->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move + 2.0f*EPSILON * clazz->move.fastUnit();
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              Vec3 dynMove  = collider.hit.ratio * move;
              Vec3 velDelta = dynMove / Timer::TICK_TIME;

              dyn->p        += dynMove;
              dyn->velocity += velDelta;
              dyn->momentum += velDelta;
              dyn->flags    &= ~Object::DISABLED_BIT;
              dyn->flags    |= Object::ENABLE_BIT;

              orbis.reposition( dyn );
            }
            if( collider.hit.ratio != 1.0f && collider.overlapsEntity( *dyn, this ) ) {
              dyn->destroy();
            }
          }
        }
      }

      if( ratio == 1.0f ) {
        state    = OPENED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      if( time > clazz->timeout ) {
        state    = CLOSING;
        time     = 0.0f;
        velocity = -clazz->move * clazz->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case CLOSING: {
      Vec3 move = offset;

      ratio  = max( ratio - clazz->ratioInc, 0.0f );
      offset = ratio * clazz->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move - 2.0f*EPSILON * clazz->move.fastUnit();
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              Vec3 dynMove  = collider.hit.ratio * move;
              Vec3 velDelta = dynMove / Timer::TICK_TIME;

              dyn->p        += dynMove;
              dyn->velocity += velDelta;
              dyn->momentum += velDelta;
              dyn->flags    &= ~Object::DISABLED_BIT;
              dyn->flags    |= Object::ENABLE_BIT;

              orbis.reposition( dyn );
            }
            if( collider.hit.ratio != 1.0f && collider.overlapsEntity( *dyn, this ) ) {
              dyn->destroy();
            }
          }
        }
      }

      if( ratio == 0.0f ) {
        state    = CLOSED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::elevatorHandler()
{
  switch( state ) {
    case CLOSED: {
      return;
    }
    case OPENING: {
      float originalRatio  = ratio;
      Vec3  originalOffset = offset;

      Vec3 move = offset;

      ratio  = min( ratio + clazz->ratioInc, 1.0f );
      time  += Timer::TICK_TIME;
      offset = ratio * clazz->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move + 2.0f*EPSILON * clazz->move.fastUnit();
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              dyn->p.z   += collider.hit.ratio * move.z;
              dyn->flags &= ~Object::DISABLED_BIT;
              dyn->flags |= Object::ENABLE_BIT;
            }
            if( collider.hit.ratio != 1.0f && collider.overlapsEntity( *dyn, this ) ) {
              ratio    = originalRatio;
              offset   = originalOffset;

              state    = ratio == 0.0f ? CLOSED : OPENED;
              time     = 0.0f;
              velocity = Vec3::ZERO;
              return;
            }
          }
        }
      }

      if( ratio == 1.0f ) {
        state    = OPENED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      return;
    }
    case CLOSING: {
      float originalRatio  = ratio;
      Vec3  originalOffset = offset;

      Vec3 move = offset;

      ratio  = max( ratio - clazz->ratioInc, 0.0f );
      time  += Timer::TICK_TIME;
      offset = ratio * clazz->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move - 2.0f*EPSILON * clazz->move.fastUnit();
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              dyn->p.z   += collider.hit.ratio * move.z;
              dyn->flags &= ~Object::DISABLED_BIT;
              dyn->flags |= Object::ENABLE_BIT;
            }
            if( collider.hit.ratio != 1.0f && collider.overlapsEntity( *dyn, this ) ) {
              ratio    = originalRatio;
              offset   = originalOffset;

              state    = ratio == 1.0f ? OPENED : CLOSED;
              time     = 0.0f;
              velocity = Vec3::ZERO;
              return;
            }
          }
        }
      }

      if( ratio == 0.0f ) {
        state    = CLOSED;
        time     = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Struct::onDemolish()
{
  overlappingObjs.clear();

  collider.mask = ~0;
  collider.getOverlaps( toAABB(), nullptr, &overlappingObjs, 4.0f * EPSILON );
  collider.mask = Object::SOLID_BIT;

  for( int i = 0; i < overlappingObjs.length(); ++i ) {
    Dynamic* dyn = static_cast<Dynamic*>( overlappingObjs[i] );

    if( ( dyn->flags & Object::SOLID_BIT ) &&
        collider.overlaps( AABB( *dyn, -2.0f * EPSILON ), dyn ) )
    {
      dyn->destroy();
    }
    else if( dyn->flags & Object::DYNAMIC_BIT ) {
      dyn->flags &= ~Object::DISABLED_BIT;
      dyn->flags |= Object::ENABLE_BIT;
    }
  }

  float deltaHeight = DEMOLISH_SPEED * Timer::TICK_TIME;
  demolishing += deltaHeight / ( maxs.z - mins.z );
  p.z -= deltaHeight;

  transf.w = Vec4( p );

  invTransf = ROTATIONS[4 - heading];
  invTransf.translate( Point::ORIGIN - p );

  Bounds bb = toAbsoluteCS( *bsp );
  mins = bb.mins;
  maxs = bb.maxs;
}

void Struct::onUpdate()
{
  for( int i = 0; i < boundObjects.length(); ) {
    if( orbis.obj( boundObjects[i] ) == nullptr ) {
      boundObjects.eraseUnordered( i );
    }
    else {
      ++i;
    }
  }

  if( life == 0.0f ) {
    onDemolish();
  }
  else {
    for( int i = 0; i < entities.length(); ++i ) {
      Entity& entity = entities[i];

      hard_assert( 0.0f <= entity.ratio && entity.ratio <= 1.0f );

      ( entity.*Entity::HANDLERS[entity.clazz->type] )();
    }
  }
}

Bounds Struct::toStructCS( const Bounds& bb ) const
{
  switch( heading ) {
    case NORTH: {
      return Bounds( Point( +bb.mins.x - p.x, +bb.mins.y - p.y, +bb.mins.z - p.z ),
                     Point( +bb.maxs.x - p.x, +bb.maxs.y - p.y, +bb.maxs.z - p.z ) );
    }
    case WEST: {
      return Bounds( Point( +bb.mins.y - p.y, -bb.maxs.x + p.x, +bb.mins.z - p.z ),
                     Point( +bb.maxs.y - p.y, -bb.mins.x + p.x, +bb.maxs.z - p.z ) );
    }
    case SOUTH: {
      return Bounds( Point( -bb.maxs.x + p.x, -bb.maxs.y + p.y, +bb.mins.z - p.z ),
                     Point( -bb.mins.x + p.x, -bb.mins.y + p.y, +bb.maxs.z - p.z ) );
    }
    case EAST: {
      return Bounds( Point( -bb.maxs.y + p.y, +bb.mins.x - p.x, +bb.mins.z - p.z ),
                     Point( -bb.mins.y + p.y, +bb.maxs.x - p.x, +bb.maxs.z - p.z ) );
    }
  }
}

Bounds Struct::toAbsoluteCS( const Bounds& bb ) const
{
  switch( heading ) {
    case NORTH: {
      return Bounds( p + Vec3( +bb.mins.x, +bb.mins.y, +bb.mins.z ),
                     p + Vec3( +bb.maxs.x, +bb.maxs.y, +bb.maxs.z ) );
    }
    case WEST: {
      return Bounds( p + Vec3( -bb.maxs.y, +bb.mins.x, +bb.mins.z ),
                     p + Vec3( -bb.mins.y, +bb.maxs.x, +bb.maxs.z ) );
    }
    case SOUTH: {
      return Bounds( p + Vec3( -bb.maxs.x, -bb.maxs.y, +bb.mins.z ),
                     p + Vec3( -bb.mins.x, -bb.mins.y, +bb.maxs.z ) );
    }
    case EAST: {
      return Bounds( p + Vec3( +bb.mins.y, -bb.maxs.x, +bb.mins.z ),
                     p + Vec3( +bb.maxs.y, -bb.mins.x, +bb.maxs.z ) );
    }
  }
}

void Struct::destroy()
{
  for( int i = 0; i < boundObjects.length(); ++i ) {
    Object* obj = orbis.obj( boundObjects[i] );

    if( obj != nullptr ) {
      obj->destroy();
    }
  }

  onDemolish();

  if( bsp->fragPool != nullptr ) {
    synapse.gen( bsp->fragPool,
                 bsp->nFrags,
                 Bounds( Point( mins.x, mins.y, 0.5f * ( mins.z + maxs.z ) ), maxs ),
                 DESTRUCT_FRAG_VELOCITY );
  }
}

Struct::Struct( const BSP* bsp_, int index_, const Point& p_, Heading heading_ )
{
  bsp         = bsp_;

  p           = p_;
  heading     = heading_;

  index       = index_;

  life        = bsp->life;
  resistance  = bsp->resistance;
  demolishing = 0.0f;

  transf      = Mat44::translation( p - Point::ORIGIN ) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat44::translation( Point::ORIGIN - p );

  hard_assert( transf.det() != 0.0f );

  Bounds bb   = toAbsoluteCS( *bsp );
  mins        = bb.mins;
  maxs        = bb.maxs;

  if( bsp->nEntities != 0 ) {
    entities.resize( bsp->nEntities );

    for( int i = 0; i < entities.length(); ++i ) {
      Entity& entity = entities[i];

      entity.clazz    = &bsp->entities[i];
      entity.str      = this;
      entity.key      = bsp->entities[i].key;
      entity.state    = Entity::CLOSED;
      entity.ratio    = 0.0f;
      entity.time     = 0.0f;
      entity.offset   = Vec3::ZERO;
      entity.velocity = Vec3::ZERO;
    }
  }
}

Struct::Struct( const BSP* bsp_, InputStream* is )
{
  bsp         = bsp_;

  p           = is->readPoint();
  heading     = Heading( is->readInt() );

  index       = is->readInt();

  life        = is->readFloat();
  resistance  = bsp->resistance;
  demolishing = is->readFloat();

  transf      = Mat44::translation( p - Point::ORIGIN ) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat44::translation( Point::ORIGIN - p );

  Bounds bb   = toAbsoluteCS( *bsp );
  mins        = bb.mins;
  maxs        = bb.maxs;

  if( bsp->nEntities != 0 ) {
    entities.resize( bsp->nEntities );

    for( int i = 0; i < entities.length(); ++i ) {
      Entity& entity = entities[i];

      entity.clazz  = &bsp->entities[i];
      entity.str    = this;
      entity.key    = is->readInt();
      entity.state  = Entity::State( is->readInt() );
      entity.ratio  = is->readFloat();
      entity.time   = is->readFloat();
      entity.offset = is->readVec3();

      if( entity.state == Entity::OPENING ) {
        entity.velocity = +entity.clazz->move * entity.clazz->ratioInc / Timer::TICK_TIME;
      }
      else if( entity.state == Entity::CLOSING ) {
        entity.velocity = -entity.clazz->move * entity.clazz->ratioInc / Timer::TICK_TIME;
      }
      else {
        entity.velocity = Vec3::ZERO;
      }
    }
  }

  int nBoundObjects = is->readInt();
  hard_assert( nBoundObjects <= bsp->nBoundObjects );

  if( bsp->nBoundObjects != 0 ) {
    boundObjects.allocate( bsp->nBoundObjects );

    for( int i = 0; i < nBoundObjects; ++i ) {
      boundObjects.add( is->readInt() );
    }
  }
}

Struct::Struct( const BSP* bsp_, const JSON& json )
{
  bsp         = bsp_;

  p           = json["p"].asPoint();
  heading     = Heading( json["heading"].asInt() );

  index       = json["index"].asInt();

  life        = json["life"].asFloat();
  resistance  = bsp->resistance;
  demolishing = json["demolishing"].asFloat();

  transf      = Mat44::translation( p - Point::ORIGIN ) ^ ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] ^ Mat44::translation( Point::ORIGIN - p );

  Bounds bb   = toAbsoluteCS( *bsp );
  mins        = bb.mins;
  maxs        = bb.maxs;

  if( bsp->nEntities != 0 ) {
    entities.resize( bsp->nEntities );

    const JSON& entitiesJSON = json["entities"];

    for( int i = 0; i < entities.length(); ++i ) {
      const JSON& entityJSON = entitiesJSON[i];
      Entity&     entity     = entities[i];

      entity.clazz  = &bsp->entities[i];
      entity.str    = this;
      entity.key    = entityJSON["key"].asInt();
      entity.state  = Entity::State( entityJSON["state"].asInt() );
      entity.ratio  = entityJSON["ratio"].asFloat();
      entity.time   = entityJSON["time"].asFloat();
      entity.offset = entityJSON["offset"].asVec3();

      if( entity.state == Entity::OPENING ) {
        entity.velocity = +entity.clazz->move * entity.clazz->ratioInc / Timer::TICK_TIME;
      }
      else if( entity.state == Entity::CLOSING ) {
        entity.velocity = -entity.clazz->move * entity.clazz->ratioInc / Timer::TICK_TIME;
      }
      else {
        entity.velocity = Vec3::ZERO;
      }
    }
  }

  const JSON& boundObjectsJSON = json["boundObjects"];
  hard_assert( boundObjectsJSON.length() <= bsp->nBoundObjects );

  if( bsp->nBoundObjects != 0 ) {
    boundObjects.allocate( bsp->nBoundObjects );

    foreach( i, boundObjectsJSON.arrayCIter() ) {
      boundObjects.add( i->asInt() );
    }
  }
}

void Struct::write( OutputStream* os ) const
{
  os->writePoint( p );
  os->writeInt( heading );

  os->writeInt( index );

  os->writeFloat( life );
  os->writeFloat( demolishing );

  for( int i = 0; i < entities.length(); ++i ) {
    os->writeInt( entities[i].key );
    os->writeInt( entities[i].state );
    os->writeFloat( entities[i].ratio );
    os->writeFloat( entities[i].time );
    os->writeVec3( entities[i].offset );
  }

  os->writeInt( boundObjects.length() );
  for( int i = 0; i < boundObjects.length(); ++i ) {
    os->writeInt( boundObjects[i] );
  }
}

JSON Struct::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "bsp", bsp->name );

  json.add( "p", p );
  json.add( "heading", heading );

  json.add( "index", index );

  json.add( "life", life );
  json.add( "demolishing", demolishing );

  JSON& entitiesJSON = json.add( "entities", JSON::ARRAY );

  for( int i = 0; i < entities.length(); ++i ) {
    JSON& entityJSON = entitiesJSON.add( JSON::OBJECT);

    entityJSON.add( "key", entities[i].key );
    entityJSON.add( "state", entities[i].state );
    entityJSON.add( "ratio", entities[i].ratio );
    entityJSON.add( "time", entities[i].time );
    entityJSON.add( "offset", entities[i].offset );
  }

  JSON& boundObjectsJSON = json.add( "boundObjects", JSON::ARRAY );

  for( int i = 0; i < boundObjects.length(); ++i ) {
    boundObjectsJSON.add( boundObjects[i] );
  }

  return json;
}

}
