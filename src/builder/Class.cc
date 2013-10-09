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
 * @file builder/Class.cc
 */

#include <builder/Class.hh>

#include <common/Timer.hh>
#include <matrix/Vehicle.hh>
#include <client/Camera.hh>
#include <builder/Context.hh>

#define SET_ATTRIB( attribBit, varName, defValue ) \
  if( config[varName].get( defValue ) ) { \
    attributes |= attribBit; \
  }

#define SET_FLAG( flagBit, varName, defValue ) \
  if( config[varName].get( defValue ) ) { \
    flags |= flagBit; \
  }

#define SET_STATE( stateBit, varName, defValue ) \
  if( config[varName].get( defValue ) ) { \
    state |= stateBit; \
  }

namespace oz
{
namespace builder
{

const Mat44 Class::INJURY_COLOUR = Mat44( 1.00f, 0.00f, 0.00f, 0.00f,
                                          1.00f, 0.00f, 0.00f, 0.00f,
                                          1.00f, 0.00f, 0.00f, 0.00f,
                                          0.20f, 0.05f, 0.05f, 1.00f );

void Class::fillObject( const char* className )
{
  /*
   * name
   */

  name        = className;
  title       = config["title"].get( name );
  description = config["description"].get( "" );

  if( name.isEmpty() ) {
    OZ_ERROR( "Empty class name" );
  }

  /*
   * flags
   */

  flags = 0;

  SET_FLAG( Object::DESTROY_FUNC_BIT, "flag.onDestroy", true  );
  SET_FLAG( Object::USE_FUNC_BIT,     "flag.onUse",     false );
  SET_FLAG( Object::UPDATE_FUNC_BIT,  "flag.onUpdate",  false );
  SET_FLAG( Object::STATUS_FUNC_BIT,  "flag.getStatus", false );
  SET_FLAG( Object::SOLID_BIT,        "flag.solid",     true  );
  SET_FLAG( Object::CYLINDER_BIT,     "flag.cylinder",  true  );
  SET_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",  false );

  /*
   * dim
   */

  dim = config["dim"].get( Vec3( -1.0f, -1.0f, -1.0f ) );

  if( dim.x < 0.0f || dim.x > Object::REAL_MAX_DIM ||
      dim.y < 0.0f || dim.y > Object::REAL_MAX_DIM ||
      dim.z < 0.0f )
  {
    OZ_ERROR( "%s: Invalid dimensions. Should be >= 0 and <= %g.", className, Object::REAL_MAX_DIM );
  }

  if( ( flags & Object::CYLINDER_BIT ) && dim.x != dim.y ) {
    OZ_ERROR( "%s: Cylindrical object must have dim.x == dim.y", className );
  }

  /*
   * life
   */

  life       = config["life"].get( 0.0f );
  resistance = config["resistance"].get( 100.0f );

  if( life <= 0.0f || !Math::isFinite( life ) ) {
    OZ_ERROR( "%s: Invalid life value. Should be > 0 and finite. If you want infinite life rather"
              " set resistance to infinity (\"inf\").", className );
  }
  if( resistance < 0.0f ) {
    OZ_ERROR( "%s: Invalid resistance. Should be >= 0.", className );
  }

  /*
   * attributes
   */

  attributes = 0;

  SET_ATTRIB( ObjectClass::NIGHT_VISION_BIT, "attrib.nightVision", false );
  SET_ATTRIB( ObjectClass::BINOCULARS_BIT,   "attrib.binoculars",  false );
  SET_ATTRIB( ObjectClass::GALILEO_BIT,      "attrib.galileo",     false );
  SET_ATTRIB( ObjectClass::MUSIC_PLAYER_BIT, "attrib.musicPlayer", false );
  SET_ATTRIB( ObjectClass::SUICIDE_BIT,      "attrib.suicide",     false );

  /*
   * key code
   */

  key = config["key"].get( 0 );

  /*
   * cost
   */

  cost = config["cost"].get( 0 );

  /*
   * inventory
   */

  nItems = config["nItems"].get( 0 );
  defaultItems.clear();

  if( nItems != 0 ) {
    flags |= Object::BROWSABLE_BIT;
  }

  if( nItems < 0 ) {
    OZ_ERROR( "%s: Inventory size must be a non-negative integer", className );
  }
  if( ( flags & Object::ITEM_BIT ) && nItems != 0 ) {
    OZ_ERROR( "%s: Item cannot have an inventory", className );
  }

  // default inventory
  if( nItems != 0 ) {
    const JSON& defaultItemsConfig = config["defaultItems"];
    int nDefaultItems = defaultItemsConfig.length();

    for( int i = 0; i < nDefaultItems; ++i ) {
      const char* itemName = defaultItemsConfig[i].get( "" );

      if( String::isEmpty( itemName ) ) {
        OZ_ERROR( "%s: Empty name for default item #%d", className, i );
      }

      defaultItems.add( itemName );
    }

    if( defaultItems.length() > nItems ) {
      OZ_ERROR( "%s: Too many items in the default inventory", className );
    }
  }

  /*
   * debris
   */

  nFrags   = config["nFrags"].get( 5 );
  fragPool = config["fragPool"].get( "" );

  /*
   * device
   */

  deviceType = config["deviceType"].get( "" );

  if( !deviceType.isEmpty() ) {
    if( flags & Object::USE_FUNC_BIT ) {
      OZ_ERROR( "%s: Device cannot have onUse handler", className );
    }

    devices.include( deviceType );
  }

  /*
   * imago
   */

  imagoType  = config["imagoType"].get( "" );

  if( imagoType.isEmpty() ) {
    imagoModel = "";
  }
  else {
    flags |= Object::IMAGO_BIT;

    imagoModel = config["imagoModel"].get( "" );

    context.usedModels.include( imagoModel, name + " (Object class)" );

    imagines.include( imagoType );
  }

  /*
   * audio
   */

  audioType = config["audioType"].get( "" );

  foreach( i, iter( audioSounds ) ) {
    *i = "";
  }

  if( !audioType.isEmpty() ) {
    flags |= Object::AUDIO_BIT;

    const JSON& soundsConfig = config["audioSounds"];

    audioSounds[Object::EVENT_CREATE]     = soundsConfig["create"].get( "" );
    audioSounds[Object::EVENT_DESTROY]    = soundsConfig["destroy"].get( "" );
    audioSounds[Object::EVENT_DAMAGE]     = soundsConfig["damage"].get( "" );
    audioSounds[Object::EVENT_HIT]        = soundsConfig["hit"].get( "" );
    audioSounds[Object::EVENT_USE]        = soundsConfig["use"].get( "" );
    audioSounds[Object::EVENT_USE_FAILED] = soundsConfig["useFailed"].get( "" );

    context.usedSounds.include( audioSounds[Object::EVENT_CREATE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_DESTROY], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_DAMAGE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_HIT], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_USE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_USE_FAILED], name + " (Object class)" );

