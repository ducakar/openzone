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
 * @file matrix/Vehicle.cc
 */

#include <matrix/Vehicle.hh>

#include <common/Timer.hh>
#include <matrix/Physics.hh>
#include <matrix/LuaMatrix.hh>

namespace oz
{

const float Vehicle::ROT_DIFF_LIMIT     = 0.50f;
const float Vehicle::ROT_VEL_DIFF_RATIO = 0.10f;
const float Vehicle::AIR_FRICTION       = 0.02f;
const float Vehicle::EXIT_EPSILON       = 0.20f;
const float Vehicle::EXIT_MOMENTUM      = 1.00f;
const float Vehicle::EJECT_EPSILON      = 0.80f;
const float Vehicle::EJECT_MOMENTUM     = 15.0f;

Pool<Vehicle, 256> Vehicle::pool;

const Vehicle::Handler Vehicle::HANDLERS[] = {
  &Vehicle::staticHandler,
  &Vehicle::wheeledHandler,
  &Vehicle::trackedHandler,
  &Vehicle::mechHandler,
  &Vehicle::hoverHandler,
  &Vehicle::airHandler
};

void Vehicle::exit()
{
  Bot* bot = static_cast<Bot*>( orbis.obj( pilot ) );

  if( bot != nullptr ) {
    float hsc[2];
    Math::sincos( h, &hsc[0], &hsc[1] );

    float handle = !( dim + bot->dim ) + EXIT_EPSILON;
    Point exitPos = Point( p.x - hsc[0] * handle, p.y + hsc[1] * handle, p.z + dim.z );

    if( !collider.overlaps( AABB( exitPos, bot->dim ) ) ) {
      pilot = -1;

      bot->p = exitPos;
      bot->exit();
    }
  }
}

void Vehicle::eject()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  Bot* bot = static_cast<Bot*>( orbis.obj( pilot ) );

  if( bot != nullptr ) {
    bot->p    = p + rot * clazz->pilotPos;
    bot->p.z += bot->dim.z + dim.z + EJECT_EPSILON;

    // kill bot if eject path is blocked
    if( collider.overlaps( *bot, this ) ) {
      bot->exit();
      bot->kill();
    }
    else {
      float hsc[2];
      Math::sincos( h, &hsc[0], &hsc[1] );

      bot->momentum += EJECT_MOMENTUM * ~Vec3( hsc[0], -hsc[1], 0.10f );
      bot->exit();
    }
  }
}

void Vehicle::service()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  life = clazz->life;
  fuel = clazz->fuel;

  for( int i = 0; i < clazz->nWeapons; ++i ) {
    if( nRounds[i] != clazz->nWeaponRounds[i] ) {
      nRounds[i] = clazz->nWeaponRounds[i];
    }
  }
}

void Vehicle::staticHandler()
{}

void Vehicle::wheeledHandler()
{
  // TODO Wheeled vehicle handler.
}

void Vehicle::trackedHandler()
{
  // TODO Tracked vehicle handler.
}

void Vehicle::mechHandler()
{
  // TODO Mech warrior handler.
}

void Vehicle::hoverHandler()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( h, &hvsc[0], &hvsc[1] );
  Math::sincos( v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

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
    move.x -= hvsc[0];
    move.y += hvsc[1];
  }
  if( actions & Bot::ACTION_BACKWARD ) {
    move.x += hvsc[0];
    move.y -= hvsc[1];
  }
  if( actions & Bot::ACTION_RIGHT ) {
    move.x += hvsc[1];
    move.y += hvsc[0];
  }
  if( actions & Bot::ACTION_LEFT ) {
    move.x -= hvsc[1];
    move.y -= hvsc[0];
  }

  momentum += move * clazz->moveMomentum;
  momentum.x *= 1.0f - AIR_FRICTION;
  momentum.y *= 1.0f - AIR_FRICTION;

  // hover momentum
  if( ratio_1 != 0.0f ) {
    float groundMomentum = min<float>( velocity * floor, 0.0f );
    float tickRatio = ratio_1*ratio_1 * Timer::TICK_TIME;

    momentum.z += clazz->hoverHeightStiffness * tickRatio;
    momentum.z -= groundMomentum * clazz->hoverMomentumStiffness * min( tickRatio / 4.0f, 1.0f );
  }
}

