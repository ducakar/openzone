/*
 *  VehicleClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "VehicleClass.h"

#include "Vehicle.h"
#include "Translator.h"

namespace oz
{

  ObjectClass *VehicleClass::init( const String &name, const Config *config )
  {
    VehicleClass *clazz = new VehicleClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", "" );

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

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT, "flag.destroyFunc", true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,  "flag.damageFunc",  false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,     "flag.hitFunc",     false );
    OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,  "flag.updateFunc",  false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,     "flag.useFunc",     false );
    OZ_CLASS_SET_FLAG( Object::CLIP_BIT,         "flag.clip",        true  );
    OZ_CLASS_SET_FLAG( Object::HOVER_BIT,        "flag.hover",       false );
    OZ_CLASS_SET_FLAG( Object::BLEND_BIT,        "flag.blend",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",    false );

    clazz->life                 = config->get( "life", 100.0f );
    clazz->damageTreshold       = config->get( "damageTreshold", 100.0f );
    clazz->damageRatio          = config->get( "damageRatio", 1.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageTreshold < 0.0f ) {
      throw Exception( "Invalid object damageTreshold. Should be >= 0." );
    }
    if( clazz->damageRatio < 0.0f ) {
      throw Exception( "Invalid object damageRatio. Should be >= 0." );
    }

    clazz->nDebris              = config->get( "nDebris", 8 );
    clazz->debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    clazz->debrisRejection      = config->get( "debrisRejection", 1.80f );
    clazz->debrisMass           = config->get( "debrisMass", 0.0f );
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.5f );
    clazz->debrisColor.x        = config->get( "debrisColor.r", 0.5f );
    clazz->debrisColor.y        = config->get( "debrisColor.g", 0.5f );
    clazz->debrisColor.z        = config->get( "debrisColor.b", 0.5f );
    clazz->debrisColorSpread    = config->get( "debrisColorSpread", 0.1f );

    clazz->mass                 = config->get( "mass", 100.0f );
    clazz->lift                 = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
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

    clazz->moveMomentum         = config->get( "moveMomentum", 5.0f );

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object *VehicleClass::create( const Vec3 &pos )
  {
    Vehicle *obj = new Vehicle();

    obj->p        = pos;
    obj->dim      = dim;

    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->type     = this;
    obj->life     = life;

    obj->mass     = mass;
    obj->lift     = lift;

    obj->state    = state;
    obj->oldState = state;

    return obj;
  }

  Object *VehicleClass::create( InputStream *istream )
  {
    Vehicle *obj = new Vehicle();

    obj->dim    = dim;
    obj->cell   = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