    audios.include( audioType );
  }

  /*
   * handlers
   */

  onDestroy = config["onDestroy"].get( "" );
  onUse     = config["onUse"].get( "" );
  onUpdate  = config["onUpdate"].get( "" );
  getStatus = config["getStatus"].get( "" );

  if( !onDestroy.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onDestroy"].get( true ) ) {
      flags &= ~Object::DESTROY_FUNC_BIT;
    }
    else {
      flags |= Object::DESTROY_FUNC_BIT;
    }
  }
  if( !onUse.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onUse"].get( true ) ) {
      flags &= ~Object::USE_FUNC_BIT;
    }
    else {
      flags |= Object::USE_FUNC_BIT;
    }
  }
  if( !onUpdate.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onUpdate"].get( true ) ) {
      flags &= ~Object::UPDATE_FUNC_BIT;
    }
    else {
      flags |= Object::UPDATE_FUNC_BIT;
    }
  }
  if( !getStatus.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.getStatus"].get( true ) ) {
      flags &= ~Object::STATUS_FUNC_BIT;
    }
    else {
      flags |= Object::STATUS_FUNC_BIT;
    }
  }
}

void Class::fillDynamic( const char* className )
{
  fillObject( className );

  flags |= Object::DYNAMIC_BIT;

  SET_FLAG( Object::ITEM_BIT, "flag.item", false );

  if( !audioType.isEmpty() ) {
    const JSON& soundsConfig = config["audioSounds"];

    audioSounds[Object::EVENT_LAND]     = soundsConfig["land"].get( "" );
    audioSounds[Object::EVENT_SPLASH]   = soundsConfig["splash"].get( "" );
    audioSounds[Object::EVENT_FRICTING] = soundsConfig["fricting"].get( "" );

    context.usedSounds.include( audioSounds[Object::EVENT_LAND], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_SPLASH], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Object::EVENT_FRICTING], name + " (Object class)" );
  }

  mass = config["mass"].get( 0.0f );
  lift = config["lift"].get( -1.0f );

  if( mass < 1.0f ) {
    OZ_ERROR( "%s: Invalid object mass. Should be >= 1 kg.", className );
  }
  if( lift < 0.0f ) {
    OZ_ERROR( "%s: Invalid object lift. Should be >= 0.", className );
  }
}

