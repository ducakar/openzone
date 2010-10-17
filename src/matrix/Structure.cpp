/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Structure.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{

  const float Structure::DAMAGE_THRESHOLD = 400.0f;

  Vector<Object*> Structure::Entity::overlapingObjs;

  Pool<Structure, 0, 256> Structure::pool;

  const Mat33 Structure::rotations[] =
  {
    Mat33::ID,
    Mat33::rotZ(  Math::PI_2 ),
    Mat33::rotZ(  Math::PI ),
    Mat33::rotZ( -Math::PI_2 )
  };

  const Mat33 Structure::invRotations[] =
  {
    Mat33::ID,
    Mat33::rotZ( -Math::PI_2 ),
    Mat33::rotZ(  Math::PI ),
    Mat33::rotZ(  Math::PI_2 )
  };



  void Structure::Entity::updateIgnoring( const BSP::Model* model )
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
      case OPENING:
      case OPENING_BLOCKED: {
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
      case CLOSING:
      case CLOSING_BLOCKED: {
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
      default: {
        assert( false );
        break;
      }
    }
  }

  void Structure::Entity::updateBlocking( const Structure* str, int iEntity,
                                          const BSP::Model* model )
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
      case OPENING:
      case OPENING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::min( ratio + model->ratioInc, 1.0f );
        offset = ratio * model->move;

        if( collider.overlapsOO( str, iEntity ) ) {
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
          collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * model->move;

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

        if( time >= model->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::max( ratio - model->ratioInc, 0.0f );
        offset = ratio * model->move;

        if( collider.overlapsOO( str, iEntity ) ) {
          ratio = oldRatio;
          offset = ratio * model->move;
        }
        else {
          if( ratio == 0.0f ) {
            state = CLOSED;
          }

          offset = oldOffset;

          overlapingObjs.clear();
          collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * model->move;

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

  void Structure::Entity::updatePushing( const Structure* str, int iEntity,
                                         const BSP::Model* model )
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
      case OPENING:
      case OPENING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::min( ratio + model->ratioInc, 1.0f );
        offset = ratio * model->move;

        overlapingObjs.clear();
        collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 momentum = ( model->ratioInc * model->move ) / Timer::TICK_TIME;
          momentum = str->toAbsoluteCS( momentum ) * ( 1.0f + 2.0f * EPSILON );

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
          collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * model->move;

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

        if( time >= model->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        Vec3 oldOffset = offset;
        float oldRatio = ratio;

        ratio = Math::max( ratio - model->ratioInc, 0.0f );
        offset = ratio * model->move;

        overlapingObjs.clear();
        collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 momentum = ( model->ratioInc * model->move ) / Timer::TICK_TIME;
          momentum = str->toAbsoluteCS( momentum ) * ( 1.0f + 2.0f * EPSILON );

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
          collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

          offset = ratio * model->move;

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

  void Structure::Entity::updateCrushing( const Structure* str, int iEntity,
                                          const BSP::Model* model )
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
      case OPENING:
      case OPENING_BLOCKED: {
        ratio = Math::min( ratio + model->ratioInc, 1.0f );
        offset = ratio * model->move;

        overlapingObjs.clear();
        collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 move = ( model->ratioInc + 2.0f * EPSILON ) * model->move;
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

        if( time >= model->timeout ) {
          time = 0.0f;
          state = CLOSING;
        }
        break;
      }
      case CLOSING:
      case CLOSING_BLOCKED: {
        ratio = Math::max( ratio - model->ratioInc, 0.0f );
        offset = ratio * model->move;

        overlapingObjs.clear();
        collider.getOverlaps( str, iEntity, &overlapingObjs, 2.0f * EPSILON );

        if( !overlapingObjs.isEmpty() ) {
          Vec3 move = ( model->ratioInc + 2.0f * EPSILON ) * model->move;
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

  Structure::Structure( int index_, int bsp_, const Vec3& p_, Rotation rot_ ) :
      index( index_ ), iBsp( bsp_ ), p( p_ ), rot( rot_ ), life( orbis.bsps[iBsp]->life ),
      entities( new Entity[orbis.bsps[iBsp]->nModels] ), nEntities( orbis.bsps[iBsp]->nModels )
  {
    for( int i = 0; i < nEntities; ++i ) {
      Entity& entity = entities[i];

      entity.offset = Vec3::ZERO;
      entity.state  = Entity::CLOSED;
      entity.ratio  = 0.0f;
      entity.time   = 0.0f;
    }
  }

  Structure::Structure( int index_, int bsp_, InputStream* istream ) :
      index( index_ ), iBsp( bsp_ ), entities( new Entity[orbis.bsps[iBsp]->nModels] ),
      nEntities( orbis.bsps[iBsp]->nModels )
  {
    readFull( istream );
  }

  Structure::~Structure()
  {
    delete[] entities;
  }

  Bounds Structure::rotate( const Bounds& in, Rotation rot )
  {
    Vec3 p = ( in.maxs - in.mins ) * 0.5f;

    switch( rot ) {
      case Structure::R0: {
        return Bounds( p + in.mins,
                       p + in.maxs );
      }
      case Structure::R90: {
        return Bounds( p + Vec3( -in.maxs.y, +in.mins.x, +in.mins.z ),
                       p + Vec3( -in.mins.y, +in.maxs.x, +in.maxs.z ) );
      }
      case Structure::R180: {
        return Bounds( p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z ),
                       p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z ) );
      }
      case Structure::R270: {
        return Bounds( p + Vec3( +in.mins.y, -in.maxs.x, +in.mins.z ),
                       p + Vec3( +in.maxs.y, -in.mins.x, +in.maxs.z ) );
      }
      default: {
        assert( false );
        break;
      }
    }
  }

  void Structure::setRotation( const Bounds& in, Rotation rot )
  {
    switch( rot ) {
      case Structure::R0: {
        mins = p + in.mins;
        maxs = p + in.maxs;
        break;
      }
      case Structure::R90: {
        mins = p + Vec3( -in.maxs.y, in.mins.x, in.mins.z );
        maxs = p + Vec3( -in.mins.y, in.maxs.x, in.maxs.z );
        break;
      }
      case Structure::R180: {
        mins = p + Vec3( -in.maxs.x, -in.maxs.y, +in.mins.z );
        maxs = p + Vec3( -in.mins.x, -in.mins.y, +in.maxs.z );
        break;
      }
      case Structure::R270: {
        mins = p + Vec3( in.mins.y, -in.maxs.x, in.mins.z );
        maxs = p + Vec3( in.maxs.y, -in.mins.x, in.maxs.z );
        break;
      }
      default: {
        assert( false );
        break;
      }
    }
  }

  void Structure::destroy()
  {
    synapse.genParts( 100, p, Vec3::ZERO, 10.0f, Vec3( 0.4f, 0.4f, 0.4f ), 0.1f,
                      1.98f, 0.0f, 2.0f );
    synapse.remove( this );
  }

  void Structure::update()
  {
    for( int i = 1; i < nEntities; ++i ) {
      BSP::Model& model = orbis.bsps[iBsp]->models[i];
      Entity& entity = entities[i];

      assert( 0.0f <= entity.ratio && entity.ratio <= 1.0f );

      switch( model.type ) {
        case BSP::Model::IGNORING: {
          entity.updateIgnoring( &model );
          break;
        }
        case BSP::Model::BLOCKING: {
          entity.updateBlocking( this, i, &model );
          break;
        }
        case BSP::Model::PUSHING: {
          entity.updatePushing( this, i, &model );
          break;
        }
        default: {
          assert( false );
          break;
        }
      }
    }
  }

  void Structure::readFull( InputStream* istream )
  {
    p    = istream->readVec3();
    rot  = Rotation( istream->readChar() );
    life = istream->readFloat();

    for( int i = 0; i < nEntities; ++i ) {
      entities[i].offset = istream->readVec3();
      entities[i].state = Entity::State( istream->readChar() );
      entities[i].ratio = istream->readFloat();
      entities[i].time = istream->readFloat();
    }
  }

  void Structure::writeFull( OutputStream* ostream )
  {
    ostream->writeVec3( p );
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
