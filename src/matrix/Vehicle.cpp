/*
 *  Vehicle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Vehicle.h"

#include "matrix/Collider.h"
#include "VehicleClass.h"
#include "Bot.h"

namespace oz
{

  const float Vehicle::EJECT_MOVE     = 4.0f;
  const float Vehicle::EJECT_MOMENTUM = 20.0f;

  void Vehicle::onDestroy()
  {
    for( int i = 0; i < CREW_MAX; i++ ) {
      if( crew[i] >= 0 ) {
        Bot *bot = static_cast<Bot*>( world.objects[crew[i]] );

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
    VehicleClass &clazz = *static_cast<VehicleClass*>( type );

    flags &= ~HOVER_BIT;
    actions = 0;
    if( crew[PILOT] >= 0 ) {
      Bot *pilot = static_cast<Bot*>( world.objects[crew[PILOT]] );

      if( pilot != null ) {
        rot = Quat::rotZYX( Math::rad( pilot->h ), 0.0f, Math::rad( pilot->v ) );
        actions = pilot->actions;
        flags |= HOVER_BIT;
        flags &= ~DISABLED_BIT;
      }
    }

    Vec3 at   = rot.rotate( Vec3( 0.0f, 1.0f, 0.0f ) );
    Vec3 side = rot.rotate( Vec3( 1.0f, 0.0f, 0.0f ) );
    Vec3 up   = rot.rotate( Vec3( 0.0f, 0.0f, 1.0f ) );

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
      momentum += move * clazz.moveMomentum;
    }

    for( int i = 0; i < CREW_MAX; i++ ) {
      if( crew[i] >= 0 ) {
        Bot *bot = static_cast<Bot*>( world.objects[crew[i]] );

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

          if( collider.test( *bot, this ) ) {
            bot->momentum += ( up + 0.5f * at ) * EJECT_MOMENTUM;
            bot->exit();
          }
          else {
            bot->p = p + rot.rotate( clazz.crewPos[0] ) + momentum * timer.TICK_TIME;
            bot->momentum = velocity;
          }
        }
        else {
          bot->p = p + rot.rotate( clazz.crewPos[0] ) + momentum * timer.TICK_TIME;
          bot->momentum = velocity;
        }
      }
    }

    oldActions = actions;
    oldState   = state;
  }

  void Vehicle::onUse( Bot *user )
  {
    if( crew[0] < 0 ) {
      crew[0] = user->index;
      user->enter( index );
    }
  }

  Vehicle::Vehicle() : rot( Quat::id() ), actions( 0 ), oldActions( 0 )
  {
    aSet( crew, -1, CREW_MAX );
  }

  void Vehicle::readFull( InputStream *istream )
  {
    DynObject::readFull( istream );

    rot          = istream->readQuat();
    state        = istream->readInt();
    oldState     = istream->readInt();
    actions      = istream->readInt();
    oldActions   = istream->readInt();

    for( int i = 0; i < CREW_MAX; i++ ) {
      crew[i] = istream->readInt();
    }
  }

  void Vehicle::writeFull( OutputStream *ostream ) const
  {
    DynObject::writeFull( ostream );

    ostream->writeQuat( rot );
    ostream->writeInt( state );
    ostream->writeInt( oldState );
    ostream->writeInt( actions );
    ostream->writeInt( oldActions );

    for( int i = 0; i < CREW_MAX; i++ ) {
      ostream->writeInt( crew[i] );
    }
  }

  void Vehicle::readUpdate( InputStream *istream )
  {
    Object::readUpdate( istream );

    rot          = istream->readQuat();
    state        = istream->readInt();
  }

  void Vehicle::writeUpdate( OutputStream *ostream ) const
  {
    DynObject::writeUpdate( ostream );

    ostream->writeQuat( rot );
    ostream->writeInt( state );
  }

}