void Class::fillWeapon( const char* className )
{
  fillDynamic( className );

  flags |= Object::WEAPON_BIT | Object::ITEM_BIT | Object::USE_FUNC_BIT | Object::UPDATE_FUNC_BIT |
           Object::STATUS_FUNC_BIT;

  if( !audioType.isEmpty() ) {
    const JSON& soundsConfig = config["audioSounds"];

    audioSounds[Weapon::EVENT_SHOT_EMPTY] = soundsConfig["shotEmpty"].get( "" );
    audioSounds[Weapon::EVENT_SHOT]       = soundsConfig["shot"].get( "" );

    context.usedSounds.include( audioSounds[Weapon::EVENT_SHOT_EMPTY], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Weapon::EVENT_SHOT], name + " (Object class)" );
  }

  int dollar = name.index( '$' );
  if( dollar < 0 ) {
    OZ_ERROR( "%s: Weapon name should be of the form botPrefix$weaponName", className );
  }

  userBase     = name.substring( 0, dollar );

  nRounds      = config["nRounds"].get( -1 );
  shotInterval = config["shotInterval"].get( 0.5f );

  onShot       = config["onShot"].get( "" );

  if( !String::isEmpty( onShot ) ) {
    flags |= Object::LUA_BIT;
  }
}

void Class::fillBot( const char* className )
{
  fillDynamic( className );

  flags |= Object::BOT_BIT | Object::CYLINDER_BIT | Object::UPDATE_FUNC_BIT |
           Object::STATUS_FUNC_BIT;
  // We don't allow browsing a bot's inventory as long as one is alive.
  flags &= ~Object::BROWSABLE_BIT;

  life *= 2.0f;

  if( !audioType.isEmpty() ) {
    const JSON& soundsConfig = config["audioSounds"];

    audioSounds[Bot::EVENT_STEP]           = soundsConfig["step"].get( "" );
    audioSounds[Bot::EVENT_WATERSTEP]      = soundsConfig["waterStep"].get( "" );
    audioSounds[Bot::EVENT_SWIM_SURFACE]   = soundsConfig["swimSurface"].get( "" );
    audioSounds[Bot::EVENT_SWIM_SUBMERGED] = soundsConfig["swimSubmerged"].get( "" );

    audioSounds[Bot::EVENT_DEATH]          = soundsConfig["death"].get( "" );
    audioSounds[Bot::EVENT_JUMP]           = soundsConfig["jump"].get( "" );
    audioSounds[Bot::EVENT_MELEE]          = soundsConfig["melee"].get( "" );
    audioSounds[Bot::EVENT_TAKE]           = soundsConfig["take"].get( "" );

    audioSounds[Bot::EVENT_POINT]          = soundsConfig["point"].get( "" );
    audioSounds[Bot::EVENT_FALL_BACK]      = soundsConfig["fallBack"].get( "" );
    audioSounds[Bot::EVENT_SALUTE]         = soundsConfig["salute"].get( "" );
    audioSounds[Bot::EVENT_WAVE]           = soundsConfig["wave"].get( "" );
    audioSounds[Bot::EVENT_FLIP]           = soundsConfig["flip"].get( "" );

    context.usedSounds.include( audioSounds[Bot::EVENT_STEP], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_WATERSTEP], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_SWIM_SURFACE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_SWIM_SUBMERGED], name + " (Object class)" );

    context.usedSounds.include( audioSounds[Bot::EVENT_DEATH], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_JUMP], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_MELEE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_TAKE], name + " (Object class)" );

    context.usedSounds.include( audioSounds[Bot::EVENT_POINT], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_FALL_BACK], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_SALUTE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_WAVE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Bot::EVENT_FLIP], name + " (Object class)" );
  }

  state = 0;

  SET_STATE( Bot::MECHANICAL_BIT, "state.mechanical", false );

  crouchDim.x = dim.x;
  crouchDim.y = dim.y;
  crouchDim.z = config["crouchDimZ"].get( -1.0f );

  if( crouchDim.z < 0.0f ) {
    OZ_ERROR( "%s: Invalid bot crouch dimensions. Should be >= 0.", className );
  }

  corpseDim.x = config["corpseDim.x"].get( 2.0f * dim.x );
  corpseDim.y = config["corpseDim.y"].get( 2.0f * dim.y );
  corpseDim.z = config["corpseDim.z"].get( 0.20f );

  if( corpseDim.x < 0.0f || corpseDim.y < 0.0f || corpseDim.z < 0.0f ) {
    OZ_ERROR( "%s: Invalid bot corpse dimensions. Should be >= 0.", className );
  }

  camZ              = config["camZ"].get( 0.0f );
  crouchCamZ        = config["crouchCamZ"].get( 0.0f );

  walkMomentum      = config["walkMomentum"].get( 1.0f );
  runMomentum       = config["runMomentum"].get( 2.0f );
  jumpMomentum      = config["jumpMomentum"].get( 5.0f );

  airControl        = config["airControl"].get( 0.025f );
  ladderControl     = config["ladderControl"].get( 0.20f );
  waterControl      = config["waterControl"].get( 0.07f );
  slickControl      = config["slickControl"].get( 0.07f );

  stepWalkInc       = config["stepWalkInc"].get( 6.0f / 6.0f ) * Timer::TICK_TIME;
  stepRunInc        = config["stepRunInc"].get( 10.0f / 6.0f ) * Timer::TICK_TIME;

  stairInc          = config["stairInc"].get( 11.0f / 64.0f );
  stairMax          = config["stairMax"].get( 22.0f / 64.0f );
  stairRateLimit    = config["stairRateLimit"].get( 0.15f );
  stairRateSupp     = config["stairRateSupp"].get( 0.80f );

  climbInc          = config["climbInc"].get( 0.25f );
  climbMax          = config["climbMax"].get( 2.25f );
  climbMomentum     = config["climbMomentum"].get( 2.0f );

  stamina           = config["stamina"].get( 100.0f );

  if( stamina <= 0.0f || !Math::isFinite( stamina ) ) {
    OZ_ERROR( "%s: Invalid stamina value. Should be > 0 and finite. If you want infinite stamina"
              " rather set stamina*Drain variables to zero.", className );
  }

  staminaGain       = config["staminaGain"].get( 2.5f ) * Timer::TICK_TIME;
  staminaRunDrain   = config["staminaRunDrain"].get( 4.0f ) * Timer::TICK_TIME;
  staminaWaterDrain = config["staminaWaterDrain"].get( 4.0f ) * Timer::TICK_TIME;
  staminaClimbDrain = config["staminaClimbDrain"].get( 10.0f ) * Timer::TICK_TIME;
  staminaJumpDrain  = config["staminaJumpDrain"].get( 5.0f );
  staminaThrowDrain = config["staminaThrowDrain"].get( 8.0f );

  regeneration      = config["regeneration"].get( 0.0f ) * Timer::TICK_TIME;

  reachDist         = config["reachDist"].get( 2.0f );
  grabWeight        = config["grabWeight"].get( 500.0f );
  throwMomentum     = config["throwMomentum"].get( 6.0f );

  weaponItem        = config["weaponItem"].get( -1 );
  meleeInterval     = config["meleeInterval"].get( 0.5f );
  onMelee           = config["onMelee"].get( "" );

  if( !String::isEmpty( onMelee ) ) {
    flags |= Object::LUA_BIT;
  }

  nameList          = config["nameList"].get( "" );

  mindFunc          = config["mindFunc"].get( "" );

  bobRotation       = Math::rad( config["bobRotation"].get( 0.35f ) );
  bobAmplitude      = config["bobAmplitude"].get( 0.07f );
  bobSwimAmplitude  = config["bobSwimAmplitude"].get( 0.07f );

  const JSON& baseColourConfig   = config["baseColour"];
  const JSON& nvColourConfig     = config["nvColour"];
  const JSON& injuryColourConfig = config["injuryColour"];

  baseColour   = baseColourConfig.get( Mat44::ID );
  nvColour     = nvColourConfig.get( Camera::NV_COLOUR );
  injuryColour = injuryColourConfig.get( INJURY_COLOUR );
}

