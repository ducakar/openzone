/*
 *  Camera.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/Matrix.hpp"
#include "client/FreeCamProxy.hpp"
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
        FREECAM,
        STRATEGIC,
        BOT
      };

    private:

      float mouseXSens;
      float mouseYSens;
      float keyXSens;
      float keyYSens;
      float smoothCoef;
      float smoothCoef_1;

      Vec3  newP;
      Vec3  oldP;

      static FreeCamProxy   freeCamProxy;
      static StrategicProxy strategicProxy;
      static BotProxy       botProxy;

      Proxy* proxy;

    public:

      Vec3  p;

      // relative to the object the camera is bound to
      float h;
      float v;
      float w;

      Quat  relRot;
      Quat  rot;

      // global rotation matrix and it's inverse
      Mat44 rotMat;
      Mat44 rotTMat;

      Vec3  right;
      Vec3  at;
      Vec3  up;

      int   tagged;
      const Object* taggedObj;

      int   bot;
      const Bot* botObj;

      State state;
      State newState;

      int   width;
      int   height;
      int   centerX;
      int   centerY;

      float angle;
      float aspect;
      float minDist;
      float maxDist;

      bool  isExternal;

      void setState( State state )
      {
        newState = state;
      }

      void setTagged( const Object* obj )
      {
        tagged    = obj == null ? -1 : obj->index;
        taggedObj = obj;
      }

      void setBot( const Bot* bot_ )
      {
        bot    = bot_ == null ? -1 : bot_->index;
        botObj = bot_;

        assert( botObj == null || ( botObj->flags & Object::BOT_BIT ) );
      }

      void move( const Vec3& pos )
      {
        p    = pos * smoothCoef_1 + oldP * smoothCoef;
        newP = pos;
        oldP = p;
      }

      void wrapMoveZ( const Vec3& pos )
      {
        p.x  = pos.x;
        p.y  = pos.y;
        p.z  = pos.z * smoothCoef_1 + oldP.z * smoothCoef;
        newP = pos;
        oldP = p;
      }

      void warp( const Vec3& pos )
      {
        oldP = pos;
        newP = pos;
        p    = pos;
      }

      void align();

      void update();
      void prepare();

      void init();

  };

  extern Camera camera;

}
}
