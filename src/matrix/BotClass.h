/*
 *  BotClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "DynObjectClass.h"
#include "Bot.h"

namespace oz
{

  // TODO BotClass
  struct BotClass : DynObjectClass
  {
    Vec3  dimCrouch;

    Vec3  camPos;
    Vec3  camPosCrouch;

    float bobInc;
    float bobAmplitude;

    float walkVelocity;
    float runVelociy;
    float crouchVelocity;
    float jumpVelocity;

    float stepInc;
    float stepMax;

    float airControl;
    float grabDistance;

    static Class *init( Config *config );

    Object *create( const Vec3 &pos );
  };

}
