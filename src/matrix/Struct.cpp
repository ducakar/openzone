/*
 *  Struct.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

  Vector<Object*> Entity::overlapingObjs;

  const float Struct::DAMAGE_THRESHOLD = 400.0f;

  Pool<Struct, 0, 256> Struct::pool;

  const Mat44 Struct::rotations[] =
  {
    Mat44::ID,
    Mat44::rotZ(  Math::PI_2 ),
    Mat44::rotZ(  Math::PI ),
    Mat44::rotZ( -Math::PI_2 )
  };

  const Mat44 Struct::invRotations[] =
  {
    Mat44::ID,
    Mat44::rotZ( -Math::PI_2 ),
    Mat44::rotZ(  Math::PI ),
    Mat44::rotZ(  Math::PI_2 )
  };

  void Entity::updateIgnoring()
  {
    switch( state ) {
      case CLOSED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = OPENING;
        }
        break;
      }
      case OPENING:
      case OPENING_BLOCKED: {
        ratio += clazz->ratioInc;
        time += Timer::TICK_TIME;

        if( ratio >= 1.0f ) {
          ratio = 1.0f;
          time = 0.0f;
          state = OPENED;
        }

        offset = ratio * clazz->move;
        break;
      }
      case OPENED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        ratio -= clazz->ratioInc;
        time += Timer::TICK_TIME;

        if( ratio <= 0.0f ) {
          ratio = 0.0f;
          time = 0.0f;
          state = CLOSED;
        }

        offset = ratio * clazz->move;
        break;
      }
      default: {
        assert( false );
        break;
      }
    }
  }

  void Entity::updateBlocking()
  {
    switch( state ) {
      case CLOSED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = OPENING;
        }
        break;
      }
      case OPENING:
      case OPENING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::min( ratio + clazz->ratioInc, 1.0f );
        offset = ratio * clazz->move;

        if( collider.overlapsOO( this ) ) {
          ratio = oldRatio;
          offset = oldOffset;
          break;
        }
        else {
          if( ratio == 1.0f ) {
            state = OPENED;
          }

          offset = oldOffset;

          overlapingObjs.clear();
          collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * clazz->move;

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }
        }
        break;
      }
      case OPENED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::max( ratio - clazz->ratioInc, 0.0f );
        offset = ratio * clazz->move;

        if( collider.overlapsOO( this ) ) {
          ratio = oldRatio;
          offset = ratio * clazz->move;
        }
        else {
          if( ratio == 0.0f ) {
            state = CLOSED;
          }

          offset = oldOffset;

          overlapingObjs.clear();
          collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * clazz->move;

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }
        }
        break;
      }
      default: {
        assert( false );
        break;
      }
    }
  }

  void Entity::updatePushing()
  {
    switch( state ) {
      case CLOSED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = OPENING;
        }
        break;
      }
      case OPENING:
      case OPENING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::min( ratio + clazz->ratioInc, 1.0f );
        offset = ratio * clazz->move;

        overlapingObjs.clear();
        collider.getOverlaps( this, &overlapingObjs, 0.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 momentum = ( clazz->ratioInc * clazz->move ) / Timer::TICK_TIME;
          momentum = str->toAbsoluteCS( momentum ) * ( 1.0f + 2.0f * EPSILON );

          if( momentum.z > 0.0f ) {
            momentum.z -= Physics::G_VELOCITY;
          }

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->momentum += momentum;
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }

          offset = oldOffset;
          ratio = oldRatio;
        }
        else {
          if( ratio == 1.0f ) {
            state = OPENED;
          }

          offset = oldOffset;

          overlapingObjs.clear();
          collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * clazz->move;

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }
        }
        break;
      }
      case OPENED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::max( ratio - clazz->ratioInc, 0.0f );
        offset = ratio * clazz->move;

        overlapingObjs.clear();
        collider.getOverlaps( this, &overlapingObjs, 0.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 momentum = ( clazz->ratioInc * clazz->move ) / Timer::TICK_TIME;
          momentum = str->toAbsoluteCS( momentum ) * ( 1.0f + 2.0f * EPSILON );

          if( momentum.z > 0.0f ) {
            momentum.z -= Physics::G_VELOCITY;
          }

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->momentum += momentum;
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }

          offset = oldOffset;
          ratio = oldRatio;
        }
        else {
          if( ratio == 0.0f ) {
            state = CLOSED;
          }

          offset = oldOffset;

          overlapingObjs.clear();
          collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * clazz->move;

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
            }
          }
        }
        break;
      }
      default: {
        assert( false );
        break;
      }
    }
  }

  void Entity::updateCrushing()
  {
    switch( state ) {
      case CLOSED: {
        time += Timer::TICK_TIME;

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = OPENING;
        }
        break;
      }
      case OPENING:
      case OPENING_BLOCKED: {
        ratio = Math::min( ratio + clazz->ratioInc, 1.0f );
        offset = ratio * clazz->move;

        overlapingObjs.clear();
        collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 move = ( clazz->ratioInc + 2.0f * EPSILON ) * clazz->move;
          move = str->toAbsoluteCS( move );

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              dyn->momentum = move / Timer::TICK_TIME;
              dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
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

        if( time >= clazz->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        ratio = Math::max( ratio - clazz->ratioInc, 0.0f );
        offset = ratio * clazz->move;

        overlapingObjs.clear();
        collider.getOverlaps( this, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 move = ( clazz->ratioInc + 2.0f * EPSILON ) * clazz->move;
          move = str->toAbsoluteCS( move );

          foreach( obj, overlapingObjs.iter() ) {
            Dynamic* dyn = static_cast<Dynamic*>( *obj );

            if( dyn->flags & Object::DYNAMIC_BIT ) {
              collider.translate( *dyn, move, dyn );

              if( collider.hit.ratio == 1.0f ) {
                dyn->p += move;
                dyn->velocity += move / Timer::TICK_TIME;
                dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
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
      default: {
        assert( false );
        break;
      }
    }
  }

  Struct::Struct( int index_, int bsp_, const Point3& p_, Rotation rot_ ) :
      index( index_ ), bsp( bsp_ ), p( p_ ), rot( rot_ ), life( orbis.bsps[bsp]->life )
  {
    const BSP* bsp = orbis.bsps[this->bsp];

    assert( bsp != null );

    entities = new Entity[bsp->nEntityClasses];
    nEntities = bsp->nEntityClasses;

    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      entity.clazz  = &bsp->entityClasses[i];
      entity.str    = this;
      entity.offset = Vec3::ZERO;
      entity.state  = Entity::CLOSED;
      entity.ratio  = 0.0f;
      entity.time   = 0.0f;
    }
  }

  Struct::Struct( int index_, int bsp_, InputStream* istream ) :
      index( index_ ), bsp( bsp_ )
  {
    const BSP* bsp = orbis.bsps[this->bsp];

    assert( bsp != null );

    entities = new Entity[bsp->nEntityClasses];
    nEntities = bsp->nEntityClasses;

    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      entity.clazz  = &bsp->entityClasses[i];
      entity.str    = this;
    }

    readFull( istream );
  }

  Struct::~Struct()
  {
    delete[] entities;
  }

  Bounds Struct::toStructCS( const Bounds& bb ) const
  {
    switch( rot ) {
      default: {
        assert( false );
      }
      case Struct::R0: {
        return Bounds( Point3( +bb.mins.x - p.x, +bb.mins.y - p.y, +bb.mins.z - p.z ),
                       Point3( +bb.maxs.x - p.x, +bb.maxs.y - p.y, +bb.maxs.z - p.z ) );
      }
      case Struct::R90: {
        return Bounds( Point3( +bb.mins.y - p.y, -bb.maxs.x + p.x, +bb.mins.z - p.z ),
                       Point3( +bb.maxs.y - p.y, -bb.mins.x + p.x, +bb.maxs.z - p.z ) );
      }
      case Struct::R180: {
        return Bounds( Point3( -bb.maxs.x + p.x, -bb.maxs.y + p.y, +bb.mins.z - p.z ),
                       Point3( -bb.mins.x + p.x, -bb.mins.y + p.y, +bb.maxs.z - p.z ) );
      }
      case Struct::R270: {
        return Bounds( Point3( -bb.maxs.y + p.y, +bb.mins.x - p.x, +bb.mins.z - p.z ),
                       Point3( -bb.mins.y + p.y, +bb.maxs.x - p.x, +bb.maxs.z - p.z ) );
      }
    }
  }

  Bounds Struct::toAbsoluteCS( const Bounds& bb ) const
  {
    switch( rot ) {
      default: {
        assert( false );
      }
      case Struct::R0: {
        return Bounds( p + Vec3( +bb.mins.x, +bb.mins.y, +bb.mins.z ),
                       p + Vec3( +bb.maxs.x, +bb.maxs.y, +bb.maxs.z ) );
      }
      case Struct::R90: {
        return Bounds( p + Vec3( -bb.maxs.y, +bb.mins.x, +bb.mins.z ),
                       p + Vec3( -bb.mins.y, +bb.maxs.x, +bb.maxs.z ) );
      }
      case Struct::R180: {
        return Bounds( p + Vec3( -bb.maxs.x, -bb.maxs.y, +bb.mins.z ),
                       p + Vec3( -bb.mins.x, -bb.mins.y, +bb.maxs.z ) );
      }
      case Struct::R270: {
        return Bounds( p + Vec3( +bb.mins.y, -bb.maxs.x, +bb.mins.z ),
                       p + Vec3( +bb.maxs.y, -bb.mins.x, +bb.maxs.z ) );
      }
    }
  }

  Bounds Struct::rotate( const Bounds& in, Rotation rot )
  {
    Point3 p = in.mins + ( in.maxs - in.mins ) * 0.5f;

    switch( rot ) {
      default: {
        assert( false );
      }
      case Struct::R0: {
        return Bounds( p + Vec3( +in.mins.x, +in.mins.y, +in.mins.z ),
                       p + Vec3( +in.maxs.x, +in.maxs.y, +in.maxs.z ) );
      }
      case Struct::R90: {
        return Bounds( p + Vec3( -in.maxs.y, +in.mins.x, +in.mins.z ),
                       p + Vec3( -in.mins.y, +in.maxs.x, +in.maxs.z ) );
      }
      case Struct::R180: {
        return Bounds( p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z ),
                       p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z ) );
      }
      case Struct::R270: {
        return Bounds( p + Vec3( +in.mins.y, -in.maxs.x, +in.mins.z ),
                       p + Vec3( +in.maxs.y, -in.mins.x, +in.maxs.z ) );
      }
    }
  }

  void Struct::setRotation( const Bounds& in, Rotation rot )
  {
    switch( rot ) {
      default: {
        assert( false );
      }
      case Struct::R0: {
        mins = p + Vec3( +in.mins.x, +in.mins.y, +in.mins.z );
        maxs = p + Vec3( +in.maxs.x, +in.maxs.y, +in.maxs.z );
        break;
      }
      case Struct::R90: {
        mins = p + Vec3( -in.maxs.y, in.mins.x, in.mins.z );
        maxs = p + Vec3( -in.mins.y, in.maxs.x, in.maxs.z );
        break;
      }
      case Struct::R180: {
        mins = p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z );
        maxs = p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z );
        break;
      }
      case Struct::R270: {
        mins = p + Vec3( in.mins.y, -in.maxs.x, in.mins.z );
        maxs = p + Vec3( in.maxs.y, -in.mins.x, in.maxs.z );
        break;
      }
    }
  }

  void Struct::destroy()
  {
    synapse.genParts( 100, p, Vec3::ZERO, 10.0f, Vec3( 0.4f, 0.4f, 0.4f ), 0.1f,
                      1.98f, 0.0f, 2.0f );
    synapse.remove( this );
  }

  void Struct::update()
  {
    for( int i = 1; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      assert( 0.0f <= entity.ratio && entity.ratio <= 1.0f );

      switch( entity.clazz->mode ) {
        case EntityClass::IGNORING: {
          entity.updateIgnoring();
          break;
        }
        case EntityClass::BLOCKING: {
          entity.updateBlocking();
          break;
        }
        case EntityClass::PUSHING: {
          entity.updatePushing();
          break;
        }
        case EntityClass::CRUSHING: {
          entity.updateCrushing();
          break;
        }
        default: {
          assert( false );
          break;
        }
      }
    }
  }

  void Struct::readFull( InputStream* istream )
  {
    p    = istream->readPoint3();
    rot  = Rotation( istream->readChar() );
    life = istream->readFloat();

    for( int i = 0; i < nEntities; ++i ) {
      entities[i].offset = istream->readVec3();
      entities[i].state = Entity::State( istream->readChar() );
      entities[i].ratio = istream->readFloat();
      entities[i].time = istream->readFloat();
    }
  }

  void Struct::writeFull( OutputStream* ostream )
  {
    ostream->writePoint3( p );
    ostream->writeChar( rot );
    ostream->writeFloat( life );

    for( int i = 0; i < nEntities; ++i ) {
      ostream->writeVec3( entities[i].offset );
      ostream->writeChar( entities[i].state );
      ostream->writeFloat( entities[i].ratio );
      ostream->writeFloat( entities[i].time );
    }
  }

}