void Class::fillVehicle( const char* className )
{
  fillDynamic( className );

  flags |= Object::VEHICLE_BIT | Object::CYLINDER_BIT | Object::USE_FUNC_BIT |
           Object::UPDATE_FUNC_BIT | Object::STATUS_FUNC_BIT;

  if( !audioType.isEmpty() ) {
    const JSON& soundsConfig = config["audioSounds"];

    audioSounds[Vehicle::EVENT_ENGINE]     = soundsConfig["engine"].get( "" );
    audioSounds[Vehicle::EVENT_SHOT0]      = soundsConfig["shot0"].get( "" );
    audioSounds[Vehicle::EVENT_SHOT1]      = soundsConfig["shot1"].get( "" );
    audioSounds[Vehicle::EVENT_SHOT2]      = soundsConfig["shot2"].get( "" );
    audioSounds[Vehicle::EVENT_SHOT3]      = soundsConfig["shot3"].get( "" );
    audioSounds[Vehicle::EVENT_SHOT_EMPTY] = soundsConfig["shotEmpty"].get( "" );

    context.usedSounds.include( audioSounds[Vehicle::EVENT_ENGINE], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Vehicle::EVENT_SHOT0], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Vehicle::EVENT_SHOT1], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Vehicle::EVENT_SHOT2], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Vehicle::EVENT_SHOT3], name + " (Object class)" );
    context.usedSounds.include( audioSounds[Vehicle::EVENT_SHOT_EMPTY], name + " (Object class)" );
  }

  state = 0;

  SET_STATE( Vehicle::CREW_VISIBLE_BIT, "state.crewVisible", false );
  SET_STATE( Vehicle::HAS_EJECT_BIT,    "state.hasEject",    false );
  SET_STATE( Vehicle::AUTO_EJECT_BIT,   "state.autoEject",   false );

  if( ( state & Vehicle::AUTO_EJECT_BIT ) && !( state & Vehicle::HAS_EJECT_BIT ) ) {
    OZ_ERROR( "%s: Vehicle has state.autoEject but not state.hasEject.", className );
  }

  bool isTypedetermined = false;

  if( config.contains( "turret" ) ) {
    type = VehicleClass::TURRET;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "wheeled" ) ) {
    type = VehicleClass::WHEELED;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "tracked" ) ) {
    type = VehicleClass::TRACKED;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "mech" ) ) {
    type = VehicleClass::MECH;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "hover" ) ) {
    type = VehicleClass::HOVER;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "air" ) ) {
    type = VehicleClass::AIR;
    isTypedetermined = true;
  }
  else if( !isTypedetermined && config.contains( "sub" ) ) {
    type = VehicleClass::SUB;
    isTypedetermined = true;
  }

  if( !isTypedetermined ) {
    OZ_ERROR( "%s: Vehicle configuration must contain exactly one of the objects 'turret',"
              " 'wheeled', 'tracked', 'mech', 'hover', 'air', 'sub' that contains configuration for"
              " the selected vehicle type.", className );
  }

  fuel     = config["fuel"].get( 100.0f );

  pilotPos = config["pilotPos"].get( Vec3::ZERO );
  lookHMin = config["lookHMin"].get( -120.0f );
  lookHMax = config["lookHMax"].get( +120.0f );
  lookVMin = config["lookVMin"].get( -60.0f );
  lookVMax = config["lookVMax"].get( +60.0f );

  if( lookHMin < -180.0f || lookHMin > 0.0f ) {
    OZ_ERROR( "%s: lookHMin must lie on interval [-180.0, 0.0]", className );
  }
  if( lookHMax < 0.0f || lookHMax > 180.0f ) {
    OZ_ERROR( "%s: lookHMax must lie on interval [0.0, 180.0]", className );
  }
  if( lookVMin < -90.0f || lookVMin > 90.0f || lookVMin > lookVMax ) {
    OZ_ERROR( "%s: lookVMin must lie on interval [-90.0, 90.0] and must not be greater than"
              " lookVMax", className );
  }
  if( lookVMax < -90.0f || lookVMax > 90.0f ) {
    OZ_ERROR( "%s: lookVMax must lie on interval [-90.0, 90.0] and must not be less than lookVMin",
              className );
  }

  lookHMin = Math::rad( lookHMin );
  lookHMax = Math::rad( lookHMax );
  lookVMin = Math::rad( lookVMin );
  lookVMax = Math::rad( lookVMax );

  rotVelLimit = Math::rad( config["rotVelLimit"].get( 60.0f ) ) * Timer::TICK_TIME;

  if( fuel <= 0.0f || !Math::isFinite( fuel ) ) {
    OZ_ERROR( "%s: Invalid fuel value. Should be > 0 and finite. If you want infinite fuel rather"
              " set fuelConsumption to zero.", className );
  }

  const JSON& weaponsConfig = config["weapons"];
  nWeapons = max( weaponsConfig.length(), 0 );

  if( nWeapons > VehicleClass::MAX_WEAPONS ) {
    OZ_ERROR( "%s: Vehicle must have between 0 and %d weapons.",
              className, VehicleClass::MAX_WEAPONS );
  }

  for( int i = 0; i < VehicleClass::MAX_WEAPONS; ++i ) {
    weaponTitles[i]        = "";
    onWeaponShot[i]        = "";
    nWeaponRounds[i]       = 0;
    weaponShotIntervals[i] = 0.0f;
  }

  for( int i = 0; i < nWeapons; ++i ) {
    weaponTitles[i]        = weaponsConfig[i]["title"].get( "" );
    onWeaponShot[i]        = weaponsConfig[i]["onShot"].get( "" );
    nWeaponRounds[i]       = weaponsConfig[i]["nRounds"].get( -1 );
    weaponShotIntervals[i] = weaponsConfig[i]["shotInterval"].get( 0.5f );

    if( weaponTitles[i].isEmpty() ) {
      OZ_ERROR( "%s: Missing weapon #%d title.", className, i );
    }
    if( onWeaponShot[i].isEmpty() ) {
      OZ_ERROR( "%s: Missing weapon #%d handler function.", className, i );
    }

    flags |= Object::LUA_BIT;
  }

  const JSON& engineConfig = config["engine"];

  engineConsumption     = engineConfig["consumption"].get( 0.0f ) * Timer::TICK_TIME;
  engineIdleConsumption = engineConfig["idleConsumption"].get( 0.0f ) * Timer::TICK_TIME;
  enginePitchBias       = engineConfig["pitchBias"].get( 1.0f );
  enginePitchRatio      = engineConfig["pitchRatio"].get( 0.001f );
  enginePitchLimit      = engineConfig["pitchLimit"].get( 2.00f );

  switch( type ) {
    case VehicleClass::TURRET: {
      break;
    }
    case VehicleClass::WHEELED: {
      const JSON& wheeledConfig = config["wheeled"];

      moveMomentum = wheeledConfig["moveMomentum"].get( 2.0f );
      break;
    }
    case VehicleClass::TRACKED: {
      const JSON& trackedConfig = config["tracked"];

      moveMomentum = trackedConfig["moveMomentum"].get( 2.0f );
      break;
    }
    case VehicleClass::MECH: {
      const JSON& mechConfig = config["mech"];

      walkMomentum   = mechConfig["walkMomentum"].get( 1.0f );
      runMomentum    = mechConfig["runMomentum"].get( 2.0f );

      stepWalkInc    = mechConfig["stepWalkInc"].get( 6.0f / 6.0f ) * Timer::TICK_TIME;
      stepRunInc     = mechConfig["stepRunInc"].get( 10.0f / 6.0f ) * Timer::TICK_TIME;

      stairInc       = mechConfig["stairInc"].get( 11.0f / 64.0f );
      stairMax       = mechConfig["stairMax"].get( 22.0f / 64.0f );
      stairRateLimit = mechConfig["stairRateLimit"].get( 0.15f );
      stairRateSupp  = mechConfig["stairRateSupp"].get( 0.80f );
      break;
    }
    case VehicleClass::HOVER: {
      const JSON& hoverConfig = config["hover"];

      moveMomentum           = hoverConfig["moveMomentum"].get( 2.0f );
      hoverHeight            = hoverConfig["height"].get( 2.0f );
      hoverHeightStiffness   = hoverConfig["heightStiffness"].get( 40.0f );
      hoverMomentumStiffness = hoverConfig["momentumStiffness"].get( 160.0f );
      break;
    }
    case VehicleClass::AIR: {
      const JSON& airConfig = config["air"];

      moveMomentum = airConfig["moveMomentum"].get( 2.0f );
      break;
    }
    case VehicleClass::SUB: {
      const JSON& subConfig = config["sub"];

      moveMomentum = subConfig["moveMomentum"].get( 2.0f );
      break;
    }
  }
}

