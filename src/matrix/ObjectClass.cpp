/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/ObjectClass.hpp"

#include "matrix/Library.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{

  void ObjectClass::fillCommonConfig( const Config* config )
  {
    /*
     * name
     */

    name        = ( *config )["name"];
    title       = gettext( config->get( "title", name ) );
    description = gettext( config->get( "description", "" ) );

    if( name.isEmpty() ) {
      throw Exception( "Empty class name" );
    }

    /*
     * dim
     */

    dim.x = config->get( "dim.x", 0.50f );
    dim.y = config->get( "dim.y", 0.50f );
    dim.z = config->get( "dim.z", 0.50f );

    if( dim.x < 0.0f || dim.x > AABB::REAL_MAX_DIM ||
        dim.y < 0.0f || dim.y > AABB::REAL_MAX_DIM ||
        dim.z < 0.0f )
    {
      throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
    }

    if( ( flags & Object::CYLINDER_BIT ) && dim.x != dim.y ) {
      throw Exception( "Cylindric object '" + name + "' should have dim.x == dim.y" );
    }

    /*
     * handler functions
     */

    onDestroy = config->get( "onDestroy", "" );
    onDamage  = config->get( "onDamage", "" );
    onHit     = config->get( "onHit", "" );
    onUse     = config->get( "onUse", "" );
    onUpdate  = config->get( "onUpdate", "" );

    if( !onDestroy.isEmpty() ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.destroyFunc", true ) ) {
        flags &= ~Object::DESTROY_FUNC_BIT;
      }
      else {
        flags |= Object::LUA_BIT | Object::DESTROY_FUNC_BIT;;
      }
    }
    if( !onDamage.isEmpty() ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.damageFunc", true ) ) {
        flags &= ~Object::DAMAGE_FUNC_BIT;
      }
      else {
        flags |= Object::LUA_BIT | Object::DAMAGE_FUNC_BIT;
      }
    }
    if( !onHit.isEmpty() ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.hitFunc", true ) ) {
        flags &= ~Object::HIT_FUNC_BIT;
      }
      else {
        flags |= Object::LUA_BIT | Object::HIT_FUNC_BIT;
      }
    }
    if( !onUse.isEmpty() ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.useFunc", true ) ) {
        flags &= ~Object::USE_FUNC_BIT;
      }
      else if( onUse.equals( "INSTRUMENT" ) ) {
        onUse.clear();
        flags |= Object::USE_FUNC_BIT;
      }
      else {
        flags |= Object::LUA_BIT | Object::USE_FUNC_BIT;
      }
    }
    if( !onUpdate.isEmpty() ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.updateFunc", true ) ) {
        flags &= ~Object::UPDATE_FUNC_BIT;
      }
      else {
        flags |= Object::LUA_BIT | Object::UPDATE_FUNC_BIT;
      }
    }

    /*
     * life
     */

    life            = config->get( "life", 100.0f );
    damageThreshold = config->get( "damageThreshold", 100.0f );

    if( life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( damageThreshold < 0.0f ) {
      throw Exception( "Invalid object damageThreshold. Should be >= 0." );
    }

    /*
     * debris
     */

    nDebris              = config->get( "nDebris", 8 );
    debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    debrisRejection      = config->get( "debrisRejection", 1.80f );
    debrisMass           = config->get( "debrisMass", 0.0f );
    debrisLifeTime       = config->get( "debrisLifeTime", 2.0f );
    debrisColour.x       = config->get( "debrisColour.r", 0.5f );
    debrisColour.y       = config->get( "debrisColour.g", 0.5f );
    debrisColour.z       = config->get( "debrisColour.b", 0.5f );
    debrisColourSpread   = config->get( "debrisColourSpread", 0.1f );

    /*
     * model
     */

    modelType = config->get( "modelType", "" );

    if( !modelType.isEmpty() ) {
      flags |= Object::MODEL_BIT;

      const char* modelName = config->get( "modelName", "" );
      modelIndex = modelName[0] == '\0' ? -1 : library.modelIndex( modelName );
    }
    else {
      modelIndex = -1;
    }

    /*
     * audio
     */

    audioType = config->get( "audioType", "" );

    if( !audioType.isEmpty() ) {
      flags |= Object::AUDIO_BIT;

      char buffer[] = "audioSample  ";
      for( int i = 0; i < AUDIO_SAMPLES; ++i ) {
        hard_assert( i < 100 );

        buffer[11] = char( '0' + ( i / 10 ) );
        buffer[12] = char( '0' + ( i % 10 ) );

        const char* sampleName = config->get( buffer, "" );
        audioSamples[i] = sampleName[0] == '\0' ? -1 : library.soundIndex( sampleName );
      }
    }
    else {
      for( int i = 0; i < AUDIO_SAMPLES; ++i ) {
        audioSamples[i] = -1;
      }
    }

    /*
     * inventory
     */

    nItems = config->get( "nItems", 0 );

    if( nItems != 0 ) {
      flags |= Object::BROWSABLE_BIT;
    }

    if( nItems < 0 ) {
      throw Exception( "Inventory size must be 0 or a positive integer" );
    }

    // default inventory
    char buffer[] = "item  ";
    for( int i = 0; i < INVENTORY_ITEMS; ++i ) {
      hard_assert( i < 100 );

      buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
      buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

      String itemName = config->get( buffer, "" );
      if( !itemName.isEmpty() ) {
        items.add( itemName );
      }
    }

    if( ( flags & Object::ITEM_BIT ) && nItems != 0 ) {
      throw Exception( "Object cannot be an item and have an inventory at the same time" );
    }
    if( items.length() > nItems ) {
      throw Exception( "More objects in the default inventory than the inventory size" );
    }
  }

  void ObjectClass::fillCommonFields( Object* obj ) const
  {
    obj->dim      = dim;
    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->clazz    = this;
    obj->life     = life;

    for( int i = 0; i < items.length(); ++i ) {
      int index = synapse.addObject( items[i], Point3::ORIGIN );
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[index] );

      hard_assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      obj->items.add( index );
      item->parent = obj->index;
      synapse.cut( item );
    }
  }

  ObjectClass::~ObjectClass()
  {}

  ObjectClass* ObjectClass::init( const Config* config )
  {
    ObjectClass* clazz = new ObjectClass();

    clazz->flags = 0;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.destroyFunc",   true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.damageFunc",    false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.hitFunc",       false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.useFunc",       false );
    OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.updateFunc",    false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
    OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );
    OZ_CLASS_SET_FLAG( Object::RANDOM_HEADING_BIT, "flag.randomHeading", false );

    clazz->fillCommonConfig( config );

    return clazz;
  }

  Object* ObjectClass::create( int index, const Point3& pos ) const
  {
    Object* obj = new Object();

    hard_assert( obj->index == -1 && obj->cell == null );

    obj->p     = pos;
    obj->index = index;

    fillCommonFields( obj );

    return obj;
  }

  Object* ObjectClass::create( int index, InputStream* istream ) const
  {
    Object* obj = new Object();

    obj->dim   = dim;

    obj->index = index;
    obj->clazz = this;

    obj->readFull( istream );

    return obj;
  }

}