void Vehicle::airHandler()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  Vec3 right = rot.x.vec3();
  Vec3 at    = -rot.z.vec3();
  Vec3 up    = rot.y.vec3();

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

  momentum   += move * clazz->moveMomentum;
  momentum.z -= physics.gravity * Timer::TICK_TIME;
  momentum   *= 1.0f - AIR_FRICTION;
}

void Vehicle::onDestroy()
{
  Bot* bot = static_cast<Bot*>( orbis.obj( pilot ) );

  if( bot != nullptr ) {
    if( state & AUTO_EJECT_BIT ) {
      eject();
    }
    else {
      bot->destroy();
    }
  }

  pilot = -1;

  Dynamic::onDestroy();
}

bool Vehicle::onUse( Bot* user )
{
  if( pilot < 0 ) {
    pilot = user->index;

    user->h = h;
    user->v = v;
    user->enter( index );

    return true;
  }
  return false;
}

void Vehicle::onUpdate()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  // clean invalid pilot reference and throw him out if dead
  Bot* bot = static_cast<Bot*>( orbis.obj( pilot ) );

  if( bot == nullptr || bot->parent < 0 ) {
    pilot = -1;
    bot   = nullptr;
  }
  else if( bot->state & Bot::DEAD_BIT ) {
    pilot = -1;
    bot->exit();
    bot = nullptr;
  }

  actions = 0;

  if( bot != nullptr ) {
    // TODO Limit rotational velocity.
//     float diffH = bot->h - h;
//     float diffV = bot->v - v;
//
//     if( diffH < -Math::TAU / 2.0f ) {
//       diffH += Math::TAU;
//     }
//     else if( diffH > Math::TAU / 2.0f ) {
//       diffH -= Math::TAU;
//     }
//
//     if( diffV < -Math::TAU / 2.0f ) {
//       diffV += Math::TAU;
//     }
//     else if( diffV > Math::TAU / 2.0f ) {
//       diffV -= Math::TAU;
//     }
//
//     float diffL = Math::sqrt( diffH*diffH + diffV*diffV );
//     if( diffL > ROT_DIFF_LIMIT ) {
//       float k = ROT_DIFF_LIMIT / diffL;
//
//       diffH *= k;
//       diffV *= k;
//     }
//
//     rotVelH = Math::mix( rotVelH, diffH, ROT_VEL_DIFF_RATIO );
//     rotVelV = Math::mix( rotVelV, diffV, ROT_VEL_DIFF_RATIO );
//
//     float rotVelL = Math::sqrt( rotVelH*rotVelH + rotVelV*rotVelV );
//     if( diffL > clazz->rotVelLimit ) {
//       float k = clazz->rotVelLimit / rotVelL;
//
//       rotVelH *= k;
//       rotVelV *= k;
//     }
//
//     h = Math::fmod( h + rotVelH + 2.0f*Math::TAU, Math::TAU );
//     v = clamp( v + rotVelV, 0.0f, Math::TAU / 2.0f );
//
//     bot->h = h;
//     bot->v = v;

    h = bot->h;
    v = bot->v;

    actions = bot->actions;
    flags  &= ~DISABLED_BIT;
  }

  rot = Mat44::rotationZXZ( h, v, w );

  if( pilot >= 0 && fuel > 0.0f ) {
    fuel -= clazz->fuelConsumption;

    ( this->*HANDLERS[clazz->type] )();
  }

  // move forwards (predicted movement) to prevent our bullets hitting us in the back when we are
  // moving very fast
  Point oldPos = p;
  p += momentum * Timer::TICK_TIME;

  if( clazz->nWeapons != 0 ) {
    if( actions & ~oldActions & Bot::ACTION_VEH_NEXT_WEAPON ) {
      weapon = ( weapon + 1 ) % clazz->nWeapons;
    }
    if( actions & Bot::ACTION_ATTACK ) {
      if( shotTime[weapon] == 0.0f ) {
        shotTime[weapon] = clazz->weaponShotIntervals[weapon];

        if( nRounds[weapon] == 0 ) {
          addEvent( EVENT_SHOT_EMPTY, 1.0f );
        }
        else {
          nRounds[weapon] = max( -1, nRounds[weapon] - 1 );

          addEvent( EVENT_SHOT0 + weapon, 1.0f );
          luaMatrix.objectCall( clazz->onWeaponShot[weapon], this, bot );
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

  if( bot != nullptr ) {
    bot->p = p + rot * clazz->pilotPos + momentum * Timer::TICK_TIME;
    bot->momentum = velocity;
    bot->velocity = velocity;

    if( bot->actions & Bot::ACTION_EXIT ) {
      exit();
    }
    else if( bot->actions & Bot::ACTION_EJECT ) {
      if( state & HAS_EJECT_BIT ) {
        eject();
      }
    }
  }

  oldActions = actions;
  oldState   = state;
}

float Vehicle::getStatus() const
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  return max( fuel / clazz->fuel, 0.0f );
}

Vehicle::Vehicle( const VehicleClass* clazz_, int index_, const Point& p_, Heading heading ) :
  Dynamic( clazz_, index_, p_, heading )
{
  h          = 0.0f;
  v          = Math::TAU / 4.0f;
  w          = 0.0f;
  rotVelH    = 0.0f;
  rotVelV    = 0.0f;
  actions    = 0;
  oldActions = 0;

  rot        = Mat44::rotationZXZ( h, v, w );
  state      = clazz_->state;
  oldState   = clazz_->state;
  fuel       = clazz_->fuel;

  pilot      = -1;

  weapon     = 0;
  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    nRounds[i]  = clazz_->nWeaponRounds[i];
    shotTime[i] = 0.0f;
  }
}

Vehicle::Vehicle( const VehicleClass* clazz_, InputStream* istream ) :
  Dynamic( clazz_, istream )
{
  h          = istream->readFloat();
  v          = istream->readFloat();
  w          = istream->readFloat();
  rotVelH    = istream->readFloat();
  rotVelV    = istream->readFloat();
  actions    = istream->readInt();
  oldActions = istream->readInt();

  rot        = Mat44::rotationZXZ( h, v, w );
  state      = istream->readInt();
  oldState   = istream->readInt();
  fuel       = istream->readFloat();

  pilot      = istream->readInt();

  weapon     = istream->readInt();
  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    nRounds[i]  = istream->readInt();
    shotTime[i] = istream->readFloat();
  }
}

