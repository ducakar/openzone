/*
 *  VehicleClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/VehicleClass.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Translator.hpp"
#include "matrix/Vehicle.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->state |= stateBit; \
  }

namespace oz
{

  ObjectClass* VehicleClass::init( const String& name, const Config* config )
  {
    VehicleClass* clazz = new VehicleClass();

    clazz->name                 = name;

    clazz->dim.x                = config->get( "dim.x", 0.50f );
    clazz->dim.y                = config->get( "dim.y", 0.50f );
    clazz->dim.z                = config->get( "dim.z", 0.50f );

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f )
    {
      throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
    }

    clazz->flags = 0;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.destroyFunc",  true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.damageFunc",   false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.hitFunc",      false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.useFunc",      false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
    OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",     true  );
    OZ_CLASS_SET_FLAG( Object::FRICTLESS_BIT,      "flag.frictless",    false );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

    clazz->life                 = config->get( "life", 100.0f );
    clazz->damageThreshold      = config->get( "damageThreshold", 100.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageThreshold < 0.0f ) {
      throw Exception( "Invalid object damageThreshold. Should be >= 0." );
    }

    clazz->nDebris              = config->get( "nDebris", 8 );
    clazz->debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    clazz->debrisRejection      = config->get( "debrisRejection", 1.80f );
    clazz->debrisMass           = config->get( "debrisMass", 0.0f );
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.5f );
    clazz->debrisColour.x       = config->get( "debrisColour.r", 0.5f );
    clazz->debrisColour.y       = config->get( "debrisColour.g", 0.5f );
    clazz->debrisColour.z       = config->get( "debrisColour.b", 0.5f );
    clazz->debrisColourSpread   = config->get( "debrisColourSpread", 0.1f );

    clazz->mass                 = config->get( "mass", 100.0f );
    clazz->lift                 = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    String sType = config->get( "type", "AIR" );
    if( sType.equals( "WHEELED" ) ) {
      clazz->type = Vehicle::WHEELED;
    }
    else if( sType.equals( "TRACKED" ) ) {
      clazz->type = Vehicle::TRACKED;
    }
    else if( sType.equals( "HOVER" ) ) {
      clazz->type = Vehicle::HOVER;
    }
    else if( sType.equals( "AIR" ) ) {
      clazz->type = Vehicle::AIR;
    }
    else {
      throw Exception( "Invalid vehicle type, should be either WHEELED, TRACKED, HOVER or AIR" );
    }

    clazz->state = 0;

    OZ_CLASS_SET_STATE( Vehicle::CREW_VISIBLE_BIT, "state.crewVisible", true );

    clazz->turnLimitH = config->get( "turnLimitH", 300.0f );
    clazz->turnLimitV = config->get( "turnLimitV", 300.0f );

    clazz->turnLimitH = Math::rad( clazz->turnLimitH ) * Timer::TICK_TIME;
    clazz->turnLimitV = Math::rad( clazz->turnLimitV ) * Timer::TICK_TIME;

    clazz->enginePitchBias  = config->get( "enginePitchBias", 1.0f );
    clazz->enginePitchRatio = config->get( "enginePitchRatio", 0.001f );
    clazz->enginePitchLimit = config->get( "enginePitchLimit", 2.00f );

    char weaponNameBuffer[] = "weapon  .name";
    char onShotBuffer[] = "weapon  .onShot";
    char nShotsBuffer[] = "weapon  .nShots";
    char shotIntervalBuffer[] = "weapon  .shotInterval";

    clazz->nWeapons = Vehicle::WEAPONS_MAX;

    for( int i = 0; i < Vehicle::WEAPONS_MAX; ++i ) {
      hard_assert( i < 100 );

      weaponNameBuffer[6] = char( '0' + ( i / 10 ) );
      weaponNameBuffer[7] = char( '0' + ( i % 10 ) );

      onShotBuffer[6] = char( '0' + ( i / 10 ) );
      onShotBuffer[7] = char( '0' + ( i % 10 ) );

      nShotsBuffer[6] = char( '0' + ( i / 10 ) );
      nShotsBuffer[7] = char( '0' + ( i % 10 ) );

      shotIntervalBuffer[6] = char( '0' + ( i / 10 ) );
      shotIntervalBuffer[7] = char( '0' + ( i % 10 ) );

      clazz->weaponNames[i]  = OZ_GETTEXT( config->get( weaponNameBuffer, "" ) );
      clazz->onShot[i]       = config->get( onShotBuffer, "" );
      clazz->nShots[i]       = config->get( nShotsBuffer, -1 );
      clazz->shotInterval[i] = config->get( shotIntervalBuffer, 0.5f );

      if( clazz->weaponNames[i].isEmpty() && clazz->nWeapons > i ) {
        clazz->nWeapons = i;
      }
      if( !String::isEmpty( clazz->onShot[i] ) ) {
        clazz->flags |= Object::LUA_BIT;
      }
    }

    clazz->crewPos[Vehicle::PILOT] = Vec3( config->get( "crew.pilot.pos.x", 0.0f ),
                                           config->get( "crew.pilot.pos.y", 0.0f ),
                                           config->get( "crew.pilot.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PILOT] = Quat::rotZYX( config->get( "crew.pilot.rot.z", 0.0f ),
                                                   0.0f,
                                                   config->get( "crew.pilot.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::GUNNER] = Vec3( config->get( "crew.gunner.pos.x", 0.0f ),
                                            config->get( "crew.gunner.pos.y", 0.0f ),
                                            config->get( "crew.gunner.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::GUNNER] = Quat::rotZYX( config->get( "crew.gunner.rot.z", 0.0f ),
                                                    0.0f,
                                                    config->get( "crew.gunner.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::COMMANDER] = Vec3( config->get( "crew.commander.pos.x", 0.0f ),
                                               config->get( "crew.commander.pos.y", 0.0f ),
                                               config->get( "crew.commander.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::COMMANDER] = Quat::rotZYX( config->get( "crew.commander.rot.z", 0.0f ),
                                                       0.0f,
                                                       config->get( "crew.commander.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER0] = Vec3( config->get( "crew.passenger0.pos.x", 0.0f ),
                                                config->get( "crew.passenger0.pos.y", 0.0f ),
                                                config->get( "crew.passenger0.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER0] = Quat::rotZYX( config->get( "crew.passenger0.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger0.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER1] = Vec3( config->get( "crew.passenger1.pos.x", 0.0f ),
                                                config->get( "crew.passenger1.pos.y", 0.0f ),
                                                config->get( "crew.passenger1.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER1] = Quat::rotZYX( config->get( "crew.passenger1.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger1.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER2] = Vec3( config->get( "crew.passenger2.pos.x", 0.0f ),
                                                config->get( "crew.passenger2.pos.y", 0.0f ),
                                                config->get( "crew.passenger2.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER2] = Quat::rotZYX( config->get( "crew.passenger2.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger2.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER3] = Vec3( config->get( "crew.passenger3.pos.x", 0.0f ),
                                                config->get( "crew.passenger3.pos.y", 0.0f ),
                                                config->get( "crew.passenger3.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER3] = Quat::rotZYX( config->get( "crew.passenger3.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger3.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER4] = Vec3( config->get( "crew.passenger4.pos.x", 0.0f ),
                                                config->get( "crew.passenger4.pos.y", 0.0f ),
                                                config->get( "crew.passenger4.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER4] = Quat::rotZYX( config->get( "crew.passenger4.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger4.rot.x", 0.0f ) );

    clazz->crewPos[Vehicle::PASSENGER5] = Vec3( config->get( "crew.passenger5.pos.x", 0.0f ),
                                                config->get( "crew.passenger5.pos.y", 0.0f ),
                                                config->get( "crew.passenger5.pos.z", 0.0f ) );
    clazz->crewRot[Vehicle::PASSENGER5] = Quat::rotZYX( config->get( "crew.passenger5.rot.z", 0.0f ),
                                                        0.0f,
                                                        config->get( "crew.passenger5.rot.x", 0.0f ) );

    clazz->moveMomentum           = config->get( "moveMomentum", 2.0f );

    clazz->hoverHeight            = config->get( "hoverHeight", 2.0f );
    clazz->hoverHeightStiffness   = config->get( "hoverHeightStiffness", 40.0f );
    clazz->hoverMomentumStiffness = config->get( "hoverMomentumStiffness", 160.0f );
    clazz->hoverJumpFactor        = config->get( "hoverJumpFactor", 4.0f );

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object* VehicleClass::create( int index, const Point3& pos ) const
  {
    Vehicle* obj = new Vehicle();

    obj->p                 = pos;
    obj->dim               = dim;

    obj->index             = index;
    obj->flags             = flags;
    obj->oldFlags          = flags;
    obj->clazz             = this;
    obj->life              = life;

    obj->mass              = mass;
    obj->lift              = lift;

    obj->h                 = 0.0f;
    obj->v                 = Math::TAU / 4.0f;
    obj->rot               = Quat::ID;
    obj->state             = state;
    obj->oldState          = state;

    obj->weapon            = 0;

    for( int i = 0; i < Vehicle::WEAPONS_MAX; ++i ) {
      obj->nShots[i]   = nShots[i];
      obj->shotTime[i] = 0.0f;
    }

    return obj;
  }

  Object* VehicleClass::create( int index, InputStream* istream ) const
  {
    Vehicle* obj = new Vehicle();

    obj->dim    = dim;

    obj->index  = index;
    obj->clazz  = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