void Class::writeObject( OutputStream* os )
{
  os->writeString( name );
  os->writeString( title );
  os->writeString( description );

  os->writeVec3( dim );
  os->writeInt( flags );
  os->writeFloat( life );
  os->writeFloat( resistance );

  os->writeInt( attributes );
  os->writeInt( key );
  os->writeInt( cost );

  os->writeInt( nItems );
  os->writeInt( defaultItems.length() );
  foreach( i, defaultItems.citer() ) {
    os->writeString( *i );
  }

  os->writeInt( nFrags );
  os->writeString( fragPool );

  os->writeString( deviceType );

  os->writeString( imagoType );
  os->writeString( imagoModel );

  os->writeString( audioType );
  for( int i = 0; i < ObjectClass::MAX_SOUNDS; ++i ) {
    os->writeString( audioSounds[i] );
  }

  os->writeString( onDestroy );
  os->writeString( onUse );
  os->writeString( onUpdate );
  os->writeString( getStatus );
}

void Class::writeDynamic( OutputStream* os )
{
  writeObject( os );

  os->writeFloat( mass );
  os->writeFloat( lift );
}

void Class::writeWeapon( OutputStream* os )
{
  writeDynamic( os );

  os->writeString( userBase );

  os->writeInt( nRounds );
  os->writeFloat( shotInterval );

  os->writeString( onShot );
}