Vehicle::Vehicle( const VehicleClass* clazz_, const JSON& json ) :
  Dynamic( clazz_, json )
{
  h          = json["h"].asFloat();
  v          = json["v"].asFloat();
  w          = json["w"].asFloat();
  rotVelH    = json["rotVelH"].asFloat();
  rotVelV    = json["rotVelV"].asFloat();
  actions    = json["actions"].asInt();
  oldActions = json["oldActions"].asInt();

  rot        = Mat44::rotationZXZ( h, v, w );
  state      = json["state"].asInt();
  oldState   = json["oldState"].asInt();
  fuel       = json["fuel"].asFloat();

  pilot      = json["pilot"].asInt();

  weapon     = json["weapon"].asInt();

  const JSON& weaponsJSON = json["weapons"];

  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    const JSON& weaponJSON = weaponsJSON[i];

    nRounds[i]  = weaponJSON["nRounds"].asInt();
    shotTime[i] = weaponJSON["shotTime"].asFloat();
  }
}

void Vehicle::write( OutputStream* ostream ) const
{
  Dynamic::write( ostream );

  ostream->writeFloat( h );
  ostream->writeFloat( v );
  ostream->writeFloat( w );
  ostream->writeFloat( rotVelH );
  ostream->writeFloat( rotVelV );
  ostream->writeInt( actions );
  ostream->writeInt( oldActions );

  ostream->writeInt( state );
  ostream->writeInt( oldState );
  ostream->writeFloat( fuel );

  ostream->writeInt( pilot );

  ostream->writeInt( weapon );
  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    ostream->writeInt( nRounds[i] );
    ostream->writeFloat( shotTime[i] );
  }
}

JSON Vehicle::write() const
{
  JSON json = Dynamic::write();

  json.add( "h", h );
  json.add( "v", v );
  json.add( "w", w );
  json.add( "rotVelH", rotVelH );
  json.add( "rotVelV", rotVelV );
  json.add( "actions", actions );
  json.add( "oldActions", oldActions );

  json.add( "state", state );
  json.add( "oldState", oldState );
  json.add( "fuel", fuel );

  json.add( "pilot", orbis.objIndex( pilot ) );

  json.add( "weapon", orbis.objIndex( weapon ) );

  JSON& weaponsJSON = json.add( JSON::ARRAY );

  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    JSON& weaponJSON = weaponsJSON.add( JSON::OBJECT );

    weaponJSON.add( "nRounds", nRounds[i] );
    weaponJSON.add( "shotTime", shotTime[i] );
  }

  return json;
}

void Vehicle::readUpdate( InputStream* )
{}

void Vehicle::writeUpdate( OutputStream* ) const
{}

}
