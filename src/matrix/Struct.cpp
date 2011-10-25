/*
 *  Struct.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Struct.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"

namespace oz
{

  void ( Struct::Entity::* Struct::Entity::handlers[] )() = {
    &Struct::Entity::updateIgnoring,
    &Struct::Entity::updateCrushing,
    &Struct::Entity::updateAutoDoor
  };

  const Mat44 Struct::rotations[] =
  {
    Mat44::ID,
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
    Mat44::ID
  };

  Pool<Struct> Struct::pool;
  Vector<Object*> Struct::overlappingObjs;

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

          foreach( obj, overlappingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

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

          foreach( obj, overlappingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

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

  Struct::~Struct()
  {
    delete[] entities;
  }

  Struct::Struct( int index_, int bspId, const Point3& p_, Heading heading_ ) :
      p( p_ ), index( index_ ), id( bspId ), bsp( orbis.bsps[bspId] ),
      heading( heading_ ), life( bsp->life ), resistance( bsp->resistance )
  {
    transf = rotations[heading];
    transf.w = p;

    invTransf = rotations[4 - heading];
    invTransf.translate( Point3::ORIGIN - p );

    Bounds bb = toAbsoluteCS( *bsp );
    mins = bb.mins;
    maxs = bb.maxs;

    nEntities = bsp->nModels;
    entities = nEntities == 0 ? null : new Entity[nEntities];

    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      entity.model  = &bsp->models[i];
      entity.str    = this;
      entity.offset = Vec3::ZERO;
      entity.state  = Entity::CLOSED;
      entity.ratio  = 0.0f;
      entity.time   = 0.0f;
    }
  }

  Struct::Struct( int index_, int bspId, InputStream* istream ) :
      index( index_ ), id( bspId ), bsp( orbis.bsps[bspId] ), resistance( bsp->resistance )
  {
    hard_assert( bsp != null );

    nEntities = bsp->nModels;
    entities = nEntities == 0 ? null : new Entity[nEntities];

    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      entity.model = &bsp->models[i];
      entity.str   = this;
    }

    readFull( istream );
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
    collider.touchOverlaps( this->toAABB(), 4.0f * EPSILON );

    synapse.genParts( 100, p, Vec3::ZERO, 10.0f, Vec3( 0.4f, 0.4f, 0.4f ), 0.1f,
                      1.98f, 0.0f, 2.0f );
    synapse.remove( this );
  }

  void Struct::update()
  {
    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      hard_assert( 0.0f <= entity.ratio && entity.ratio <= 1.0f );

      ( entity.*Entity::handlers[entity.model->type] )();
    }
  }

  void Struct::readFull( InputStream* istream )
  {
    mins      = istream->readPoint3();
    maxs      = istream->readPoint3();
    transf    = istream->readMat44();
    invTransf = istream->readMat44();
    p         = istream->readPoint3();
    heading   = Heading( istream->readInt() );
    life      = istream->readFloat();

    for( int i = 0; i < nEntities; ++i ) {
      entities[i].offset = istream->readVec3();
      entities[i].state = Entity::State( istream->readInt() );
      entities[i].ratio = istream->readFloat();
      entities[i].time = istream->readFloat();
    }
  }

  void Struct::writeFull( OutputStream* ostream )
  {
    ostream->writePoint3( mins );
    ostream->writePoint3( maxs );
    ostream->writeMat44( transf );
    ostream->writeMat44( invTransf );
    ostream->writePoint3( p );
    ostream->writeInt( heading );
    ostream->writeFloat( life );

    for( int i = 0; i < nEntities; ++i ) {
      ostream->writeVec3( entities[i].offset );
      ostream->writeInt( entities[i].state );
      ostream->writeFloat( entities[i].ratio );
      ostream->writeFloat( entities[i].time );
    }
  }

}