void Class::writeBot( OutputStream* os )
{
  writeDynamic( os );

  os->writeInt( state );

  os->writeVec3( crouchDim );
  os->writeVec3( corpseDim );

  os->writeFloat( camZ );
  os->writeFloat( crouchCamZ );

  os->writeFloat( walkMomentum );
  os->writeFloat( runMomentum );
  os->writeFloat( jumpMomentum );

  os->writeFloat( airControl );
  os->writeFloat( ladderControl );
  os->writeFloat( waterControl );
  os->writeFloat( slickControl );

  os->writeFloat( stepWalkInc );
  os->writeFloat( stepRunInc );

  os->writeFloat( stairInc );
  os->writeFloat( stairMax );
  os->writeFloat( stairRateLimit );
  os->writeFloat( stairRateSupp );

  os->writeFloat( climbInc );
  os->writeFloat( climbMax );
  os->writeFloat( climbMomentum );

  os->writeFloat( stamina );
  os->writeFloat( staminaGain );
  os->writeFloat( staminaRunDrain );
  os->writeFloat( staminaWaterDrain );
  os->writeFloat( staminaClimbDrain );
  os->writeFloat( staminaJumpDrain );
  os->writeFloat( staminaThrowDrain );

  os->writeFloat( regeneration );

  os->writeFloat( reachDist );
  os->writeFloat( grabWeight );
  os->writeFloat( throwMomentum );

  os->writeInt( weaponItem );
  os->writeFloat( meleeInterval );
  os->writeString( onMelee );

  os->writeString( nameList );

  os->writeString( mindFunc );

  os->writeFloat( bobRotation );
  os->writeFloat( bobAmplitude );
  os->writeFloat( bobSwimAmplitude );

  os->writeMat44( baseColour );
  os->writeMat44( nvColour );
  os->writeMat44( injuryColour );
}

