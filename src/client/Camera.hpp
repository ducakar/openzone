/*
 *  Camera.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Bot.hpp"

#include "client/StrategicProxy.hpp"
#include "client/BotProxy.hpp"

namespace oz
{
namespace client
{

  class Camera
  {
    public:

      enum State
      {
        NONE,
        STRATEGIC,
        BOT
      };

    private:

      float mouseXSens;
      float mouseYSens;
      float keyXSens;
      float keyYSens;
      float smoothCoef;

      static StrategicProxy strategicProxy;
      static BotProxy       botProxy;

      Proxy* proxy;

    public:

      Point3 p;
      Point3 newP;
      Point3 oldP;

      // relative to the object the camera is bound to
      float  h;
      float  v;
      float  w;
      float  relH;
      float  relV;

      Quat   relRot;
      Quat   rot;

      // global rotation matrix and it's inverse
      Mat44  rotMat;
      Mat44  rotTMat;

      Vec3   right;
      Vec3   up;
      Vec3   at;

      int    tagged;
      const Object* taggedObj;

      int    bot;
      Bot*   botObj;

      State  state;
      State  newState;
      State  defaultState;

      int    width;
      int    height;
      int    centreX;
      int    centreY;

      float  coeff;
      float  aspect;
      float  vertPlane;
      float  horizPlane;
      float  minDist;
      float  maxDist;

      bool   isExternal;
      bool   allowReincarnation;

      void setState( State state )
      {
        newState = state;
      }

      void setTagged( const Object* obj )
      {
        tagged    = obj == null ? -1 : obj->index;
        taggedObj = obj;
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

      void move( const Point3& pos )
      {
        p    = pos + smoothCoef * ( oldP - pos );
        newP = pos;
        oldP = p;
      }

      void warp( const Point3& pos )
      {
        oldP = pos;
        newP = pos;
        p    = pos;
      }

      void warpMoveZ( const Point3& pos )
      {
        p.x  = pos.x;
        p.y  = pos.y;
        p.z  = pos.z + smoothCoef * ( oldP.z - pos.z );
        newP = pos;
        oldP = p;
      }

      void align();
      void update();
      void prepare();

      void read( InputStream* istream );
      void write( OutputStream* ostream ) const;

      void clear();

      void init();

  };

  extern Camera camera;

}
}
