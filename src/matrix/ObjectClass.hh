/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file matrix/ObjectClass.hh
 */

#pragma once

#include "matrix/common.hh"

#define OZ_CLASS_SET_FLAG( flagBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    flags |= flagBit; \
  }

namespace oz
{
namespace matrix
{

class Object;
class FragPool;

class ObjectClass
{
  public:

    static const int MAX_ITEMS        = 100;
    static const int MAX_SOUNDS       = 16;

    static const int NIGHT_VISION_BIT = 0x01;
    static const int BINOCULARS_BIT   = 0x02;
    static const int GALILEO_BIT      = 0x04;
    static const int MUSIC_PLAYER_BIT = 0x08;
    static const int SUICIDE_BIT      = 0x10;

    typedef ObjectClass* CreateFunc();

    String          name;
    String          title;
    String          description;

    Vec3            dim;
    int             flags;
    float           life;
    float           resistance;

    int             attributes;
    int             key;

    const FragPool* fragPool;
    int             nFrags;

    int             nItems;

    Vector<const ObjectClass*> defaultItems;

    int             deviceType;

    int             imagoType;
    int             imagoModel;

    int             audioType;
    int             audioSounds[MAX_SOUNDS];

    String          onDestroy;
    String          onDamage;
    String          onHit;
    String          onUse;
    String          onUpdate;

  protected:

    void fillCommonConfig( const Config* config );

  public:

    virtual ~ObjectClass();

    static ObjectClass* createClass();

    virtual void initClass( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( InputStream* istream ) const;

};

}
}

/**
 * @page ObjectClass Static Objects
 */