void Class::writeVehicle( OutputStream* os )
{
  writeDynamic( os );

  os->writeInt( type );
  os->writeInt( state );
  os->writeFloat( fuel );

  os->writeVec3( pilotPos );

  os->writeFloat( lookHMin );
  os->writeFloat( lookHMax );
  os->writeFloat( lookVMin );
  os->writeFloat( lookVMax );

  os->writeFloat( rotVelLimit );

  os->writeInt( nWeapons );

  for( int i = 0; i < nWeapons; ++i ) {
    os->writeString( weaponTitles[i] );
    os->writeString( onWeaponShot[i] );
    os->writeInt( nWeaponRounds[i] );
    os->writeFloat( weaponShotIntervals[i] );
  }

  os->writeFloat( engineConsumption );
  os->writeFloat( engineIdleConsumption );
  os->writeFloat( enginePitchBias );
  os->writeFloat( enginePitchRatio );
  os->writeFloat( enginePitchLimit );

  switch( type ) {
    case VehicleClass::TURRET: {
      break;
    }
    case VehicleClass::WHEELED: {
      os->writeFloat( moveMomentum );
      break;
    }
    case VehicleClass::TRACKED: {
      os->writeFloat( moveMomentum );
      break;
    }
    case VehicleClass::MECH: {
      os->writeFloat( walkMomentum );
      os->writeFloat( runMomentum );

      os->writeFloat( stepWalkInc );
      os->writeFloat( stepRunInc );

      os->writeFloat( stairInc );
      os->writeFloat( stairMax );
      os->writeFloat( stairRateLimit );
      os->writeFloat( stairRateSupp );
      break;
    }
    case VehicleClass::HOVER: {
      os->writeFloat( moveMomentum );

      os->writeFloat( hoverHeight );
      os->writeFloat( hoverHeightStiffness );
      os->writeFloat( hoverMomentumStiffness );
      break;
    }
    case VehicleClass::AIR: {
      os->writeFloat( moveMomentum );
      break;
    }
    case VehicleClass::SUB: {
      os->writeFloat( moveMomentum );
      break;
    }
  }
}

