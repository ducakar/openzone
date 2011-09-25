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
#include "matrix/Library.hpp"
#include "matrix/Vehicle.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->state |= stateBit; \
  }

namespace oz
{

  ObjectClass* VehicleClass::init( const Config* config )
  {
    VehicleClass* clazz = new VehicleClass();

    clazz->flags = Object::DYNAMIC_BIT | Object::HIT_FUNC_BIT |
        Object::USE_FUNC_BIT | Object::UPDATE_FUNC_BIT | Object::VEHICLE_BIT;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.destroyFunc",  true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.damageFunc",   false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.hitFunc",      false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.useFunc",      false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
    OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",     true  );
    OZ_CLASS_SET_FLAG( Object::FRICTLESS_BIT,      "flag.frictless",    false );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

    clazz->fillCommonConfig( config );

    clazz->mass = config->get( "mass", 100.0f );
    clazz->lift = config->get( "lift", 13.0f );

    if( clazz->mass < 0.01f ) {
      throw Exception( "Invalid object mass. Should be >= 0.01." );
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

    char weaponNameBuffer[]   = "weapon  .name";
    char onShotBuffer[]       = "weapon  .onShot";
    char nShotsBuffer[]       = "weapon  .nShots";
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

      clazz->weaponNames[i]  = gettext( config->get( weaponNameBuffer, "" ) );
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

    clazz->pilotPos = Vec3( config->get( "pilot.pos.x", 0.0f ),
                            config->get( "pilot.pos.y", 0.0f ),
                            config->get( "pilot.pos.z", 0.0f ) );
    clazz->pilotRot = Quat::rotZYX( config->get( "pilot.rot.z", 0.0f ),
                                    0.0f,
                                    config->get( "pilot.rot.x", 0.0f ) );

    clazz->moveMomentum           = config->get( "moveMomentum", 2.0f );

    clazz->hoverHeight            = config->get( "hoverHeight", 2.0f );
    clazz->hoverHeightStiffness   = config->get( "hoverHeightStiffness", 40.0f );
    clazz->hoverMomentumStiffness = config->get( "hoverMomentumStiffness", 160.0f );

    return clazz;
  }

  Object* VehicleClass::create( int index, const Point3& pos ) const
  {
    Vehicle* obj = new Vehicle();

    obj->p        = pos;
    obj->index    = index;

    obj->mass     = mass;
    obj->lift     = lift;

    obj->h        = 0.0f;
    obj->v        = Math::TAU / 4.0f;
    obj->rot      = Quat::ID;
    obj->state    = state;
    obj->oldState = state;

    obj->weapon   = 0;

    for( int i = 0; i < Vehicle::WEAPONS_MAX; ++i ) {
      obj->nShots[i]   = nShots[i];
      obj->shotTime[i] = 0.0f;
    }

    fillCommonFields( obj );

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
