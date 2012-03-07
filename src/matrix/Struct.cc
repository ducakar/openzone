/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "stable.hh"

#include "matrix/Struct.hh"

#include "matrix/Collider.hh"
#include "matrix/Synapse.hh"
#include "matrix/Bot.hh"

namespace oz
{
namespace matrix
{

const Mat44 Struct::ROTATIONS[] =
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
const float Struct::DEMOLISH_SPEED         = 8.0f * Timer::TICK_TIME;
const float Struct::MOMENTUM_DAMAGE_COEF   = 0.01f;
const float Struct::MAX_HIT_DAMAGE_MASS    = 100.0f;

const Entity::Handler Entity::HANDLERS[] = {
  &Entity::staticHandler,
  &Entity::manualDoorHandler,
  &Entity::autoDoorHandler,
  &Entity::ignoringBlockHandler,
  &Entity::crushingBlockHandler,
  &Entity::elevatorHandler
};

Vector<Object*> Struct::overlappingObjs;
Pool<Struct>    Struct::pool;

void Entity::trigger()
{
  if( model->target == -1 || key > 0 ) {
    return;
  }

  if( model->type == Model::STATIC ) {
    state = OPENING;
  }

  int strIndex = model->target / Struct::MAX_ENTITIES;
  int entIndex = model->target % Struct::MAX_ENTITIES;

  Struct* str = orbis.structs[strIndex];

  if( str != null ) {
    Entity& entity = str->entities[entIndex];

    if( entity.state == OPENED || entity.state == OPENING ) {
      entity.state = CLOSING;
      entity.time = 0.0f;
      entity.velocity = -entity.model->move * entity.model->ratioInc / Timer::TICK_TIME;
    }
    else {
      entity.state = OPENING;
      entity.time = 0.0f;
      entity.velocity = entity.model->move * entity.model->ratioInc / Timer::TICK_TIME;
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
    Object* obj = orbis.objects[*i];

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
  time += Timer::TICK_TIME;

  switch( state ) {
    case CLOSED: {
      return;
    }
    case OPENING: {
      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      if( ratio == 1.0f ) {
        state = OPENED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      return;
    }
    case CLOSING: {
      offset = Vec3::ZERO;

      if( collider.overlaps( this ) ) {
        offset = ratio * model->move;

        if( ratio == 1.0f ) {
          state = OPENED;
          time = 0.0f;
        }
        else {
          state = OPENING;
          time = 0.0f;
          velocity = model->move * model->ratioInc / Timer::TICK_TIME;
        }
        return;
      }

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      if( ratio == 0.0f ) {
        state = CLOSED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::autoDoorHandler()
{
  time += Timer::TICK_TIME;

  switch( state ) {
    case CLOSED: {
      if( ( timer.ticks + uint( str->index ) ) % 8 != 0 ) {
        return;
      }

      if( collider.overlaps( this, model->margin ) ) {
        state = OPENING;
        time = 0.0f;
        velocity = model->move * model->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      if( ratio == 1.0f ) {
        state = OPENED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      if( time >= model->timeout ) {
        time = 0.0f;

        offset = Vec3::ZERO;

        if( !collider.overlaps( this, model->margin ) ) {
          state = CLOSING;
          velocity = -model->move * model->ratioInc / Timer::TICK_TIME;
        }

        offset = model->move;
      }
      return;
    }
    case CLOSING: {
      offset = Vec3::ZERO;

      if( collider.overlaps( this, model->margin ) ) {
        offset = ratio * model->move;

        if( ratio == 1.0f ) {
          state = OPENED;
          time = 0.0f;
        }
        else {
          state = OPENING;
          time = 0.0f;
          velocity = model->move * model->ratioInc / Timer::TICK_TIME;
        }
        return;
      }

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      if( ratio == 0.0f ) {
        state = CLOSED;
        time = 0.0f;
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
      if( time >= model->timeout ) {
        state = OPENING;
        time = 0.0f;
        velocity = model->move * model->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      if( ratio == 1.0f ) {
        state = OPENED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      if( time >= model->timeout ) {
        state = CLOSING;
        time = 0.0f;
        velocity = -model->move * model->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case CLOSING: {
      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      if( ratio == 0.0f ) {
        state = CLOSED;
        time = 0.0f;
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
      if( time >= model->timeout ) {
        state = OPENING;
        time = 0.0f;
        velocity = model->move * model->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case OPENING: {
      Vec3 move = offset;

      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move + 4.0f*EPSILON * ~model->move;
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              Vec3 dynMove  = collider.hit.ratio * move;
              Vec3 velDelta = dynMove / Timer::TICK_TIME;

              dyn->p        += dynMove;
              dyn->momentum += velDelta;
              dyn->velocity += velDelta;
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
        state = OPENED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      if( time >= model->timeout ) {
        state = CLOSING;
        time = 0.0f;
        velocity = -model->move * model->ratioInc / Timer::TICK_TIME;
      }
      return;
    }
    case CLOSING: {
      Vec3 move = offset;

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move - 4.0f*EPSILON * ~model->move;
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < Struct::overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( Struct::overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio != 0.0f ) {
              Vec3 dynMove  = collider.hit.ratio * move;
              Vec3 velDelta = dynMove / Timer::TICK_TIME;

              dyn->p        += dynMove;
              dyn->momentum += velDelta;
              dyn->velocity += velDelta;
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
        state = CLOSED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
  }
}

void Entity::elevatorHandler()
{
  time += Timer::TICK_TIME;

  switch( state ) {
    case CLOSED: {
      return;
    }
    case OPENING: {
      float originalRatio = ratio;
      Vec3 originalOffset = offset;

      Vec3 move = offset;

      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move + EPSILON * ~model->move;
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
        state = OPENED;
        time = 0.0f;
        velocity = Vec3::ZERO;
      }
      return;
    }
    case OPENED: {
      return;
    }
    case CLOSING: {
      float originalRatio = ratio;
      Vec3 originalOffset = offset;

      Vec3 move = offset;

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      Struct::overlappingObjs.clear();
      collider.getOverlaps( this, &Struct::overlappingObjs );

      if( !Struct::overlappingObjs.isEmpty() ) {
        move = offset - move - EPSILON * ~model->move;
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
        state = CLOSED;
        time = 0.0f;
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
  collider.getOverlaps( toAABB(), null, &overlappingObjs, 4.0f * EPSILON );
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

  demolishing += DEMOLISH_SPEED / ( maxs.z - mins.z );
  p.z -= DEMOLISH_SPEED;

  transf.w = Vec4( p );

  invTransf = ROTATIONS[4 - heading];
  invTransf.translate( Point3::ORIGIN - p );

  Bounds bb = toAbsoluteCS( *bsp );
  mins = bb.mins;
  maxs = bb.maxs;
}

void Struct::onUpdate()
{
  for( int i = 0; i < boundObjects.length(); ) {
    if( orbis.objects[ boundObjects[i] ] == null ) {
      boundObjects.removeUO( i );
    }
    else {
      ++i;
    }
  }

  if( life <= 0.0f ) {
    onDemolish();
  }
  else {
    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      hard_assert( 0.0f <= entity.ratio && entity.ratio <= 1.0f );

      ( entity.*Entity::HANDLERS[entity.model->type] )();
    }
  }
}

Struct::~Struct()
{
  delete[] entities;
}

Bounds Struct::toStructCS( const Bounds& bb ) const
{
  switch( heading ) {
    case NORTH: {
      return Bounds( Point3( +bb.mins.x - p.x, +bb.mins.y - p.y, +bb.mins.z - p.z ),
                     Point3( +bb.maxs.x - p.x, +bb.maxs.y - p.y, +bb.maxs.z - p.z ) );
    }
    case WEST: {
      return Bounds( Point3( +bb.mins.y - p.y, -bb.maxs.x + p.x, +bb.mins.z - p.z ),
                     Point3( +bb.maxs.y - p.y, -bb.mins.x + p.x, +bb.maxs.z - p.z ) );
    }
    case SOUTH: {
      return Bounds( Point3( -bb.maxs.x + p.x, -bb.maxs.y + p.y, +bb.mins.z - p.z ),
                     Point3( -bb.mins.x + p.x, -bb.mins.y + p.y, +bb.maxs.z - p.z ) );
    }
    case EAST: {
      return Bounds( Point3( -bb.maxs.y + p.y, +bb.mins.x - p.x, +bb.mins.z - p.z ),
                     Point3( -bb.mins.y + p.y, +bb.maxs.x - p.x, +bb.maxs.z - p.z ) );
    }
  }
}

Bounds Struct::rotate( const Bounds& in, Heading heading )
{
  Point3 p = in.mins + ( in.maxs - in.mins ) * 0.5f;

  switch( heading ) {
    case NORTH: {
      return Bounds( p + Vec3( +in.mins.x, +in.mins.y, +in.mins.z ),
                     p + Vec3( +in.maxs.x, +in.maxs.y, +in.maxs.z ) );
    }
    case WEST: {
      return Bounds( p + Vec3( -in.maxs.y, +in.mins.x, +in.mins.z ),
                     p + Vec3( -in.mins.y, +in.maxs.x, +in.maxs.z ) );
    }
    case SOUTH: {
      return Bounds( p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z ),
                     p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z ) );
    }
    case EAST: {
      return Bounds( p + Vec3( +in.mins.y, -in.maxs.x, +in.mins.z ),
                     p + Vec3( +in.maxs.y, -in.mins.x, +in.maxs.z ) );
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
    Object* obj = orbis.objects[ boundObjects[i] ];

    if( obj != null ) {
      obj->destroy();
    }
  }

  onDemolish();

  if( bsp->fragPool != null ) {
    synapse.gen( bsp->fragPool,
                 bsp->nFrags,
                 Bounds( Point3( mins.x, mins.y, 0.5f * ( mins.z + maxs.z ) ), maxs ),
                 DESTRUCT_FRAG_VELOCITY );
  }
}

Struct::Struct( const BSP* bsp_, int index_, const Point3& p_, Heading heading_ )
{
  bsp         = bsp_;
  p           = p_;
  index       = index_;
  heading     = heading_;

  life        = bsp->life;
  resistance  = bsp->resistance;
  demolishing = 0.0f;

  transf      = Mat44::translation( p - Point3::ORIGIN ) * ROTATIONS[heading];
  invTransf   = ROTATIONS[4 - heading] * Mat44::translation( Point3::ORIGIN - p );

  hard_assert( transf.det() != 0.0f );

  Bounds bb   = toAbsoluteCS( *bsp );
  mins        = bb.mins;
  maxs        = bb.maxs;

  nEntities   = bsp->nModels;
  entities    = nEntities == 0 ? null : new Entity[nEntities];

  for( int i = 0; i < nEntities; ++i ) {
    Entity& entity = entities[i];

    entity.model    = &bsp->models[i];
    entity.str      = this;
    entity.offset   = Vec3::ZERO;
    entity.key      = bsp->models[i].key;
    entity.state    = Entity::CLOSED;
    entity.ratio    = 0.0f;
    entity.time     = 0.0f;
    entity.velocity = Vec3::ZERO;
  }
}

Struct::Struct( const BSP* bsp_, InputStream* istream )
{
  mins        = istream->readPoint3();
  maxs        = istream->readPoint3();
  transf      = istream->readMat44();
  invTransf   = istream->readMat44();

  bsp         = bsp_;
  p           = istream->readPoint3();
  index       = istream->readInt();
  heading     = Heading( istream->readInt() );

  life        = istream->readFloat();
  resistance  = bsp->resistance;
  demolishing = istream->readFloat();

  nEntities   = bsp->nModels;
  entities    = nEntities == 0 ? null : new Entity[nEntities];

  for( int i = 0; i < nEntities; ++i ) {
    Entity& entity = entities[i];

    entity.offset = istream->readVec3();
    entity.model  = &bsp->models[i];
    entity.str    = this;
    entity.key    = istream->readInt();
    entity.state  = Entity::State( istream->readInt() );
    entity.ratio  = istream->readFloat();
    entity.time   = istream->readFloat();

    if( entity.state == Entity::OPENING ) {
      entity.velocity = +entity.model->move * entity.model->ratioInc / Timer::TICK_TIME;
    }
    else if( entity.state == Entity::CLOSING ) {
      entity.velocity = -entity.model->move * entity.model->ratioInc / Timer::TICK_TIME;
    }
    else {
      entity.velocity = Vec3::ZERO;
    }
  }

  int nBoundObjects = istream->readInt();
  hard_assert( nBoundObjects <= bsp->nBoundObjects );

  if( bsp->nBoundObjects != 0 ) {
    boundObjects.alloc( bsp->nBoundObjects );

    for( int i = 0; i < nBoundObjects; ++i ) {
      boundObjects.add( istream->readInt() );
    }
  }
}

void Struct::write( BufferStream* ostream )
{
  ostream->writePoint3( mins );
  ostream->writePoint3( maxs );
  ostream->writeMat44( transf );
  ostream->writeMat44( invTransf );

  ostream->writePoint3( p );
  ostream->writeInt( index );
  ostream->writeInt( heading );

  ostream->writeFloat( life );
  ostream->writeFloat( demolishing );

  for( int i = 0; i < nEntities; ++i ) {
    ostream->writeVec3( entities[i].offset );
    ostream->writeInt( entities[i].key );
    ostream->writeInt( entities[i].state );
    ostream->writeFloat( entities[i].ratio );
    ostream->writeFloat( entities[i].time );
  }

  ostream->writeInt( boundObjects.length() );
  for( int i = 0; i < boundObjects.length(); ++i ) {
    ostream->writeInt( boundObjects[i] );
  }
}

}
}
