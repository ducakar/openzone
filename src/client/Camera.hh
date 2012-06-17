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

#include "client/StrategicProxy.hh"
#include "client/BotProxy.hh"

namespace oz
{
namespace client
{

class Camera
{
  public:

    static const float ROT_LIMIT;
    static const float MIN_DISTANCE;
    static const float SMOOTHING_COEF;

    enum State
    {
      NONE,
      STRATEGIC,
      BOT
    };

    static StrategicProxy strategicProxy;
    static BotProxy       botProxy;

    Proxy*        proxy;

  public:

    Point         p;
    Vec3          velocity;

    // Absolute rotation except when in a vehicle it is relative to vehicle orientation (when
    // looking forward).
    float         h;
    float         v;
    float         w;
    float         mag;

    // camera rotation change (from input)
    float         relH;
    float         relV;

    // global rotation quaternion, matrix and it's inverse
    Quat          rot;
    Mat44         rotMat;
    Mat44         rotTMat;

    Vec3          right;
    Vec3          up;
    Vec3          at;

    int           object;
    const Object* objectObj;

    int           entity;
    const Entity* entityObj;

    int           bot;
    Bot*          botObj;

    int           vehicle;
    Vehicle*      vehicleObj;

    int           width;
    int           height;
    int           centreX;
    int           centreY;

    float         coeff;
    float         aspect;
    float         vertPlane;
    float         horizPlane;
    float         maxDist;

    float         mouseXSens;
    float         mouseYSens;
    float         keyXSens;
    float         keyYSens;

    bool          isExternal;
    bool          allowReincarnation;
    bool          nightVision;

    State         state;
    State         newState;

    void setState( State state )
    {
      newState = state;
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

    void rotate( float h_, float v_ )
    {
      h = h_;
      v = v_;
    }

    void smoothRotate( float h_, float v_ )
    {
      h = Math::mix( h, h_, SMOOTHING_COEF );
      v = Math::mix( v, v_, SMOOTHING_COEF );
    }

    void move( const Point& pos )
    {
      Point oldP = p;

      p        = pos;
      velocity = ( p - oldP ) / Timer::TICK_TIME;
    }

    void smoothMove( const Point& pos )
    {
      Point oldP = p;

      p        = Math::mix( p, pos, SMOOTHING_COEF );
      velocity = ( p - oldP ) / Timer::TICK_TIME;
    }

    void warp( const Point& pos )
    {
      p        = pos;
      velocity = Vec3::ZERO;
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

    void init( int screenWidth, int screenHeight );

};

extern Camera camera;

}
}