void Class::buildObjClass( OutputStream* os, const char* className )
{
  File configFile = String::str( "@class/%s.json", className );

  if( !config.load( configFile ) ) {
    OZ_ERROR( "Failed to load '%s'", configFile.path().cstr() );
  }

  const String& baseClass = config["base"].asString();

  names.add( className );
  bases.add( baseClass );

  if( baseClass.equals( "Object" ) ) {
    fillObject( className );
    writeObject( os );
  }
  else if( baseClass.equals( "Dynamic" ) ) {
    fillDynamic( className );
    writeDynamic( os );
  }
  else if( baseClass.equals( "Weapon" ) ) {
    fillWeapon( className );
    writeWeapon( os );
  }
  else if( baseClass.equals( "Bot" ) ) {
    fillBot( className );
    writeBot( os );
  }
  else if( baseClass.equals( "Vehicle" ) ) {
    fillVehicle( className );
    writeVehicle( os );
  }

  config.clear( true );
}

void Class::buildFragPool( OutputStream* os, const char* poolName )
{
  File configFile = String::str( "@frag/%s.json", poolName );

  JSON config;
  if( !config.load( configFile ) ) {
    OZ_ERROR( "Failed to load '%s'", configFile.path().cstr() );
  }

  int flags = 0;

  SET_FLAG( oz::FragPool::FADEOUT_BIT, "flag.fadeout", true );

  float velocitySpread = config["velocitySpread"].get( 4.0f );

  if( velocitySpread < 0.0f ) {
    OZ_ERROR( "%s: Frag velocitySpread must be >= 0.0", poolName );
  }

  float life       = config["life"].get( 4.0f );
  float lifeSpread = config["lifeSpread"].get( 1.0f );
  float mass       = config["mass"].get( 0.0f );
  float elasticity = config["elasticity"].get( 0.5f );

  if( life <= 0.0f ) {
    OZ_ERROR( "%s: Frag life must be > 0.0", poolName );
  }
  if( lifeSpread < 0.0f ) {
    OZ_ERROR( "%s: Frag lifeSpread must be >= 0.0", poolName );
  }
  if( mass < 0.0f ) {
    OZ_ERROR( "%s: Frag mass must be >= 0.0", poolName );
  }
  if( elasticity < 0.0f || 1.0f < elasticity ) {
    OZ_ERROR( "%s: Frag elasticity must lie on interval [0, 1]", poolName );
  }

  const JSON& modelsConfig = config["models"];
  int nModels = modelsConfig.length();

  List<String> models;

  for( int i = 0; i < nModels; ++i ) {
    const char* modelName = modelsConfig[i].get( "" );

    if( String::isEmpty( modelName ) ) {
      OZ_ERROR( "%s: Empty name for model #%d", poolName, i );
    }

    models.add( modelName );

    context.usedModels.include( modelName, String( poolName ) + " (Object class)" );
  }

  os->writeString( poolName );

  os->writeInt( flags );

  os->writeFloat( velocitySpread );

  os->writeFloat( life );
  os->writeFloat( lifeSpread );

  os->writeFloat( mass );
  os->writeFloat( elasticity );

  os->writeInt( models.length() );
  foreach( i, models.citer() ) {
    os->writeString( *i );
  }

  config.clear( true );
}

void Class::destroy()
{
  name        = "";
  title       = "";
  description = "";

  fragPool    = "";

  defaultItems.clear();
  defaultItems.deallocate();

  deviceType = "";

  imagoType  = "";
  imagoModel = "";

  audioType  = "";
  for( int i = 0; i < ObjectClass::MAX_SOUNDS; ++i ) {
    audioSounds[i] = "";
  }

  userBase = "";
  onShot   = "";

  mindFunc = "";
  nameList = "";

  for( int i = 0; i < VehicleClass::MAX_WEAPONS; ++i ) {
    weaponTitles[i] = "";
    onWeaponShot[i] = "";
  }

  names.clear();
  names.deallocate();
  bases.clear();
  bases.deallocate();
  devices.clear();
  devices.deallocate();
  imagines.clear();
  imagines.deallocate();
  audios.clear();
  audios.deallocate();
}

Class clazz;

}
}
