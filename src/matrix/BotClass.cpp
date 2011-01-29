/*
 *  BotClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/BotClass.hpp"

#include "matrix/Bot.hpp"
#include "matrix/Names.hpp"
#include "matrix/Synapse.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->state |= stateBit; \
  }

namespace oz
{

  ObjectClass* BotClass::init( const String& name, const Config* config )
  {
    BotClass* clazz = new BotClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", name );

    clazz->dim.x                = config->get( "dim.x", 0.44f );
    clazz->dim.y                = config->get( "dim.y", 0.44f );
    clazz->dim.z                = config->get( "dim.z", 0.99f );

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f )
    {
      throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
    }

    clazz->flags = 0;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.destroyFunc",  true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.damageFunc",   false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.useFunc",      false );
    OZ_CLASS_SET_FLAG( Object::ITEM_BIT,           "flag.item",         false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
    OZ_CLASS_SET_FLAG( Object::DETECT_BIT,         "flag.detect",       true  );
    OZ_CLASS_SET_FLAG( Object::CLIMBER_BIT,        "flag.climber",      true  );
    OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,         "flag.pusher",       true  );
    OZ_CLASS_SET_FLAG( Object::HOVER_BIT,          "flag.hover",        false );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
    OZ_CLASS_SET_FLAG( Object::DELAYED_DRAW_BIT,   "flag.delayedDraw",  false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

    clazz->life                 = 2.0f * config->get( "life", 100.0f );
    clazz->damageThreshold      = config->get( "damageThreshold", 100.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageThreshold < 0.0f ) {
      throw Exception( "Invalid object damageThreshold. Should be >= 0." );
    }

    clazz->nDebris              = config->get( "nDebris", 8 );
    clazz->debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    clazz->debrisRejection      = config->get( "debrisRejection", 1.95f );
    clazz->debrisMass           = config->get( "debrisMass", 0.0f );
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.5f );
    clazz->debrisColour.x       = config->get( "debrisColour.r", 0.5f );
    clazz->debrisColour.y       = config->get( "debrisColour.g", 0.0f );
    clazz->debrisColour.z       = config->get( "debrisColour.b", 0.0f );
    clazz->debrisColourSpread   = config->get( "debrisColourSpread", 0.1f );

    clazz->mass                 = config->get( "mass", 100.0f );
    clazz->lift                 = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    clazz->dimCrouch.x          = clazz->dim.x;
    clazz->dimCrouch.y          = clazz->dim.y;
    clazz->dimCrouch.z          = config->get( "dimCrouch.z", 0.79f );

    if( clazz->dimCrouch.z < 0.0f ) {
      throw Exception( "Invalid bot crouch dimensions. Should be >= 0." );
    }

    clazz->camZ                 = config->get( "camZ", 0.89f );
    clazz->crouchCamZ           = config->get( "crouchCamZ", 0.69f );

    clazz->bobWalkInc           = config->get( "bobWalkInc", 8.00f );
    clazz->bobRunInc            = config->get( "bobRunInc", 16.00f );
    clazz->bobSwimInc           = config->get( "bobSwimInc", 2.00f );
    clazz->bobSwimRunInc        = config->get( "bobSwimRunInc", 4.00f );
    clazz->bobRotation          = config->get( "bobRotation", 0.25f );
    clazz->bobAmplitude         = config->get( "bobAmplitude", 0.02f );
    clazz->bobSwimAmplitude     = config->get( "bobSwimAmplitude", 0.05f );

    clazz->walkMomentum         = config->get( "walkMomentum", 1.5f );
    clazz->runMomentum          = config->get( "runMomentum", 4.0f );
    clazz->crouchMomentum       = config->get( "crouchMomentum", 1.2f );
    clazz->jumpMomentum         = config->get( "jumpMomentum", 5.0f );

    clazz->stepInc              = config->get( "stepInc", 0.25f );
    clazz->stepMax              = config->get( "stepMax", 0.50f );
    clazz->stepRateLimit        = config->get( "stepRateLimit", 0.00f );
    clazz->stepRateCoeff        = config->get( "stepRateCoeff", 500.0f );
    clazz->stepRateSupp         = config->get( "stepRatesupp", 0.50f );

    clazz->airControl           = config->get( "airControl", 0.025f );
    clazz->climbControl         = config->get( "climbControl", 1.50f );
    clazz->waterControl         = config->get( "waterControl", 0.05f );

    clazz->grabDistance         = config->get( "grabDistance", 2.0f );
    clazz->grabMass             = config->get( "grabMass", 50.0f );
    clazz->throwMomentum        = config->get( "throwMomentum", 6.0f );

    clazz->stamina              = config->get( "stamina", 100.0f );
    clazz->staminaGain          = config->get( "staminaGain", 0.05f );
    clazz->staminaWaterDrain    = config->get( "staminaWaterDrain", 0.10f );
    clazz->staminaRunDrain      = config->get( "staminaRunDrain", 0.08f );
    clazz->staminaJumpDrain     = config->get( "staminaJumpDrain", 4.0f );
    clazz->staminaThrowDrain    = config->get( "staminaThrowDrain", 5.0f );

    clazz->state = 0;

    OZ_CLASS_SET_STATE( Bot::STEPPING_BIT,  "state.stepping",  true );
    OZ_CLASS_SET_STATE( Bot::CROUCHING_BIT, "state.crouching", false );
    OZ_CLASS_SET_STATE( Bot::RUNNING_BIT,   "state.running",   true );

    // default inventory
    char buffer[] = "inventoryItem  ";
    for( int i = 0; i < INVENTORY_ITEMS; ++i ) {
      assert( i < 100 );

      buffer[ sizeof buffer - 3 ] = char( '0' + ( i / 10 ) );
      buffer[ sizeof buffer - 2 ] = char( '0' + ( i % 10 ) );

      String itemName = config->get( buffer, "" );
      if( !itemName.isEmpty() ) {
        clazz->inventoryItems.add( itemName );
      }
    }

    clazz->weaponItem           = config->get( "weaponItem", -1 );

    clazz->mindType             = config->get( "mindType", "" );
    clazz->mindFunction         = config->get( "mindFunction", "" );

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object* BotClass::create( int index, const Point3& pos ) const
  {
    Bot* obj = new Bot();

    assert( obj->index == -1 && obj->cell == null && obj->parent == -1 );

    obj->p        = pos;
    obj->r        = !dim;
    obj->dim      = dim;

    obj->h        = 0.0f;
    obj->v        = 0.0f;

    obj->index    = index;
    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->clazz    = this;
    obj->life     = life;

    obj->mass     = mass;
    obj->lift     = lift;

    obj->camZ     = camZ;
    obj->state    = state;
    obj->oldState = state;
    obj->stamina  = stamina;

    obj->name     = names.genName();

    for( int i = 0; i < inventoryItems.length(); ++i ) {
      int index = synapse.addObject( inventoryItems[i], Point3::ORIGIN );
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[index] );

      assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      if( weaponItem == i ) {
        obj->weaponItem = item->index;
      }
      obj->take( item );
    }

    return obj;
  }

  Object* BotClass::create( int index, InputStream* istream ) const
  {
    Bot* obj = new Bot();

    obj->index  = index;
    obj->clazz  = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    obj->camZ = ( obj->state & Bot::CROUCHING_BIT ) ? crouchCamZ : camZ;

    return obj;
  }

}
