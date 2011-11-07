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

#include "matrix/FragPool.hpp"

#define OZ_CLASS_SET_FLAG( flagBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    flags |= flagBit; \
  }

namespace oz
{
namespace matrix
{

class Object;

class ObjectClass
{
  private:

    static const int MAX_ITEMS = 100;

  public:

    // 00 <= AUDIO_SOUNDS <= 99 (two decimal digits)
    static const int MAX_SOUNDS = 16;

    typedef ObjectClass* ( * CreateFunc )();

    String          name;
    String          title;
    String          description;

    Vec3            dim;
    int             flags;
    float           life;
    float           resistance;

    String          onDestroy;
    String          onDamage;
    String          onHit;
    String          onUse;
    String          onUpdate;

    const FragPool* fragPool;
    int             nFrags;

    int             deviceType;

    int             imagoType;
    int             imagoModel;

    int             audioType;
    int             audioSounds[MAX_SOUNDS];

    int             nItems;

    Vector<const ObjectClass*> defaultItems;

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
