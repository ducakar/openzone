/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/ObjectClass.hpp
 */

#pragma once

#include "matrix/common.hpp"

#define OZ_CLASS_SET_FLAG( flagBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->flags |= flagBit; \
  }

namespace oz
{
namespace matrix
{

class Object;

class ObjectClass
{
  private:

    static const int INVENTORY_ITEMS = 100;

  public:

    // 00 <= AUDIO_SOUNDS <= 99 (two decimal digits)
    static const int AUDIO_SOUNDS = 16;

    typedef ObjectClass* ( * InitFunc )( const Config* config );

    String name;
    String title;
    String description;

    Vec3   dim;
    int    flags;
    int    type;
    float  life;
    float  resistance;

    String onDestroy;
    String onDamage;
    String onHit;
    String onUse;
    String onUpdate;

    int    nDebris;
    float  debrisVelocitySpread;
    float  debrisRejection;
    float  debrisMass;
    float  debrisLifeTime;
    float  debrisColourSpread;
    Vec3   debrisColour;

    String deviceType;

    String imagoType;
    int    imagoModel;

    String audioType;
    int    audioSounds[AUDIO_SOUNDS];

    int    nItems;
    Vector<String> items;

  protected:

    void fillCommonConfig( const Config* config );
    void fillCommonFields( Object* obj ) const;

  public:

    virtual ~ObjectClass();

    static ObjectClass* init( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( int index, InputStream* istream ) const;

};

}
}
