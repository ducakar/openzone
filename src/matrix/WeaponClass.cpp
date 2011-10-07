/*
 *  WeaponClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/WeaponClass.hpp"

#include "matrix/Weapon.hpp"
#include "matrix/Library.hpp"

namespace oz
{

  ObjectClass* WeaponClass::init( const Config* config )
  {
    WeaponClass* clazz = new WeaponClass();

    clazz->flags = Object::DYNAMIC_BIT | Object::WEAPON_BIT | Object::ITEM_BIT |
        Object::UPDATE_FUNC_BIT | Object::USE_FUNC_BIT;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",    true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",     false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",        false );
    OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.onUpdate",     false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
    OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",     true  );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

    clazz->fillCommonConfig( config );

    clazz->mass = config->get( "mass", 100.0f );
    clazz->lift = config->get( "lift", 12.0f );

    if( clazz->mass < 0.01f ) {
      throw Exception( "Invalid object mass. Should be >= 0.01." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    clazz->onShot = config->get( "onShot", "" );

    if( !String::isEmpty( clazz->onShot ) ) {
      clazz->flags |= Object::LUA_BIT;
    }

    clazz->nRounds      = config->get( "nRounds", -1 );
    clazz->shotInterval = config->get( "shotInterval", 0.5f );

    return clazz;
  }

  Object* WeaponClass::create( int index, const Point3& pos ) const
  {
    Weapon* obj = new Weapon();

    hard_assert( obj->index == -1 && obj->cell == null && obj->parent == -1 );

    obj->p        = pos;
    obj->index    = index;

    obj->mass     = mass;
    obj->lift     = lift;

    obj->nRounds  = nRounds;
    obj->shotTime = 0.0f;

    fillCommonFields( obj );

    return obj;
  }

  Object* WeaponClass::create( int index, InputStream* istream ) const
  {
    Weapon* obj = new Weapon();

    obj->dim   = dim;

    obj->index = index;
    obj->clazz = this;

    obj->mass  = mass;
    obj->lift  = lift;

    obj->readFull( istream );

    return obj;
  }

  void WeaponClass::fillAllowedUsers()
  {
    int underscore = name.index( '_' );
    if( underscore == -1 ) {
      throw Exception( "Weapon class file must be named <botClass>_weapon.<weapon>.rc" );
    }

    String matchClass = name.substring( 0, underscore );

    foreach( clazz, library.classes.citer() ) {
      String botClassBase = clazz.value()->name;

      int dot = botClassBase.index( '.' );
      if( dot != -1 ) {
        botClassBase = botClassBase.substring( 0, dot );
      }

      if( matchClass.equals( botClassBase ) ) {
        allowedUsers.add( clazz.value() );
      }
    }
  }

}
