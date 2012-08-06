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
 * @file client/Camera.hh
 */

#pragma once

#include "matrix/Vehicle.hh"

#include "client/StrategicProxy.hh"
#include "client/UnitProxy.hh"
#include "client/CinematicProxy.hh"

namespace oz
{
namespace client
{

class Camera
{
  public:

    static const float  ROT_LIMIT;
    static const float  MIN_DISTANCE;
    static const float  SMOOTHING_COEF;
    static const float  ROT_SMOOTHING_COEF;
    static const Mat44  NV_COLOUR;
    static Proxy* const PROXIES[];

    enum State
    {
      NONE,
      STRATEGIC,
      UNIT,
      CINEMATIC
    };

    static StrategicProxy strategic;
    static UnitProxy      unit;
    static CinematicProxy cinematic;

    Proxy*        proxy;

  public:

    // Rotation, magnification, position and velocity.
    Quat          rot;
    float         mag;
    Point         p;
    Vec3          velocity;

    // Current rotation, magnification and position are smoothly changed to match desired ones.
    Quat          desiredRot;
    float         desiredMag;
    Point         desiredPos;
    Point         oldPos;

    // Camera rotation change (from input).
    float         relH;
    float         relV;

    // Global rotation quaternion, matrix and it's inverse.
    Mat44         rotMat;
    Mat44         rotTMat;

    // Global colour transformation.
    Mat44         colour;

    Mat44         baseColour;
    Mat44         nvColour;

    Vec3          right;
    Vec3          up;
    Vec3          at;

    int           width;
    int           height;
    int           centreX;
    int           centreY;

    float         coeff;
    float         aspect;
    float         vertPlane;
    float         horizPlane;
    float         maxDist;

    int           object;
    const Object* objectObj;

    int           entity;
    const Entity* entityObj;

    int           bot;
    Bot*          botObj;

    int           vehicle;
    Vehicle*      vehicleObj;

    List<int>     switchableUnits;

    bool          isFixedAspect;
    bool          allowReincarnation;
    bool          nightVision;
    bool          isExternal;

    State         state;
    State         newState;

    void setState( State state_ )
    {
      newState = state_;
    }

    void setTaggedObj( const Object* obj )
    {
      object    = obj == null ? -1 : obj->index;
      objectObj = obj;
    }

    void setTaggedEnt( const Entity* ent )
    {
      entityObj = ent;

      if( ent == null ) {
        entity = -1;
      }
      else {
        int strIndex = ent->str->index;
        int entIndex = int( ent - ent->str->entities );

        entity = strIndex * Struct::MAX_ENTITIES + entIndex;
      }
    }

    void setBot( Bot* botObj_ )
    {
      if( botObj != null ) {
        botObj->state &= ~Bot::PLAYER_BIT;
      }

      if( botObj_ == null ) {
        bot    = -1;
        botObj = null;
      }
      else {
        bot    = botObj_->index;
        botObj = botObj_;

        botObj_->state |= Bot::PLAYER_BIT;
      }

      hard_assert( botObj == null || ( botObj->flags & Object::BOT_BIT ) );
    }

    void rotateTo( const Quat& q )
    {
      rot        = q;
      desiredRot = q;
    }

    void smoothRotateTo( const Quat& q )
    {
      desiredRot = q;
    }

    void moveTo( const Point& p_ )
    {
      p          = p_;
      desiredPos = p_;
    }

    void magnify( float mag_ )
    {
      mag        = mag_;
      desiredMag = mag_;
    };

    void smoothMagnify( float mag_ )
    {
      desiredMag = mag_;
    }

    void smoothMoveTo( const Point& p_ )
    {
      desiredPos = p_;
    }

    void warpTo( const Point& p_ )
    {
      p          = p_;
      desiredPos = p_;
      oldPos     = p_;
    }

    void updateReferences();

    /**
     * Re-calculate rotation quaternion, matrices and <tt>at</tt>, <tt>up</tt>, <tt>left</tt>
     * vectors.
     */
    void align();

    /**
     * Process input.
     *
     * This pass is performed before matrix update. It should e.g. update player's bot orientation
     * and actions.
     */
    void prepare();

    /**
     * Update camera after matrix simulation step.
     *
     * This pass should align camera to match bot position and orientation etc.
     */
    void update();

    void reset();

    void read( InputStream* istream );
    void write( BufferStream* ostream ) const;

    void init();

};

extern Camera camera;

}
}
