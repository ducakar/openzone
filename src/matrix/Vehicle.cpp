/*
 *  Vehicle.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Vehicle.hpp"

#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/VehicleClass.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Lua.hpp"

namespace oz
{

  const float Vehicle::EXIT_EPSILON   = 0.20f;
  const float Vehicle::EXIT_MOMENTUM  = 1.00f;
  const float Vehicle::EJECT_MOMENTUM = 20.0f;

  Pool<Vehicle> Vehicle::pool;

  void ( Vehicle::* Vehicle::handlers[4] )( const Mat44& rotMat ) = {
    &Vehicle::wheeledHandler,
    &Vehicle::trackedHandler,
    &Vehicle::hoverHandler,
    &Vehicle::airHandler
  };

  void Vehicle::wheeledHandler( const Mat44& )
  {}

  void Vehicle::trackedHandler( const Mat44& )
  {}

  void Vehicle::hoverHandler( const Mat44& rotMat )
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

    const Vec3& right = rotMat.x;
    const Vec3& at    = rotMat.y;

    // raycast for hover
    float ratio = clamp( p.z / ( dim.z + clazz->hoverHeight ), 0.0f, 1.0f );
    Vec3  floor = Vec3( 0.0f, 0.0f, 1.0f );

    collider.translate( p, Vec3( 0.0f, 0.0f, -dim.z - clazz->hoverHeight ) );

    if( collider.hit.ratio < ratio ) {
      ratio = collider.hit.ratio;
      floor = collider.hit.normal;
    }

    float ratio_1 = 1.0f - ratio;

    // controls
    Vec3 move = Vec3::ZERO;

    if( actions & Bot::ACTION_FORWARD ) {
      move.x += at.x;
      move.y += at.y;
    }
    if( actions & Bot::ACTION_BACKWARD ) {
      move.x -= at.x;
      move.y -= at.y;
    }
    if( actions & Bot::ACTION_RIGHT ) {
      move.x += right.x;
      move.y += right.y;
    }
    if( actions & Bot::ACTION_LEFT ) {
      move.x -= right.x;
      move.y -= right.y;
    }

    momentum += move * clazz->moveMomentum;

    // hover momentum
    if( ratio_1 != 0.0f ) {
      float groundMomentum = min( velocity * floor, 0.0f );
      float tickRatio = ratio_1*ratio_1 * Timer::TICK_TIME;

      momentum.z += clazz->hoverHeightStiffness * tickRatio;
      momentum.z -= groundMomentum * clazz->hoverMomentumStiffness * min( tickRatio / 4.0f, 1.0f );
    }
  }

  void Vehicle::airHandler( const Mat44& rotMat )
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

    const Vec3& right = rotMat.x;
    const Vec3& at    = rotMat.y;
    const Vec3& up    = rotMat.z;

    // controls
    Vec3 move = Vec3::ZERO;

    if( actions & Bot::ACTION_FORWARD ) {
      move += at;
    }
    if( actions & Bot::ACTION_BACKWARD ) {
      move -= at;
    }
    if( actions & Bot::ACTION_RIGHT ) {
      move += right;
    }
    if( actions & Bot::ACTION_LEFT ) {
      move -= right;
    }
    if( actions & Bot::ACTION_VEH_UP ) {
      move += up;
    }
    if( actions & Bot::ACTION_VEH_DOWN ) {
      move -= up;
    }

    momentum += move * clazz->moveMomentum;
    momentum.z -= Physics::G_MOMENTUM;
    momentum.z *= 1.0f - Physics::AIR_FRICTION;
  }

  void Vehicle::onDestroy()
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

    Mat44 rotMat = Mat44( rot );

    for( int i = 0; i < CREW_MAX; ++i ) {
      if( crew[i] != -1 ) {
        Bot* bot = static_cast<Bot*>( orbis.objects[ crew[i] ] );

        crew[i] = -1;

        if( bot != null ) {
          bot->p = p + rotMat * clazz->crewPos[i];

          Point3 ejectPos = Point3( bot->p.x, bot->p.y, p.z + dim.z + bot->dim.z + EXIT_EPSILON );

          // kill bot if eject path is blocked
          if( collider.overlaps( AABB( ejectPos, bot->dim ) ) ) {
            bot->kill();
            bot->exit();
          }
          else {
            float hsc[2];
            Math::sincos( h, &hsc[0], &hsc[1] );

            bot->p = ejectPos;
            bot->momentum += EJECT_MOMENTUM * ~Vec3( hsc[0], -hsc[1], 0.10f );
            bot->exit();
          }
        }
      }
    }
    Object::onDestroy();
  }

  void Vehicle::onUpdate()
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

    // clean invalid crew references and throw out dead crew
    for( int i = 0; i < CREW_MAX; ++i ) {
      if( crew[i] != -1 ) {
        Bot* bot = static_cast<Bot*>( orbis.objects[ crew[i] ] );

        if( bot == null || bot->parent == -1 ) {
          crew[i] = -1;
        }
        else if( bot->flags & Bot::DEATH_BIT ) {
          crew[i] = -1;
          bot->exit();
        }
      }
    }

    actions = 0;

    Bot* pilot = null;

    if( crew[PILOT] != -1 ) {
      pilot = static_cast<Bot*>( orbis.objects[ crew[PILOT] ] );

      h = pilot->h;
      v = pilot->v;
      rot = Quat::rotZYX( h, 0.0f, v - Math::TAU / 4.0f );
      actions = pilot->actions;
      flags &= ~DISABLED_BIT;
    }

    Mat44 rotMat = Mat44::rotation( rot );

    if( crew[PILOT] != -1 ) {
      ( this->*handlers[clazz->type] )( rotMat );
    }

    // move forwards (predicted movement) to prevent our bullets hitting us in the back when we are
    // moving very fast
    Point3 oldPos = p;
    p += momentum * Timer::TICK_TIME;

    if( clazz->nWeapons != 0 ) {
      if( actions & ~oldActions & Bot::ACTION_VEH_NEXT_WEAPON ) {
        weapon = ( weapon + 1 ) % clazz->nWeapons;
        addEvent( EVENT_NEXT_WEAPON, 1.0f );
      }
      if( actions & Bot::ACTION_ATTACK ) {
        if( shotTime[weapon] == 0.0f ) {
          shotTime[weapon] = clazz->shotInterval[weapon];

          if( nShots[weapon] == 0 ) {
            addEvent( EVENT_SHOT0_EMPTY + weapon*2, 1.0f );
          }
          else {
            nShots[weapon] = max( -1, nShots[weapon] - 1 );

            addEvent( EVENT_SHOT0 + weapon*2, 1.0f );
            lua.call( clazz->onShot[weapon], this, pilot );
          }
        }
      }
    }

    p = oldPos;

    for( int i = 0; i < clazz->nWeapons; ++i ) {
      if( shotTime[i] > 0.0f ) {
        shotTime[i] = max( shotTime[i] - Timer::TICK_TIME, 0.0f );
      }
    }

    for( int i = 0; i < CREW_MAX; ++i ) {
      if( crew[i] != -1 ) {
        Bot* bot = static_cast<Bot*>( orbis.objects[crew[i]] );

        bot->p = p + rotMat * clazz->crewPos[i] + momentum * Timer::TICK_TIME;
        bot->momentum = velocity;
        bot->velocity = velocity;

        if( bot->actions & Bot::ACTION_EXIT ) {
          float hsc[2];
          Math::sincos( h, &hsc[0], &hsc[1] );

          float  handle = !( dim + bot->dim ) + EXIT_EPSILON;
          Point3 exitPos = Point3( p.x - hsc[0] * handle, p.y + hsc[1] * handle, p.z + dim.z );

          if( !collider.overlaps( AABB( exitPos, bot->dim ) ) ) {
            crew[i] = -1;

            bot->p = exitPos;
            bot->exit();
          }
        }
        else if( bot->actions & Bot::ACTION_EJECT ) {
          bot->p = p + rotMat * clazz->crewPos[i];

          Point3 ejectPos = Point3( p.x, p.y, p.z + dim.z + bot->dim.z + EXIT_EPSILON );

          crew[i] = -1;

          // kill bot if eject path is blocked
          if( collider.overlaps( AABB( ejectPos, bot->dim ) ) ) {
            bot->kill();
            bot->exit();
          }
          else {
            float hsc[2];
            Math::sincos( h, &hsc[0], &hsc[1] );

            bot->p = ejectPos;
            bot->momentum += EJECT_MOMENTUM * ~Vec3( hsc[0], -hsc[1], 0.10f );
            bot->exit();
          }
        }
      }
    }

    oldActions = actions;
    oldState   = state;
  }

  void Vehicle::onUse( Bot* user )
  {
    if( crew[PILOT] == -1 ) {
      crew[PILOT] = user->index;

      user->h = h;
      user->v = v;
      user->enter( index );
    }
  }

  Vehicle::Vehicle() : actions( 0 ), oldActions( 0 ), weapon( 0 )
  {
    aSet( crew, -1, CREW_MAX );
  }

  void Vehicle::service()
  {
    const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );
    bool wasServiced = false;

    if( life != clazz->life ) {
      life = clazz->life;
      wasServiced = true;
    }

    for( int i = 0; i < clazz->nWeapons; ++i ) {
      if( nShots[i] != clazz->nShots[i] ) {
        nShots[i] = clazz->nShots[i];
        wasServiced = true;
      }
    }

    if( wasServiced ) {
      addEvent( EVENT_SERVICE, 1.0f );
    }
  }

  void Vehicle::readFull( InputStream* istream )
  {
    Dynamic::readFull( istream );

    h                 = istream->readFloat();
    v                 = istream->readFloat();
    rot               = istream->readQuat();
    state             = istream->readInt();
    oldState          = istream->readInt();
    actions           = istream->readInt();
    oldActions        = istream->readInt();

    weapon            = istream->readInt();

    for( int i = 0; i < WEAPONS_MAX; ++i ) {
      nShots[i]   = istream->readInt();
      shotTime[i] = istream->readFloat();
    }

    for( int i = 0; i < CREW_MAX; ++i ) {
      crew[i] = istream->readInt();
    }
  }

  void Vehicle::writeFull( OutputStream* ostream ) const
  {
    Dynamic::writeFull( ostream );

    ostream->writeFloat( h );
    ostream->writeFloat( v );
    ostream->writeQuat( rot );
    ostream->writeInt( state );
    ostream->writeInt( oldState );
    ostream->writeInt( actions );
    ostream->writeInt( oldActions );

    ostream->writeInt( weapon );

    for( int i = 0; i < WEAPONS_MAX; ++i ) {
      ostream->writeInt( nShots[i] );
      ostream->writeFloat( shotTime[i] );
    }

    for( int i = 0; i < CREW_MAX; ++i ) {
      ostream->writeInt( crew[i] );
    }
  }

  void Vehicle::readUpdate( InputStream* istream )
  {
    Dynamic::readUpdate( istream );

    rot             = istream->readQuat();
    state           = istream->readInt();

    weapon          = istream->readInt();

    for( int i = 0; i < WEAPONS_MAX; ++i ) {
      nShots[i] = istream->readInt();
    }
  }

  void Vehicle::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeQuat( rot );
    ostream->writeInt( state );

    ostream->writeInt( weapon );

    for( int i = 0; i < WEAPONS_MAX; ++i ) {
      ostream->writeInt( nShots[i] );
    }
  }

}
