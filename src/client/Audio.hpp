/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Audio.hpp
 *
 * Audio analogue to Imago class.
 */

#pragma once

#include "matrix/Vehicle.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

class Audio
{
  public:

    static const int   UPDATED_BIT = 0x00000001;

    static const float REFERENCE_DISTANCE;
    static const float ROLLOFF_FACTOR;
    static const float COCKPIT_GAIN_FACTOR;
    static const float COCKPIT_PITCH_FACTOR;

    typedef Audio* ( * CreateFunc )( const Object* object );

  protected:

    // obj: source object of the effect, parent: object at which the effect is played
    // obj != parent: e.g. an object obj in the inventory of bot parent plays a sound
    void playSound( int sound, float volume, const Object* obj, const Object* parent ) const;
    void playContSound( int sound, float volume, const Object* obj, const Object* parent ) const;
    void playEngineSound( int sound, float volume, float pitch, const Vehicle* veh ) const;

    explicit Audio( const Object* obj );

  public:

    const Object*      obj;
    const ObjectClass* clazz;
    int                flags;

    virtual ~Audio();

    virtual void play( const Audio* parent ) = 0;

};

}
}
