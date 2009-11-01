/*
 *  HealthArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "HealthArea.h"

#include "matrix/BotClass.h"
#include "client/Camera.h"

#include <GL/glu.h>

namespace oz
{
namespace client
{
namespace ui
{

  void HealthArea::draw()
  {
    if( camera.bot != null ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( 0, 30, 250, 20 );
      rect( 0,  0, 250, 20 );

      const BotClass *clazz = static_cast<const BotClass*>( camera.bot->type );

      float life         = ( camera.bot->life - clazz->life / 2.0f ) / ( clazz->life / 2.0f );
      int   lifeWidth    = max( static_cast<int>( life * 248.0f ), 0 );
      float stamina      = camera.bot->stamina / clazz->stamina;
      int   staminaWidth = max( static_cast<int>( stamina * 248.0f ), 0 );

      glColor4f( 1.0f - life, life, 0.0f, 0.6f );
      fill( 1, 31, lifeWidth, 18 );
      glColor4f( 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
      fill( 1, 1, staminaWidth, 18 );
    }
  }

}
}
}
