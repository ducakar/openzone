/*
 *  Vehicle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Vehicle.hpp"

#include "matrix/Collider.hpp"
#include "matrix/VehicleClass.hpp"
#include "matrix/Bot.hpp"

namespace oz
{

  const float Vehicle::EJECT_MOVE     = 4.0f;
  const float Vehicle::EJECT_MOMENTUM = 20.0f;

  Pool<Vehicle> Vehicle::pool;

  void Vehicle::onDestroy()
  {
    for( int i = 0; i < CREW_MAX; ++i ) {
      if( crew[i] != -1 ) {
        Bot* bot = static_cast<Bot*>( orbis.objects[crew[i]] );

        if( bot != null ) {
          bot->exit();
          bot->kill();
        }
      }
    }
    Object::onDestroy();
  }

  void Vehicle::onUpdate()
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( type );

    flags &= ~HOVER_BIT;
    actions = 0;
    if( crew[PILOT] != -1 ) {
      Bot* pilot = static_cast<Bot*>( orbis.objects[crew[PILOT]] );

      if( pilot != null ) {
        rot = Quat::rotZYX( Math::rad( pilot->h ), 0.0f, Math::rad( pilot->v ) );
        actions = pilot->actions;
        flags |= HOVER_BIT;
        flags &= ~DISABLED_BIT;
      }
    }

    Mat33 rotMat = rot.rotMat33();
    Vec3 at      = rotMat * Vec3( 0.0f, 1.0f, 0.0f );
    Vec3 side    = rotMat * Vec3( 1.0f, 0.0f, 0.0f );
    Vec3 up      = rotMat * Vec3( 0.0f, 0.0f, 1.0f );

    // controls
    Vec3 move = Vec3::zero();

    if( actions & Bot::ACTION_FORWARD ) {
      move += at;
    }
    if( actions & Bot::ACTION_BACKWARD ) {
      move -= at;
    }
    if( actions & Bot::ACTION_RIGHT ) {
      move += side;
    }
    if( actions & Bot::ACTION_LEFT ) {
      move -= side;
    }
    if( actions & Bot::ACTION_JUMP ) {
      move += up;
    }
    if( actions & Bot::ACTION_CROUCH ) {
      move -= up;
    }
    if( !move.isZero() ) {
      momentum += move * clazz->moveMomentum;
    }

    for( int i = 0; i < CREW_MAX; ++i ) {
      if( crew[i] != -1 ) {
        Bot* bot = static_cast<Bot*>( orbis.objects[crew[i]] );

        if( bot == null || bot->parent != index ) {
          crew[i] = -1;
        }
        else if( bot->flags & Bot::DEATH_BIT ) {
          crew[i] = -1;
          bot->exit();
        }
        else if( bot->actions & Bot::ACTION_EJECT ) {
          // move up a bit to prevent colliding with the vehicle
          bot->p += up * EJECT_MOVE;

          bot->momentum += ( up + 0.5f * at ) * EJECT_MOMENTUM;
          bot->exit();
        }
        else {
          bot->p = p + rotMat * clazz->crewPos[0] + momentum * Timer::TICK_TIME;
          bot->momentum = velocity;
        }
      }
    }

    oldActions = actions;
    oldState   = state;
  }

  void Vehicle::onUse( Bot* user )
  {
    if( crew[0] == -1 ) {
      crew[0] = user->index;
      user->enter( index );
    }
  }

  Vehicle::Vehicle() : rot( Quat::id() ), actions( 0 ), oldActions( 0 )
  {
    aSet( crew, -1, CREW_MAX );
  }

  void Vehicle::readFull( InputStream* istream )
  {
    Dynamic::readFull( istream );

    rot          = istream->readQuat();
    state        = istream->readInt();
    oldState     = istream->readInt();
    actions      = istream->readInt();
    oldActions   = istream->readInt();

    for( int i = 0; i < CREW_MAX; ++i ) {
      crew[i] = istream->readInt();
    }
  }

  void Vehicle::writeFull( OutputStream* ostream ) const
  {
    Dynamic::writeFull( ostream );

    ostream->writeQuat( rot );
    ostream->writeInt( state );
    ostream->writeInt( oldState );
    ostream->writeInt( actions );
    ostream->writeInt( oldActions );

    for( int i = 0; i < CREW_MAX; ++i ) {
      ostream->writeInt( crew[i] );
    }
  }

  void Vehicle::readUpdate( InputStream* istream )
  {
    Dynamic::readUpdate( istream );

    rot   = istream->readQuat();
    state = istream->readInt();
  }

  void Vehicle::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeQuat( rot );
    ostream->writeInt( state );
  }

}
