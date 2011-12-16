/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

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

void ( Struct::Entity::* const Struct::Entity::HANDLERS[] )() = {
  &Struct::Entity::updateIgnoring,
  &Struct::Entity::updateCrushing,
  &Struct::Entity::updateAutoDoor
};

Vector<Object*> Struct::overlappingObjs;
Pool<Struct> Struct::pool;

void Struct::Entity::updateIgnoring()
{
  switch( state ) {
    case CLOSED: {
      time += Timer::TICK_TIME;

      if( time >= model->timeout ) {
        time = 0.0f;
        state = OPENING;
      }
      break;
    }
    case OPENING: {
      ratio += model->ratioInc;
      time += Timer::TICK_TIME;

      if( ratio >= 1.0f ) {
        ratio = 1.0f;
        time = 0.0f;
        state = OPENED;
      }

      offset = ratio * model->move;
      break;
    }
    case OPENED: {
      time += Timer::TICK_TIME;

      if( time >= model->timeout ) {
        time = 0.0f;
        state = CLOSING;
      }
      break;
    }
    case CLOSING: {
      ratio -= model->ratioInc;
      time += Timer::TICK_TIME;

      if( ratio <= 0.0f ) {
        ratio = 0.0f;
        time = 0.0f;
        state = CLOSED;
      }

      offset = ratio * model->move;
      break;
    }
  }
}

void Struct::Entity::updateCrushing()
{
  switch( state ) {
    case CLOSED: {
      time += Timer::TICK_TIME;

      if( time >= model->timeout ) {
        time = 0.0f;
        state = OPENING;
      }
      break;
    }
    case OPENING: {
      collider.touchOverlaps( str->toAbsoluteCS( *model + offset ).toAABB(), 4.0f * EPSILON );

      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      overlappingObjs.clear();
      collider.getOverlaps( this, &overlappingObjs, 0.5f * EPSILON );

      if( !overlappingObjs.isEmpty() ) {
        Vec3 move = ( model->ratioInc + 4.0f * EPSILON ) * model->move;
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio == 1.0f ) {
              dyn->p += move;
              dyn->velocity += move / Timer::TICK_TIME;
              dyn->flags &= ~Object::DISABLED_BIT;
            }
            else {
              dyn->destroy();
            }
          }
        }
      }
      else if( ratio == 1.0f ) {
        state = OPENED;
      }
      break;
    }
    case OPENED: {
      time += Timer::TICK_TIME;

      if( time >= model->timeout ) {
        time = 0.0f;
        state = CLOSING;
      }
      break;
    }
    case CLOSING: {
      collider.touchOverlaps( str->toAbsoluteCS( *model + offset ).toAABB(), 4.0f * EPSILON );

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      overlappingObjs.clear();
      collider.getOverlaps( this, &overlappingObjs, 0.5f * EPSILON );

      if( !overlappingObjs.isEmpty() ) {
        Vec3 move = ( model->ratioInc + 4.0f * EPSILON ) * -model->move;
        move = str->toAbsoluteCS( move );

        for( int i = 0; i < overlappingObjs.length(); ++i ) {
          Dynamic* dyn = static_cast<Dynamic*>( overlappingObjs[i] );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            collider.translate( dyn, move );

            if( collider.hit.ratio == 1.0f ) {
              dyn->p += move;
              dyn->velocity += move / Timer::TICK_TIME;
              dyn->flags &= ~Object::DISABLED_BIT;
            }
            else {
              dyn->destroy();
            }
          }
        }
      }
      else if( ratio == 0.0f ) {
        state = CLOSED;
      }
      break;
    }
  }
}

void Struct::Entity::updateAutoDoor()
{
  switch( state ) {
    case CLOSED: {
      if( timer.ticks % 10 == 0 ) {
        break;
      }

      if( collider.overlapsOO( this, model->margin ) ) {
        time = 0.0f;
        state = OPENING;
      }
      break;
    }
    case OPENING: {
      ratio = min( ratio + model->ratioInc, 1.0f );
      offset = ratio * model->move;

      if( ratio == 1.0f ) {
        state = OPENED;
      }
      break;
    }
    case OPENED: {
      time += Timer::TICK_TIME;

      if( time >= model->timeout ) {
        time = 0.0f;

        offset = Vec3::ZERO;

        if( !collider.overlapsOO( this, model->margin ) ) {
          state = CLOSING;
        }

        offset = model->move;
      }
      break;
    }
    case CLOSING: {
      offset = Vec3::ZERO;

      if( collider.overlapsOO( this, model->margin ) ) {
        offset = ratio * model->move;
        state = ratio == 1.0f ? OPENED : OPENING;
        break;
      }

      ratio = max( ratio - model->ratioInc, 0.0f );
      offset = ratio * model->move;

      if( ratio == 0.0f ) {
        state = CLOSED;
      }
      break;
    }
  }
}

void Struct::onDemolish()
{
  overlappingObjs.clear();
  collider.mask = ~0;
  collider.getOverlaps( this->toAABB(), &overlappingObjs, null, 4.0f * EPSILON );
  collider.mask = Object::SOLID_BIT;

  for( int i = 0; i < overlappingObjs.length(); ++i ) {
    Dynamic* dyn = static_cast<Dynamic*>( overlappingObjs[i] );

    if( ( dyn->flags & Object::SOLID_BIT ) &&
        collider.overlaps( dyn->toAABB( -2.0f * EPSILON ), dyn ) )
    {
      dyn->destroy();
    }
    else if( dyn->flags & Object::DYNAMIC_BIT ) {
      dyn->flags &= ~Object::MOVE_CLEAR_MASK;
      dyn->lower = -1;
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
    synapse.genFrags( bsp->fragPool,
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

    entity.model  = &bsp->models[i];
    entity.str    = this;
    entity.offset = Vec3::ZERO;
    entity.state  = Entity::CLOSED;
    entity.ratio  = 0.0f;
    entity.time   = 0.0f;
  }

  if( bsp->nBoundObjects != 0 ) {
    boundObjects.alloc( bsp->nBoundObjects );
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
    entity.state  = Entity::State( istream->readInt() );
    entity.ratio  = istream->readFloat();
    entity.time   = istream->readFloat();
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
